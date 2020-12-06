// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPS_BlackHole.generated.h"


class USphereComponent;


UCLASS()
class FPSGAME_API AFPS_BlackHole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPS_BlackHole();

	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "SphereCollisions")
		USphereComponent* AttractSphere;

	UPROPERTY(EditAnywhere, Category = "SphereCollisions")
		USphereComponent* SuckSphere;

	UPROPERTY(EditAnywhere, Category = "SuckConfig")
		float SuckTime = 0.1f;

	UPROPERTY(EditAnywhere, Category = "SuckConfig")
		float AttractForceRadius = 4096.f;

	UPROPERTY(EditAnywhere, Category = "SuckConfig")
		float AttractForce = -4096.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnSuckSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	TMap<AActor*, FVector> ActorsToSuckInitScaleMap;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
