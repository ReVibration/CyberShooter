// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSEnemyCharacter.h"

#include "Engine/Engine.h"
#include "TDSEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ATDSEnemyCharacter::ATDSEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set AI Controller class
	AIControllerClass = ATDSEnemyAIController::StaticClass();
	// Enable automatic AI possession
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


	// Configure the controlled character to use controller rotation for yaw
	bUseControllerRotationYaw = true;

	// Ensure the controlled character does not orient rotation to movement
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Set rotation rate for smooth turning
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

}

// Called when the game starts or when spawned
void ATDSEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize health
	CurrentHealth = MaxHealth;

	// Bind the OnTakeDamage function to handle damage events
	OnTakeAnyDamage.AddDynamic(this, &ATDSEnemyCharacter::HandleTakeAnyDamage);
}

// Called every frame
void ATDSEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATDSEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATDSEnemyCharacter::HandleTakeAnyDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser
)
{
	// Reduce health by damage amount
	CurrentHealth -= Damage;

	// Debug: Print current health to the screen
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.f,
			FColor::Red,
			FString::Printf(TEXT("Enemy Health: %.0f"), CurrentHealth)
		);
	}

	// Check for death
	if (CurrentHealth <= 0.f)
	{
		// Handle enemy death (e.g., play animation, destroy actor, etc.)
		Destroy();
	}
}
