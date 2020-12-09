// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FPSAIGuardController.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSAIGuardController : public AAIController
{
	GENERATED_BODY()
	
public:
	AFPSAIGuardController();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
