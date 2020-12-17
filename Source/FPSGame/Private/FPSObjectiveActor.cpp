// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"

#include "Components/SphereComponent.h"

#include "Kismet/GameplayStatics.h"

#include "FPSCharacter.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	PickupTimeAccomulation = PickupDestroyTime;
	InitScale = GetActorScale3D();

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();

	InitialZLocation = GetActorLocation().Z;

}

void AFPSObjectiveActor::PlayEffects()
{
	if (bCanPlayEffects) {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupFX, GetActorLocation());
		UGameplayStatics::PlaySound2D(GetWorld(), PickupSound);
		bCanPlayEffects = false;

	}

}

void AFPSObjectiveActor::DestroyAnim()
{
}

// Called every frame
void AFPSObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldDestroy) {

		PickupTimeAccomulation -= DeltaTime;

		if (PickupTimeAccomulation <= 0.f) {

			if (HasAuthority())
				Destroy();
			bShouldDestroy = false;

		}
		else {

			SetActorScale3D(InitScale * (PickupTimeAccomulation / PickupDestroyTime));

		}
	}
	else if (bShouldRotate) {

		RotateYawMoveZ(DeltaTime);

	}

}

void AFPSObjectiveActor::RotateYawMoveZ(float DeltaTime)
{
	// rotate yaw

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw += DeltaTime * RotationYawSpeed;
	SetActorRotation(Rotation);

	// Calc Delta Time To Float Movement For Move Up or Down Only When We Make 180 Rotation

	float FloatMovementDelta = RotationYawSpeed / 90.f * DeltaTime; // multiplyed by to, cos we move to -20 and to 20, so it's double distance and shoold be double speed

	// UE_LOG(LogTemp, Warning, TEXT("rotation delta %f, move delta %f"), DeltaTime, FloatMovementDelta);

	// float movement on z axis

	FVector ActorLoc = GetActorLocation();

	if (ActorLoc.Z > InitialZLocation + OnRotationMoveZMagnitude)
		bOnRotationMoveZPositive = false;
	else if (ActorLoc.Z < InitialZLocation - OnRotationMoveZMagnitude)
		bOnRotationMoveZPositive = true;

	if (bOnRotationMoveZPositive)
		ActorLoc.Z += OnRotationMoveZMagnitude * FloatMovementDelta;
	else
		ActorLoc.Z -= OnRotationMoveZMagnitude * FloatMovementDelta;

	SetActorLocation(ActorLoc);

}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{

	AActor::NotifyActorBeginOverlap(OtherActor);


	AFPSCharacter* MyCharacter = Cast<AFPSCharacter>(OtherActor);

	if (MyCharacter) {

		if (HasAuthority())
			MyCharacter->bIsCarryingObjective = true;

		PlayEffects();
		bShouldDestroy = true;

	}

}

