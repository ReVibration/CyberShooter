// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "NavigationSystem.h"

void ATDSEnemyAIController::OnPossess(APawn* InPawn)
{
	// Call the base class OnPossess
	Super::OnPossess(InPawn);

	// Get reference to the player pawn
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Start the timer to update the combat slot target position around the player
	GetWorldTimerManager().SetTimer(
		SlotTimerHandle,
		this,
		&ATDSEnemyAIController::UpdateSlotTarget,
		slotRecalcInterval,
		true
	);

	UpdateSlotTarget(); // Initial call to set the slot target immediately

	SlotAngleRad = FMath::FRandRange(0.f, 2 * PI); // Start at a random angle to spread out multiple enemies
	bSlotAngleInit = true; // Mark that we've initialized the slot angle
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
	

	// Smoothly interpolate towards the slot target for more natural movement
	SmoothedSlotTarget = FMath::VInterpTo(SmoothedSlotTarget, CurrentSlotTarget, DeltaSeconds, SlotSmoothingSpeed);
	// Calculate distance to current slot target smoothed
	const float SlotDist = FVector::Dist(SmoothedSlotTarget, GetPawn()->GetActorLocation());


	if (Dist <= ChaseDistance)
	{
		RotatePawnTowardPlayer(DeltaSeconds); // Focus on the player when within chase distance
		if(Dist <= AttackRange)
		{
			SetFocus(PlayerPawn); // Set focus to the player when within attack range
			StartAttacking(); // Start attacking if within attack range
			return; // No need to move if we're attacking
		}

		StopAttacking(); // Stop attacking if we're not within attack range

		if (SlotDist > StopDistance)
		{
			// Move towards the current slot target if we're not close enough and cooldown has passed
			TimeSinceLastMove += DeltaSeconds;
			if (TimeSinceLastMove >= RepathCooldown)
			{


				MoveToLocation(SmoothedSlotTarget, StopDistance, true);
				TimeSinceLastMove = 0.f; // Reset cooldown timer
			}
		}
		else
		{
			StopMovement(); // Stop moving if we're close enough to the slot target
		}
	}
	else
	{
		StopAttacking(); // Stop attacking if we're out of chase distance
		StopMovement(); // Stop moving if we're out of chase distance
		ClearFocus(EAIFocusPriority::Gameplay); // Clear focus if we're out of chase distance
	}

	// Handle stuck detection and recovery
	HandleStuck(DeltaSeconds);

}

void ATDSEnemyAIController::StartAttacking()
{
	// If already attacking, do nothing
	if (bIsAttacking) return;
	// Set attacking flag
	bIsAttacking = true;

	// When we start attacking, we want to ensure the AI is facing the player
	SetFocus(PlayerPawn);

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
	
	// Clear focus to allow movement again
	ClearFocus(EAIFocusPriority::Gameplay);
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

void ATDSEnemyAIController::UpdateSlotTarget()
{
	// Ensure we have valid references
	if (!PlayerPawn || !GetPawn()) return;

	// Recalculate the target position for the AI to occupy around the player
	CurrentSlotTarget = ComputeSlotTarget();

	if(SmoothedSlotTarget.IsZero())
	{
		SmoothedSlotTarget = CurrentSlotTarget; // Initialize smoothed target to current target if it's the first update
	}
}

FVector ATDSEnemyAIController::ComputeSlotTarget() const
{
	// Get the player's location
	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	
	const float Angle = SlotAngleRad; // Use the current angle for this AI to determine its position around the player

	// Calculate the offset from the player based on the angle and desired radius
	FVector Offset(
		FMath::Cos(Angle) * SlotRadius,
		FMath::Sin(Angle) * SlotRadius,
		0.f
	);

	// Add a little noise so it doesn't look robotic
	Offset.X += FMath::FRandRange(-slotJitter, slotJitter);
	Offset.Y += FMath::FRandRange(-slotJitter, slotJitter);

	// Calculate the desired position by adding the offset to the player's location
	FVector Desired = PlayerLoc + Offset;

	// Check for nearby pawns to avoid overlapping with them
	TArray<AActor*> Nearby;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Desired,
		140.f, // Radius for overlap
		{ UEngineTypes::ConvertToObjectType(ECC_Pawn) },
		APawn::StaticClass(),
		{ GetPawn() },
		Nearby
	);

	// Calculate a push vector to move away from nearby pawns and prevent clustering
	FVector Push(0.f, 0.f, 0.f);
	for (AActor* A: Nearby)
	{
		if (!A || A == GetPawn()) continue; // Ignore invalid actors and the player

		// Calculate a push vector away from nearby pawns to prevent clustering
		FVector ToActor = Desired - A->GetActorLocation();
		ToActor.Z = 0.f; // Only push in the horizontal plane

		const float Len = ToActor.Size();
		if (Len > 1.f) 
		{
			Push += (ToActor / Len) * (140.f - Len); // Push stronger the closer we are to the other pawn, with a max push at 140 units
		}
	}

	Desired += Push; // Apply the push to the desired position

	// Project to navmesh so it's always reachable
	FNavLocation Projected;
	// Use a larger extent for projection to allow for more flexible positioning around the player
	if (const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		// The FVector(200.f, 200.f, 200.f) extent allows for a wider search area around the desired point, which can help find a valid navigation point even if the exact desired location is not on the navmesh.
		if (NavSys->ProjectPointToNavigation(Desired, Projected, FVector(200.f, 200.f, 200.f)))
		{
			return Projected.Location;
		}
	}

	return Desired;
}

