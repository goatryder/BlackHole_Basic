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
	MovingBackToGuardPoint
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

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
		void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
		void OnHearNoiseEvent(APawn* PawnNoise, const FVector& Location, float Volume);

	FRotator OriginalRotation;
	FVector OriginalPosition;

	UFUNCTION()
		void ResetOrientation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		EAIState GuardState;

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
		void OnStateChanged(EAIState NewState);

	UPROPERTY(EditAnywhere, Category = "AI")
		AActor* PatrolPointStart;

	UPROPERTY(EditAnywhere, Category = "AI")
		AActor* PatrolPointEnd;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool bCanPatrol = true;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool bStopPatrollingWhenSuspicios = true;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool bMoveToTargetWhenAlerted = true;

	UPROPERTY(EditAnywhere, Category = "AI")
		TSubclassOf<AFPSAIGuardController> AIClass = AFPSAIGuardController::StaticClass();

	UPROPERTY(EditAnywhere, Category = "AI")
		float PatrolPointAcceptanceRadius = 5.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		float ChasingActorAcceptanceRadius = 50.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		float MoveToNextPatrolPointDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		float ContinuePatrollingAfterSuspiciousDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		float NoPatrolResetOrientationAfterNoiseDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool bCanChasePlayerWhenSpotted = true;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool StopChassingAfterLooseTargetSightDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		bool AlertedToChassingDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		float BaseSpeed = 375.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
		float ChassingSpeed = 520.0f;

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
		void OnAIMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
	FTimerHandle TimerHandle_MoveTo;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
