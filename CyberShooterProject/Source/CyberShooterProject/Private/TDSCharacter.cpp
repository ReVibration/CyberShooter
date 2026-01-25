// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSCharacter.h"

#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
ATDSCharacter::ATDSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Fire a projectile
void ATDSCharacter::Fire()
{
	// Make sure we have a projectile to spawn
	if (!ProjectileClass) return;

	// Calculate the spawn location
	const FVector SpawnLocation = 
		GetActorLocation() + GetActorForwardVector() * 80.f;

	// Calculate the spawn rotation
	const FRotator SpawnRotation = GetActorRotation();

	// Spawn the projectile
	GetWorld()->SpawnActor<AActor>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation
	);
}

// Called to bind functionality to input
void ATDSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Call the base class
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to an enhanced input component
	UEnhancedInputComponent* EnhancedInputComponent = 
		Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// Make sure it worked
	if (!EnhancedInputComponent) return;

	if (MoveAction)
	{
		// Bind the move action
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ATDSCharacter::Move
		);
	}

	if (FireAction)
	{
		// Bind the fire action
		EnhancedInputComponent->BindAction(
			FireAction,
			ETriggerEvent::Triggered,
			this,
			&ATDSCharacter::Fire
		);
	}

}

// Called when the player wants to move
void ATDSCharacter::Move(const FInputActionValue& Value)
{
	// Get the 2D movement vector
	const FVector2D Input = Value.Get<FVector2D>();

	// Don't move if there's no input
	if (Input.IsNearlyZero()) return;

	// Add movement in that direction
	AddMovementInput(GetActorForwardVector(), Input.Y);
	AddMovementInput(GetActorRightVector(), Input.X);
}