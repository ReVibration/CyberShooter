// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

void ATDSEnemyAIController::OnPossess(APawn* InPawn)
{
	// Call the base class OnPossess
	Super::OnPossess(InPawn);

	// Get reference to the player pawn
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);


}

void ATDSEnemyAIController::Tick(float DeltaSeconds)
{
	// Call the base class Tick
	Super::Tick(DeltaSeconds);

	// Ensure we have valid references
	if (!PlayerPawn || !GetPawn()) 
	{
		// Try to get the player pawn if not already set
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!PlayerPawn) return; 
	}


	// Calculate distance to player
	const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());

	if(Dist <= AttackRange)
	{
		// Start attacking if within attack range
		StartAttacking();
		return;
	}

	// Stop attacking if out of range
	StopAttacking();
	
	if (Dist <= ChaseDistance)
	{
		// Move towards the player if within chase distance
		MoveToActor(PlayerPawn, 5.0f);
	}
	else
	{
		// Stop moving if outside chase distance
		StopMovement();
	}
}

void ATDSEnemyAIController::StartAttacking()
{
	// If already attacking, do nothing
	if (bIsAttacking) return;
	// Set attacking flag
	bIsAttacking = true;

	// Stop movement to attack
	StopMovement();

	// Perform the first attack immediately
	DoMeleeAttack();

	// Set timer for subsequent attacks
	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ATDSEnemyAIController::DoMeleeAttack,
		AttackInterval,
		true
	);
}

void ATDSEnemyAIController::StopAttacking()
{
	// Clear attacking flag
	bIsAttacking = false;
	// Clear the attack timer
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

void ATDSEnemyAIController::DoMeleeAttack()
{
	// Ensure we have valid references
	if (!PlayerPawn || !GetPawn()) return;

	// Calculate distance to player
	const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());

	// If within attack range, apply damage
	if (Dist <= AttackRange)
	{
		// Apply damage to the player
		UGameplayStatics::ApplyDamage(
			PlayerPawn,
			AttackDamage,
			this,
			GetPawn(),
			UDamageType::StaticClass()
		);
	}
	else
	{
		// If out of range, stop attacking
		StopAttacking();
		return;
	}
}

