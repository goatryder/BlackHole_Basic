#include "Grabber.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
} // constructor


void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	GetComponentAndBind();
	
} // begin play


void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetReachEnd());
	}
} // tick


void UGrabber::GetComponentAndBind()
{

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);

	UE_LOG(LogTemp, Warning, TEXT("GRAB COMPONENT AND BIND CALLED"));

}

void UGrabber::Grab()
{

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, *FString::Printf(TEXT("[Grabber] GRABBED")));

	auto HitResult = GetPhysicsBodyInReach();
	auto GrabComponent = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if (ActorHit) {

		PhysicsHandle->GrabComponentAtLocation(

			GrabComponent,
			NAME_None,
			GrabComponent->GetOwner()->GetActorLocation()

		);

	}

}

void UGrabber::Release()
{

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, *FString::Printf(TEXT("[Grabber] RELEASED")));

	PhysicsHandle->ReleaseComponent();

}

FHitResult UGrabber::GetPhysicsBodyInReach()
{
	const FName TraceTag(TEXT("MyTraceTag"));

	GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	TraceParameters.TraceTag = TraceTag;

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByObjectType(

		HitResult,
		GetReachStart(),
		GetReachEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters

	);

	return HitResult;
}

FVector UGrabber::GetReachStart()
{
	FVector PlayerViewPoint;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPoint,
		PlayerViewPointRotation
	);

	return PlayerViewPoint;
}

FVector UGrabber::GetReachEnd()
{
	FVector PlayerViewPoint;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPoint,
		PlayerViewPointRotation
	);

	FVector LineTraceEnd = PlayerViewPoint + PlayerViewPointRotation.Vector() * Reach;

	return LineTraceEnd;
}


