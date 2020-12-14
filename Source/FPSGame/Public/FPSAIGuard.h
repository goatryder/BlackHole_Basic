// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "FPSAIGuardController.h"

#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;
// class FPathFollowingResult;


UENUM(BlueprintType)
enum class EAIState : uint8
{

	Idle,
	Suspicious,
	Alerted,
	Patrolling,
	ChassingTarget,
	MovingBackToGuardPoint,
	LooseChassingTarget
};


UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
		void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
		void OnHearNoiseEvent(APawn* PawnNoise, const FVector& Location, float Volume);

	FRotator OriginalRotation;
	FVector OriginalPosition;

	UFUNCTION()
		void ResetOrientation();

	UPROPERTY(EditAnywhere, Category = "AI")
		bool bDebugPrint;

	UPROPERTY(EditAnywhere, Category = "AI")
		float BaseSpeed = 375.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		float NoPatrolResetOrientationAfterNoiseDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		TSubclassOf<AFPSAIGuardController> AIClass = AFPSAIGuardController::StaticClass();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		EAIState GuardState;

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
		void OnStateChanged(EAIState NewState);
	
	UPROPERTY(EditAnywhere, Category = "AI")
		bool bFailMissionWhenPlayerSpotted;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool bCanPatrol = true;
	
	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanPatrol"))
		AActor* PatrolPointStart;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanPatrol"))
		AActor* PatrolPointEnd;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanPatrol"))
		float PatrolPointAcceptanceRadius = 5.0f;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanPatrol"))
		bool bStopPatrollingWhenSuspicios = true;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanPatrol"))
		float MoveToNextPatrolPointDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanPatrol"))
		float ContinuePatrollingAfterSuspiciousDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool bCanChasePlayerWhenSpotted = true;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanChasePlayerWhenSpotted"))
		bool bFailMissionWheChasedPlayerCached = true;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanChasePlayerWhenSpotted"))
		float ChasingActorAcceptanceRadius = 50.0f;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanChasePlayerWhenSpotted"))
		bool AlertedToChassingDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanChasePlayerWhenSpotted"))
		float ChassingSpeed = 520.0f;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bCanChasePlayerWhenSpotted"))
		float TargetVisualSensetivity = 2.0f;

	AFPSAIGuardController* AIGuardController;

	APawn* PlayerPawnREF;

	bool bLastReachedPatrolPointWasStart;

	void MoveToTarget(AActor* TargetActor, float AcceptaceRadius);
	void SetPatrolling(bool bPatrolling, bool bStopImmediately);
	
	UFUNCTION()
		void ContinuePatrolling();
	
	UFUNCTION()
		void ChasePlayer();

	void MoveBackToGuardPoint();

	UFUNCTION()
		void ChaseCompleteHandle();

	UFUNCTION()
		void OnAIMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
	bool bTestCanSeePlayer;

	// not used
	float LostVisualDelta;
	
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
		bool bShowSightProgressBar;

	// not used
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (EditCondition = "bShowSightProgressBar"))
		float SightProgressBarVal = 0.f;

	// not used
	void UpdateSightProgressBarVal(float Delta);

	FTimerHandle TimerHandle_MoveTo;
	FTimerHandle TimerHandle_CheckPlayerInSight;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
