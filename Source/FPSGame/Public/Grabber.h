#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

class UPhysicsHandleComponent;
class UInputComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAME_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGrabber();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
		float Reach = 100.f;

protected:
	virtual void BeginPlay() override;

	UPhysicsHandleComponent* PhysicsHandle;

	UInputComponent* InputComponent;

	void GetComponentAndBind();

	void Grab();
	void Release();

	FHitResult GetPhysicsBodyInReach();

	FVector GetReachStart();
	FVector GetReachEnd();

		
};
