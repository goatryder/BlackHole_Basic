// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/AudioComponent.h"
#include "Components/ArrowComponent.h"
#include "Particles/ParticleSystemComponent.h"
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

	// particle
	LaunchFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaunchFX"));
	LaunchFX->SetupAttachment(RootComponent);
	LaunchFX->bAutoActivate = false;
	// end particle

}

void AFPSLaunchPad::BeginPlay()
{
	Super::BeginPlay();

	/*
	FVector Start = GetActorLocation();

	FVector ArrowFinish = LaunchArrow->GetForwardVector() * 300.0f + Start;
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, ArrowFinish, FLinearColor::Yellow, 22222222222221.0f);

	FVector NormalFinish = FVector::ForwardVector * 300.0f + Start;
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, NormalFinish, FLinearColor::White, 22222222222221.0f);

	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, Start + LaunchVelocity, FLinearColor::Green, 22222222222221.0f);

	float DotProduct = FVector::DotProduct(LaunchArrow->GetForwardVector(), FVector::ForwardVector);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, Start + LaunchVelocity.RotateAngleAxis(Angle, FVector::UpVector), FLinearColor::Red, 22222222222221.0f);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("angle  %f"), Angle));
	*/

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *FString::Printf(TEXT("Magnitude before: %f, %s"), LaunchVelocity.Size(), *LaunchVelocity.ToString()));

	
	float DotProduct = FVector::DotProduct(LaunchArrow->GetForwardVector(), FVector::ForwardVector);
	FVector CrossProduct =  FVector::CrossProduct(LaunchArrow->GetForwardVector(), FVector::ForwardVector);

	float AngleDir = FMath::Sign(FVector::DotProduct(CrossProduct, FVector::DownVector));
	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

	LaunchVelocity = LaunchVelocity.RotateAngleAxis(Angle * AngleDir, FVector::UpVector);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *FString::Printf(TEXT("Magnitude after: %f, %s"), LaunchVelocity.Size(), *LaunchVelocity.ToString()));

}

void AFPSLaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// handle cooldown

	if (bIsOnCooldown) {

		LaunchCooldownAccumulation += DeltaTime;

		if (LaunchCooldownAccumulation > LaunchCooldown) {
			bIsOnCooldown = false;
			LaunchCooldownAccumulation = 0.0f;
			SwapDecals(false);
		}

	}

	// end handle cooldown

	/*
	FVector ZOffset = FVector(0.0f, 0.0f, 50.0f);
	FVector Start = GetActorLocation() + ZOffset;
	
	FVector ArrowFinish = LaunchArrow->GetForwardVector() * 300.0f + Start;
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, ArrowFinish, FLinearColor::Yellow, 1);
	
	FVector NormalFinish = FVector::ForwardVector * 300.0f + Start;
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, NormalFinish, FLinearColor::White, 1);

	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, Start + LaunchVelocity, FLinearColor::Green, 1);

	float DotProduct = FVector::DotProduct(LaunchArrow->GetForwardVector(), FVector::ForwardVector);
	FVector CrossProduct = FVector::CrossProduct(LaunchArrow->GetForwardVector(), FVector::ForwardVector);

	float AngleDir = FMath::Sign(FVector::DotProduct(CrossProduct, FVector::DownVector));

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct)) * AngleDir;
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, Start + LaunchVelocity.RotateAngleAxis(Angle, FVector::UpVector), FLinearColor::Red, 1);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("angle  %f"), Angle));
	*/
}

void AFPSLaunchPad::SwapDecals(bool Cooldown)
{

	Decal_Launched->SetVisibility(Cooldown);
	Decal_Idle->SetVisibility(!Cooldown);

}

void AFPSLaunchPad::BoxCompBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *FString::Printf(TEXT("overlap launchpad box")));

	AFPSCharacter* Character = Cast<AFPSCharacter>(OtherActor);

	if (bIsOnCooldown)
		return;

	if (Character) {

		Character->LaunchCharacter(LaunchVelocity, true, true);

	} 
	else if (OtherComp->IsSimulatingPhysics()) {

		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);

	}
	else {

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *FString::Printf(TEXT("overlapped obj cannot be launched")));
		return;
	}

	LaunchSound->Activate(true);
	LaunchFX->Activate(true);

	bIsOnCooldown = true;
	SwapDecals(true);

	/*FTimerHandle UnusedTimeHandle;
	GetWorldTimerManager().SetTimer(UnusedTimeHandle, this, &AFPSLaunchPad::SwapDecals, 1.0f);*/
}
