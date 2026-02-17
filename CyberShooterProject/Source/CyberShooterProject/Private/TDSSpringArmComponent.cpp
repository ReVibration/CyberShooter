// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSSpringArmComponent.h"

UTDSSpringArmComponent::UTDSSpringArmComponent()
{
	// Starting point for the spring arm
	MinArmLength = 700.f;
}

FVector UTDSSpringArmComponent::BlendLocations(
	const FVector& DesiredArmLocation,
	const FVector& TraceHitLocation,
	bool bHitSomething,
	float DeltaTime)
{
	// Let the base class decide the normal result (handles collision smoothing etc.)
	FVector Result = Super::BlendLocations(DesiredArmLocation, TraceHitLocation, bHitSomething, DeltaTime);

	// Clamp how close the camera can get to the spring arm origin
	if (MinArmLength > 0.f)
	{
		const FVector ArmOrigin = GetComponentLocation();
		FVector ToCamera = Result - ArmOrigin;
		const float CurrentLen = ToCamera.Length();

		// If it is too close then change the camera to be clamped
		if (CurrentLen < MinArmLength)
		{
			const FVector Dir = ToCamera.IsNearlyZero() ? -GetForwardVector() : (ToCamera / CurrentLen);
			Result = ArmOrigin + Dir * MinArmLength;
		}
	}

	return Result;
}