// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "TDSEnemyCharacter.generated.h"

// Forward declaration of the delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, AActor*, DeadEnemy);

UCLASS()
class ATDSEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDSEnemyCharacter();

	// Delegate for when the enemy dies
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyDied OnEnemyDied;

	// Function to perform melee hit logic, called from animation notify
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformMeleeHit();

	// The animation montage to play when the enemy attacks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* MeleeAttackMontage;

	// The animation montage to play when the enemy dies
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	// Function to check if the enemy is dead
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsDead() const { return bIsDead; }

	// Function to handle the enemy's death, called when health reaches zero
	UFUNCTION()
	void HandleDeath();


	// Returns the current health value
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	// Returns the maximum health value
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

protected:

	// Function to destroy the enemy actor after death animation finishes
	UFUNCTION()
	void DestroyEnemy();

	// Flag to track if the enemy is dead
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsDead = false;

	// Time to wait after death before destroying the actor
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DeathDestroyDelay = 2.0f;

	// Timer handle for managing delayed destruction after death
	FTimerHandle DeathDestroyTimerHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Health properties
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.f;

	float CurrentHealth;


	// Distance at which the AI can attack the player
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackRange = 150.f;

	// Damage dealt per attack
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackDamage = 10.f;

	// Function to handle taking damage
	UFUNCTION()
	void HandleTakeAnyDamage(
		AActor* DamagedActor,
		float Damage,
		const class UDamageType* DamageType,
		class AController* InstigatedBy,
		AActor* DamageCauser
	);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
