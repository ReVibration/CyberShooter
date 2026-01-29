// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDSCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every game tick to update
	virtual void Tick(float DeltaSeconds) override;

	// Called when the game starts to setup inputs
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	// --- Camera ---
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	float RotationSpeed = 2.f;

	// --- Enhanced Input ---
	// This is the mapping context for the controller
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext;

	// Mapping for movement
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;
	
	// Mapping for fire
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

	// --- Combat ---
	// This allows us to change the projectile when we want to
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	USceneComponent* Muzzle;

	// A property that can change the fire rate 
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float FireRate = 8.f;

	float NextFireTime = 0.f;

	// Called when the player want's to fire a projectile
	void FirePressed(const FInputActionValue& Value);

	// Called when the player want's to move
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	void FaceMouseCursor();

};
