// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UDecalComponent;
class UAudioComponent;
class UArrowComponent;
class UParticleSystemComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	AFPSLaunchPad();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
		UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
		UArrowComponent* LaunchArrow;

	UPROPERTY(EditAnywhere, Category = "Components")
		UDecalComponent* Decal_Launched;

	UPROPERTY(EditAnywhere, Category = "Components")
		UDecalComponent* Decal_Idle;

	UPROPERTY(EditAnywhere, Category = "Components")
		UAudioComponent* LaunchSound;

	UPROPERTY(EditAnywhere, Category = "Components")
		UParticleSystemComponent* LaunchFX;

	UPROPERTY(EditAnywhere, Category = "LaunchParams")
		float LaunchCooldown=1.f;

	float LaunchCooldownAccumulation;
	bool bIsOnCooldown;

	UPROPERTY(EditAnywhere, Category = "LaunchParams")
		FVector LaunchVelocity = FVector(1000.0f, 0.0f, 1000.f);

	void SwapDecals(bool Cooldown);

	UFUNCTION()
		void BoxCompBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

};
