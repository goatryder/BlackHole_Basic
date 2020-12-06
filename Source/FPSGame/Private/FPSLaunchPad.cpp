// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/AudioComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FPSCharacter.h"

AFPSLaunchPad::AFPSLaunchPad()
{
	PrimaryActorTick.bCanEverTick = true;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("LaunchPadTriggerBox"));
	RootComponent = BoxComp;
	BoxComp->InitBoxExtent(FVector(128.f, 128.f, 32.f));
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::BoxCompBeginOverlap);
	
	// mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaunchPadMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (FoundMesh.Succeeded())
		StaticMesh->SetStaticMesh(FoundMesh.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Challenges/LaunchPad/M_LaunchPadBase.M_LaunchPadBase"));
	if (FoundMaterial.Succeeded())
		StaticMesh->SetMaterial(0, FoundMaterial.Object);

	StaticMesh->SetRelativeTransform(FTransform(
		FRotator(0.f, 90.f, 0.f), FVector(0.f, 0.f, -16.f), FVector(2.f, 2.f, 0.2f)
	));
	// end mesh 

	LaunchArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchArrow"));
	LaunchArrow->SetupAttachment(RootComponent);
	LaunchArrow->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	// sound
	LaunchSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LaunchSound"));
	LaunchSound->SetupAttachment(RootComponent);
	LaunchSound->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> FoundSound(TEXT("/Engine/EditorSounds/Notifications/CompileSuccess.CompileSuccess"));
	if (FoundSound.Succeeded())
		LaunchSound->SetSound(FoundSound.Object);
	// end sound
	
	// decals
	Decal_Idle = CreateDefaultSubobject<UDecalComponent>(TEXT("IdleDecal"));
	Decal_Idle->SetupAttachment(RootComponent);	
	Decal_Idle->DecalSize = FVector(12.0f, 99.f, 99.f);
	Decal_Idle->SetRelativeRotation(FRotator(90.f, 0.f, 90.f));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial2(TEXT("/Game/Challenges/LaunchPad/M_LaunchPAd2.M_LaunchPAd2"));
	if (FoundMaterial2.Succeeded())
		Decal_Idle->SetMaterial(0, FoundMaterial2.Object);

	Decal_Launched = CreateDefaultSubobject<UDecalComponent>(TEXT("LaunchDecal"));
	Decal_Launched->SetupAttachment(RootComponent);
	Decal_Launched->SetVisibility(false);
	Decal_Launched->SetupAttachment(RootComponent);
	Decal_Launched->DecalSize = FVector(12.0f, 99.f, 99.f);
	Decal_Launched->SetRelativeRotation(FRotator(90.f, 0.f, 90.f));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial3(TEXT("/Game/Challenges/LaunchPad/M_LaunchPAd_IDLE.M_LaunchPAd_IDLE"));
	if (FoundMaterial3.Succeeded())
		Decal_Launched->SetMaterial(0, FoundMaterial3.Object);
	// end decals

	//LaunchVelocity
	UE_LOG(LogTemp, Warning, TEXT("Magnitude before: %f"), LaunchVelocity.Size());
	LaunchVelocity = FVector::DotProduct(FVector::ForwardVector, GetActorRightVector()) * LaunchVelocity;
	UE_LOG(LogTemp, Warning, TEXT("Magnitude after: %f"), LaunchVelocity.Size());

}

void AFPSLaunchPad::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSLaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFPSLaunchPad::BoxCompBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *FString::Printf(TEXT("overlap launchpad box")));

	AFPSCharacter* Character = Cast<AFPSCharacter>(OtherActor);

	if (Character) {

		Character->LaunchCharacter(LaunchVelocity, false, false);

		UKismetSystemLibrary::DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LaunchVelocity, FLinearColor::Green, 2222222222.0f);

	} 
	else if (OtherComp->IsSimulatingPhysics()) {

		OtherComp->AddForce(LaunchVelocity);

	}

}
