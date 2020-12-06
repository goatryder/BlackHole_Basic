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
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();

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

			Destroy();
			bShouldDestroy = false;

		}
		else {

			SetActorScale3D(InitScale * (PickupTimeAccomulation / PickupDestroyTime));

		}
	}

}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{

	AActor::NotifyActorBeginOverlap(OtherActor);


	AFPSCharacter* MyCharacter = Cast<AFPSCharacter>(OtherActor);
	
	if (MyCharacter) {

		MyCharacter->bIsCarryingObjective = true;

		PlayEffects();
		bShouldDestroy = true;

	}

}

