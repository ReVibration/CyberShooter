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
	float StopDistance = 170.0f;

private:

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


