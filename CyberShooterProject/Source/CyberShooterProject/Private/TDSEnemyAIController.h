// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TDSEnemyAIController.generated.h"


UCLASS()
class ATDSEnemyAIController : public AAIController
{
	GENERATED_BODY()


public:
	// Sets default values for this controller's properties
	virtual void OnPossess(APawn* InPawn) override;
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:

	// Variables for stuck detection
	FVector LastLocation = FVector::ZeroVector;
	float StuckTime = 0.f;

	// Parameters for stuck detection
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Stuck")
	float StuckSpeedThreshold = 5.f;   // units/sec (very low)

	// Time the AI must be below the speed threshold before we consider it stuck
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Stuck")
	float StuckTimeToTrigger = 0.6f;   // seconds


	UPROPERTY(EditDefaultsOnly, Category = "Rotation")
	float ChaseTurnSpeed = 8.f; // Speed at which the AI will rotate to face the player when within chase distance

	// Radius around the player where the AI will try to position itself for combat
	UPROPERTY(EditDefaultsOnly, Category="Combat|Slots")
	float SlotRadius = 230.f;

	// Slight random offset added to the AI's position when trying to occupy a combat slot, to prevent them from clustering too tightly
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Slots")
	float slotJitter = 8.f;

	// Time interval for recalculating combat slots to allow for dynamic repositioning during combat
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Slots")
	float slotRecalcInterval = 0.8f;

	// Speed at which the AI will move towards its target combat slot position, used for smoothing movement around the player
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Slots")
	float SlotSmoothingSpeed = 6.f;

	FVector SmoothedSlotTarget = FVector::ZeroVector;

	float SlotAngleRad = 0.f; // Current angle around the player for the AI to position itself in combat
	bool bSlotAngleInit = false; // Whether the initial slot angle has been set, used to ensure the AI starts in a unique position around the player

	// Timer handle for managing combat slot recalculations
	FTimerHandle SlotTimerHandle;

	// Current target position for the AI to move towards when trying to occupy a combat slot around the player
	FVector CurrentSlotTarget = FVector::ZeroVector;

	// Distance at which the AI can attack the player
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackRange = 220.f;

	// Damage dealt per attack
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackDamage = 10.f;

	// Time interval between attacks
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackInterval = 0.8f;

	// Distance at which the AI will start chasing the player
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ChaseDistance = 2000.0f;

	// Distance at which the AI will stop moving towards the player
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float StopDistance = 80.0f;

private:

	// Function to handle stuck detection and recovery
	void HandleStuck(float DeltaSeconds);
	void Unstick();

	// Function to smoothly rotate the AI's pawn to face the player when within chase distance
	void RotatePawnTowardPlayer(float DeltaSeconds);

	// Function to calculate and update the AI's target position for occupying a combat slot around the player
	void UpdateSlotTarget();

	// Function to compute the ideal position for the AI to occupy around the player based on the current number of enemies and their positions
	FVector ComputeSlotTarget() const;

	// Timer handle for managing attack intervals
	FTimerHandle AttackTimerHandle;

	// Functions to manage attacking behavior
	void StartAttacking();
	void StopAttacking();
	void DoMeleeAttack();

	// Whether the AI is currently attacking
	bool bIsAttacking = false;

	// Reference to the player pawn
	UPROPERTY()
	APawn* PlayerPawn = nullptr;

	// Cooldown time for pathfinding updates to prevent excessive calls
	float RepathCooldown = 0.2f;
	float TimeSinceLastMove = 0.f;

};


