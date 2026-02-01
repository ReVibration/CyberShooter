// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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
	if (!PlayerPawn || !GetPawn()) { return; }

	// Calculate distance to player
	const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());

	// Decide whether to chase or stop based on distance
	if (Dist <= StopDistance)
	{
		// Stop moving if within stop distance
		StopMovement();
	}
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

	

