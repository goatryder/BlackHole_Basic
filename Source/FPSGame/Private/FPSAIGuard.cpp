// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"

#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnHearNoiseEvent);

	AIControllerClass = AIClass;
	// TODO: VISIBILITY LOST CHASING DELAY
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	AIGuardController = Cast<AFPSAIGuardController>(GetController());

	if (AIGuardController)
		AIGuardController->ReceiveMoveCompleted.AddDynamic(this, &AFPSAIGuard::OnAIMoveComplete);

	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;

	OriginalRotation = GetActorRotation();
	OriginalPosition = GetActorLocation();

	if (bCanPatrol)
		SetPatrolling(true, false);

}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{

	if (GuardState == EAIState::Alerted || GuardState == EAIState::ChassingTarget)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Purple, false, 5.0f);
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("AI SAW %s"), *SeenPawn->GetName()));

	if (GuardState == EAIState::Patrolling)
		SetPatrolling(false, true);

	SetGuardState(EAIState::Alerted);
	
	PlayerPawnREF = SeenPawn;

	if (bCanChasePlayerWhenSpotted) {

		GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
		GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::ChasePlayer, AlertedToChassingDelay);
	
	} // chase player
	
	else {

		if (bFailMissionWhenPlayerSpotted)
		{

			AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

			if (GM && SeenPawn)
				GM->CompleteMission(SeenPawn, false);

		}

	} // game over
}

void AFPSAIGuard::OnHearNoiseEvent(APawn* PawnNoise, const FVector& Location, float Volume)
{

	if (GuardState == EAIState::Alerted || GuardState == EAIState::ChassingTarget)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Red, false, 5.0f);

	if (GuardState == EAIState::Patrolling) {
		SetPatrolling(false, true);
	}

	SetGuardState(EAIState::Suspicious);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	if (bCanPatrol) {

		GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
		GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::ContinuePatrolling, MoveToNextPatrolPointDelay);
	
	}

	else {

		GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
		GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::ResetOrientation, NoPatrolResetOrientationAfterNoiseDelay);

	}

}

void AFPSAIGuard::ResetOrientation()
{

	SetActorRotation(OriginalRotation);

	if (GuardState != EAIState::Alerted)
		SetGuardState(EAIState::Idle);

}

void AFPSAIGuard::OnRep_GuardState()
{

	OnStateChanged(GuardState);

}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{

	if (GuardState == NewState) {

		return;

	}

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::Printf(TEXT("NewState: %d"), NewState));

	GuardState = NewState;
	OnRep_GuardState();

	// OnStateChanged(GuardState);

}

void AFPSAIGuard::MoveToTarget(AActor* TargetActor, float AcceptanceRadius)
{

	if (AIGuardController && TargetActor) {

		FAIMoveRequest MoveRequest;

		MoveRequest.SetGoalActor(TargetActor);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		AIGuardController->MoveTo(MoveRequest);
	}

	

}

void AFPSAIGuard::SetPatrolling(bool bPatrolling, bool bStopImmediately = false)
{

	if (bDebugPrint)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("Patrol")));

	if (!bCanPatrol) {

		UE_LOG(LogTemp, Warning, TEXT("Set Patrolling Called but bCanPatrol is False! Please change it to true"));
		return;

	}
		

	if (bPatrolling) {
		
		SetGuardState(EAIState::Patrolling);

		MoveToTarget(bLastReachedPatrolPointWasStart ? PatrolPointEnd : PatrolPointStart, PatrolPointAcceptanceRadius);

	}
	else {

		SetGuardState(EAIState::Idle);

		if (bStopImmediately && AIGuardController)
			AIGuardController->StopMovement();
	}


}

void AFPSAIGuard::ContinuePatrolling()
{

	SetPatrolling(true, false);

}

void AFPSAIGuard::ChasePlayer()
{

	SetGuardState(EAIState::ChassingTarget);

	MoveToTarget(PlayerPawnREF, ChasingActorAcceptanceRadius);

	GetCharacterMovement()->MaxWalkSpeed = ChassingSpeed;

	//TimerHandle_CheckPlayerInSight.Invalidate();

	//TimerHandle_CheckPlayerInSight = FTimerHandle();
	//GetWorldTimerManager().ClearTimer(TimerHandle_CheckPlayerInSight);
	//GetWorldTimerManager().SetTimer(TimerHandle_CheckPlayerInSight, this, &AFPSAIGuard::CheckPlayerInSight, CheckIfSpottedTargetInSightDelay, true);

}

void AFPSAIGuard::MoveBackToGuardPoint()
{

	SetGuardState(EAIState::MovingBackToGuardPoint);

	if (AIGuardController) {

		FAIMoveRequest MoveRequest;

		MoveRequest.SetGoalLocation(OriginalPosition);
		MoveRequest.SetAcceptanceRadius(PatrolPointAcceptanceRadius);

		AIGuardController->MoveTo(MoveRequest);
	}

}

