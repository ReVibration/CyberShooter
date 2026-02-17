// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "TDSSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class UTDSSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	UTDSSpringArmComponent();

	// Minimum distance that the camera is allowed to retract to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera/Clamp", meta=(ClampMin="0.0"))
	float MinArmLength;
	
protected:

	virtual FVector BlendLocations(const FVector& DesiredArmLocation,
		const FVector& TraceHitLocation,
		bool bHitSomething,
		float DeltaTime) override;

};