void ATDSEnemyAIController::RotatePawnTowardPlayer(float DeltaSeconds)
{
	// Ensure we have valid references
	if (!PlayerPawn || !GetPawn()) return;

	// Get the pawn location and calculate the direction vector to the player
	const FVector PawnLoc = GetPawn()->GetActorLocation();
	FVector ToPlayer = PlayerPawn->GetActorLocation() - PawnLoc;
	ToPlayer.Z = 0.f;

	// If the player is exactly above or below the AI, we can't calculate a rotation, so just return
	if (ToPlayer.IsNearlyZero()) return;

	// Calculate the target rotation to face the player and smoothly interpolate towards it
	const FRotator TargetRot = ToPlayer.Rotation();
	const FRotator NewRot = FMath::RInterpTo(
		GetPawn()->GetActorRotation(),
		TargetRot,
		DeltaSeconds,
		ChaseTurnSpeed
	);

	GetPawn()->SetActorRotation(NewRot);
}

void ATDSEnemyAIController::HandleStuck(float DeltaSeconds)
{
	// Ensure we have a valid pawn reference
	APawn* P = GetPawn();
	if (!P) return;

	// Get the current location of the pawn
	const FVector Current = P->GetActorLocation();

	// If this is the first tick or we have no valid last location, initialize it and return
	if (LastLocation.IsZero())
	{
		LastLocation = Current;
		return;
	}

	// Calculate the speed of the pawn based on the distance traveled since the last tick
	const float Speed = FVector::Dist(Current, LastLocation) / FMath::Max(DeltaSeconds, 0.001f);
	LastLocation = Current;

	// Only consider "stuck" while we are meant to be moving (chasing, not attacking)
	if (bIsAttacking) { StuckTime = 0.f; return; }

	// If the speed is below the threshold, increment the stuck timer. If it exceeds the time to trigger, attempt to unstick.
	if (Speed < StuckSpeedThreshold)
	{
		StuckTime += DeltaSeconds;
		// If we've been below the speed threshold for long enough, we consider ourselves stuck and attempt to unstick
		if (StuckTime >= StuckTimeToTrigger)
		{
			Unstick();
			StuckTime = 0.f;
		}
	}
	else
	{
		// If we're moving above the speed threshold, reset the stuck timer
		StuckTime = 0.f;
	}
}

void ATDSEnemyAIController::Unstick()
{
	// Ensure we have valid references
	if (!PlayerPawn || !GetPawn()) return;

	// Random small sidestep away from the crowd
	const FVector Right = GetPawn()->GetActorRightVector();
	const float Sign = FMath::RandBool() ? 1.f : -1.f;

	FVector Nudge = Right * Sign * FMath::RandRange(180.f, 220.f);
	Nudge.Z = 0.f;

	CurrentSlotTarget += Nudge;   // shift target a bit
	SmoothedSlotTarget = CurrentSlotTarget;

	StopMovement();
	TimeSinceLastMove = RepathCooldown; // force a repath immediately next tick
}