void AFPSAIGuard::ChaseCompleteHandle()
{

	if (GuardState != EAIState::LooseChassingTarget)
		return;

	if (PawnSensingComp->HasLineOfSightTo(PlayerPawnREF)) {

		if (bDebugPrint)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, *FString::Printf(TEXT("Player Stil In Sight, NEXT: Chase")));


		GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
		GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::ChasePlayer, 0.1f);

		// ChasePlayer();

	}
	else {
		
		// TimerHandle_CheckPlayerInSight.Invalidate();

		if (bCanPatrol) {

			if (bDebugPrint)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("Player Loose Sight, NEXT: Patrol")));
			
			GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
			GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::ContinuePatrolling, MoveToNextPatrolPointDelay);

		}

		else {

			if (bDebugPrint)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("Player Loose Sight, NEXT: Back To Guard Post")));

			GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
			GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::MoveBackToGuardPoint, MoveToNextPatrolPointDelay);

		}

	}

}

void AFPSAIGuard::OnAIMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{

	if (GuardState == EAIState::Patrolling) {
		
		bLastReachedPatrolPointWasStart = !bLastReachedPatrolPointWasStart;

		if (bCanPatrol) {
			
			if (bDebugPrint)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::Printf(TEXT("Patrol Point Reached, NEXT: Patrol")));
			
			GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
			GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::ContinuePatrolling, MoveToNextPatrolPointDelay);
		
		}

		else {

			if (bDebugPrint)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::Printf(TEXT("Patrol Point Reached, NEXT: Back To Guard Post")));
			
			GetWorldTimerManager().ClearTimer(TimerHandle_MoveTo);
			GetWorldTimerManager().SetTimer(TimerHandle_MoveTo, this, &AFPSAIGuard::MoveBackToGuardPoint, MoveToNextPatrolPointDelay);
		
		}
	
	} // patrolling moving complete

	else if (GuardState == EAIState::ChassingTarget) {

		if (bDebugPrint)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::Printf(TEXT("Chassing Player Complete, Reset Speed")));

		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;

		if (Result == EPathFollowingResult::Success && bFailMissionWheChasedPlayerCached) {

			if (bDebugPrint)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::Printf(TEXT("Chassing Player Success, Player Cached, Game Over")));

			AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());  

			if (GM && PlayerPawnREF) {

				GM->CompleteMission(PlayerPawnREF, false);

			}

		} // game over

		else {

			if (bDebugPrint)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::Printf(TEXT("Chassing Player Fail, Check if Can Chase")));
			
			SetGuardState(EAIState::LooseChassingTarget);

			GetWorldTimerManager().ClearTimer(TimerHandle_CheckPlayerInSight);
			GetWorldTimerManager().SetTimer(TimerHandle_CheckPlayerInSight, this, &AFPSAIGuard::ChaseCompleteHandle, 0.5f);

		} // handle chase fail

	} // chassing moving complete

	else if (GuardState == EAIState::MovingBackToGuardPoint) {

		if (bDebugPrint)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::Printf(TEXT("Moved back Guard Post, NEXT: Reset Orientation")));
		
		ResetOrientation();

	} // moving back to guard point complete

}

void AFPSAIGuard::UpdateSightProgressBarVal(float Delta)
{

	LostVisualDelta += Delta;

	// clamp
	if (LostVisualDelta >= TargetVisualSensetivity)
		LostVisualDelta = TargetVisualSensetivity;
	
	else if (LostVisualDelta <= 0.0f)
		LostVisualDelta = 0.0f;
	//

	// progress bar
	if (LostVisualDelta == 0.0f)
		SightProgressBarVal = 0.0f;
	else
		SightProgressBarVal = LostVisualDelta / TargetVisualSensetivity;
	//

}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*

	bool bCanSee = PawnSensingComp->HasLineOfSightTo(PlayerPawnREF);

	bTestCanSeePlayer = bCanSee;

	if (bCanSee) {

		if (bDebugPrint)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, *FString::Printf(TEXT("Can See")));

		//UpdateSightProgressBarVal(DeltaTime);

	}
		
	else {

		if (bDebugPrint)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, *FString::Printf(TEXT("Can't See")));

		//UpdateSightProgressBarVal(-DeltaTime);

	} */

	//if (bDebugPrint)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, *FString::Printf(TEXT("Lost Visual Delta %f"), LostVisualDelta));



	//if (bDebugPrint && bCanSee != bTestCanSeePlayer) {

	//	if (bCanSee) {

	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, *FString::Printf(TEXT("Can See")));
	//		
	//	}
	//	else {

	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("Can't See")));
	//		
	//	}
	//}

}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}