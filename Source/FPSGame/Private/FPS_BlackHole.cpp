// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_BlackHole.h"

#include "Components/SphereComponent.h"

#include "FPSProjectile.h"

// Sets default values
AFPS_BlackHole::AFPS_BlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SuckSphere = CreateDefaultSubobject<USphereComponent>("SuckSphere");
	SuckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	SuckSphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	SuckSphere->OnComponentBeginOverlap.AddDynamic(this, &AFPS_BlackHole::OnSuckSphereOverlapBegin);
	RootComponent = SuckSphere;

	AttractSphere = CreateDefaultSubobject<USphereComponent>("AttractSphere");
	AttractSphere->InitSphereRadius(AttractForceRadius);
	AttractSphere->SetupAttachment(RootComponent);
	AttractSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttractSphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AFPS_BlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPS_BlackHole::OnSuckSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!bEnableBlackHole && bCanEnableByProjectile && Cast<AFPSProjectile>(OtherActor)) {

		bEnableBlackHole = true;
	}
	else if (!ActorsToSuckInitScaleMap.Contains(OtherActor)) {
		
		ActorsToSuckInitScaleMap.Add(OtherActor, OtherActor->GetActorScale3D());
	
	}
	/*
	FVector ActorToSuckInitScale = ActorsToSuckInitScaleMap[OtherActor];

	//FVector NewScale = OtherActor->GetActorScale3D() - ActorToSuckInitScale * (GetWorld()->GetDeltaSeconds() / SuckTime);
	FVector NewScale = OtherActor->GetActorScale3D() - ActorToSuckInitScale * 0.2f;
	OtherActor->SetActorScale3D(NewScale);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *FString::Printf(TEXT("InitScale: %s, NewScale: %s"), *ActorToSuckInitScale.ToString(), *NewScale.ToString()));

	if (ActorToSuckInitScale.Size() / NewScale.Size() >= 4.f) {
		ActorsToSuckInitScaleMap.Remove(OtherActor);
		OtherActor->Destroy();
	}*/
}

// Called every frame
void AFPS_BlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bEnableBlackHole)
		return;

	TArray<UPrimitiveComponent*>OverlappedComps;

	AttractSphere->GetOverlappingComponents(OverlappedComps);

	for (int32 i = 0; i < OverlappedComps.Num(); i++) {
			
		UPrimitiveComponent* Comp = OverlappedComps[i];

		if (Comp->IsSimulatingPhysics()) {

			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *FString::Printf(TEXT("overlapped actor force: %f"), Comp->GetMass() * Comp->GetMassScale()));

			Comp->AddRadialForce(GetActorLocation(), AttractForceRadius, AttractForce, ERadialImpulseFalloff::RIF_Constant, true);
			// Comp->AddForce(GetActorLocation() - Comp->GetOwner()->GetActorLocation() * 2048.f, NAME_None, true);

		}

	}


	
	// suck actors handle
	TArray <AActor*> Keys;
	int32 KeyNum = ActorsToSuckInitScaleMap.GetKeys(Keys);
	
	for (int32 i = 0; i < KeyNum; i++) {

		AActor* ActorToSuck = Keys[i];
		FVector ActorToSuckInitScale = ActorsToSuckInitScaleMap[ActorToSuck];

		FVector NewScale = ActorToSuck->GetActorScale3D() - ActorToSuckInitScale * (DeltaTime * 2.f / SuckTime);
		ActorToSuck->SetActorScale3D(NewScale);

		if (ActorToSuckInitScale.Size() / NewScale.Size() >= 4.f) {

			ActorsToSuckInitScaleMap.Remove(ActorToSuck);
			ActorToSuck->Destroy();

		}

		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *FString::Printf(TEXT("Actor to suck name %s; init scale: %s"), *ActorToSuck->GetName(), *ActorToSuckInitScale.ToString()));
	}
	
	

}

