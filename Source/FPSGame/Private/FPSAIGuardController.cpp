// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuardController.h"


AFPSAIGuardController::AFPSAIGuardController() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFPSAIGuardController::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

void AFPSAIGuardController::BeginPlay()
{

	Super::BeginPlay();

}

void AFPSAIGuardController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, *FString::Printf(TEXT("Move Completed AFPSAIGuardController Overrided")));
}
