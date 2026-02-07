// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSEnemyAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"

void ATDSEnemyAIController::OnPossess(APawn* InPawn)
{
	// Call the base class OnPossess
	Super::OnPossess(InPawn);

	// Get reference to the player pawn
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Randomize initial slot angle for this AI to ensure they start in different positions around the player
	SlotAngleRad = FMath::FRandRange(0.f, 2 * PI);
	bSlotAngleInit = true;

	SetState(EEnemyState::Idle); // Start in idle state

	SlotJitterOffset = FVector2D(
		FMath::FRandRange(-slotJitter, slotJitter),
		FMath::FRandRange(-slotJitter, slotJitter)
	);

}

void ATDSEnemyAIController::Tick(float DeltaSeconds)
{
	// Call the base class Tick
	Super::Tick(DeltaSeconds);

	// If we don't have a pawn, do nothing
	if (!GetPawn()) return; 

	if (!PlayerPawn)
	{
		// Try to get the player pawn again if we don't have it
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0); 
	}

	// Check if we need to change states based on distance to player
	UpdateStateTransitions(); 
	
	// Execute behavior based on current state
	RunState(DeltaSeconds);

	// Check if we're stuck and try to unstick if necessary
	HandleStuck(DeltaSeconds); 
}

void ATDSEnemyAIController::UpdateStateTransitions()
{
	// Ensure we have valid references
	if (!PlayerPawn || !GetPawn()) return;

	// Calculate distance to player
	const float Dist = FVector::Dist2D(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());

	switch (State) 
	{
		// In Idle state, if the player comes within chase distance, switch to Chasing
		case EEnemyState::Idle:
		{
			if (Dist <= ChaseDistance)
			{
				SetState(EEnemyState::Chasing);
			}
			break;
		}
		// In Chasing state, if we get within attack range, switch to Attacking. If the player gets too far away, switch back to Idle
		case EEnemyState::Chasing:
		{
			if (Dist <= AttackRange)
			{
				SetState(EEnemyState::Attacking);
			}
			else if (Dist > ChaseDistance)
			{
				SetState(EEnemyState::Idle);
			}
			break;
		}

		// In Attacking state, if the player moves out of attack range but is still within chase distance, switch back to Chasing. If the player moves out of chase distance, switch back to Idle
		case EEnemyState::Attacking:
		{
			if (Dist > AttackRange)
			{
				SetState(EEnemyState::Chasing);
			}
			else if (Dist > ChaseDistance)
			{
				SetState(EEnemyState::Idle);
			}
			break;
		}
	}
}

void ATDSEnemyAIController::SetState(EEnemyState NewState)
{
	// If we're already in the desired state, do nothing
	if (State == NewState) return; 

	switch (State)
	{
		case EEnemyState::Attacking:
		{
			SetOrientRotationToMovement(false);
			// Clean up attacking state when leaving it
			StopAttacking();
			break;
		default:
			break;
		}
	}

	// Update to the new state
	State = NewState;

	switch (State)
	{
		case EEnemyState::Idle:
		{
			// Clear focus to allow wandering
			ClearFocus(EAIFocusPriority::Gameplay);
			SetOrientRotationToMovement(true);
			// Stop any movement when entering idle
			StopMovement();

			// Clear any existing slot timers
			GetWorldTimerManager().ClearTimer(SlotTimerHandle);

			PickNewWanderTarget(); // immediate first target
			break;
		}
		case EEnemyState::Chasing:
		{
			SetOrientRotationToMovement(false);
			// Ensure we're not attacking when we start chasing
			StopAttacking();
			// Clear wander target so we can start chasing immediately
			bHasWanderTarget = false;
			// Clear any existing wander timers
			GetWorldTimerManager().ClearTimer(WanderTimerHandle);

			// Update the target slot around the player immediately when we start chasing
			UpdateSlotTarget();

			// Set timer to update the target slot around the player at regular intervals while chasing
			GetWorldTimerManager().SetTimer(
				SlotTimerHandle,
				this,
				&ATDSEnemyAIController::UpdateSlotTarget,
				slotRecalcInterval,
				true
			);
			break;
		}
		case EEnemyState::Attacking:
		{
			// Start the attacking behavior and ensure we stop moving when we start attacking
			StopMovement();
			StartAttacking();
			break;
		}
	}

}

void ATDSEnemyAIController::RunState(float DeltaSeconds)
{
	// Ensure we have a valid pawn reference
	if (!GetPawn()) return;

	switch (State)
	{
		case EEnemyState::Idle:
		{
			// If we don't have a wander target, the timer will handle it.
			if (!bHasWanderTarget) return;

			const float DistToTarget = FVector::Dist2D(WanderTarget, GetPawn()->GetActorLocation());
			if (DistToTarget <= 120.f)
			{
				bHasWanderTarget = false; // Clear the target so we pick a new one after a delay
				StopMovement(); // Stop movement when we reach the wander target
				StartWanderAfterDelay(); // Start a new wander after a short delay to create more natural idle behavior
				return;
			}

			// Update timer for the last move
			TimeSinceLastWanderMove += DeltaSeconds;
			if (TimeSinceLastWanderMove >= WanderRepathCooldown)
			{
				// If we've been moving towards the wander target for a while, pick a new one to prevent getting stuck trying to reach an unreachable point
				MoveToLocation(WanderTarget, 80.f, true);
				TimeSinceLastWanderMove = 0.f;
			}
			break;
		}

		case EEnemyState::Chasing:
		{
			// Face player while chasing (your smooth rotation)
			RotatePawnTowardPlayer(DeltaSeconds);

			// Smooth slot targetting to help with rotation and make movement look more natural instead of robotic snapping to the slot position
			SmoothedSlotTarget = FMath::VInterpTo(SmoothedSlotTarget, CurrentSlotTarget, DeltaSeconds, SlotSmoothingSpeed);
			const float SlotDist = FVector::Dist2D(SmoothedSlotTarget, GetPawn()->GetActorLocation());

			// Only move towards the slot if we're not close enough to it, and use a cooldown to prevent excessive pathfinding calls which can cause performance issues
			if (SlotDist > StopDistance)
			{
				TimeSinceLastMove += DeltaSeconds;
				if (TimeSinceLastMove >= RepathCooldown)
				{
					MoveToLocation(SmoothedSlotTarget, StopDistance, true);
					TimeSinceLastMove = 0.f;
				}
			}
			else
			{
				StopMovement(); // Stop movement if we're close enough to the slot to prevent jittery movement
			}
			break;
		}

		case EEnemyState::Attacking:
		{
			// Attacking is handles by the timer so just keep facing the player to ensure we look in the right direction while attacking
			RotatePawnTowardPlayer(DeltaSeconds);
			// Ensure we stay still while attacking, in case we got here from chasing and were still moving. We want to be stationary while attacking.
			StopMovement();
			break;
		}
	}
}

void ATDSEnemyAIController::StartWanderAfterDelay()
{
	// Randomize the delay before picking a new wander target to create more natural idle behavior, so the AI doesn't always pause for the same amount of time before moving again
	const float Delay = FMath::FRandRange(WanderPauseMin, WanderPauseMax);

	// Set a timer to pick a new wander target after the randomized delay
	GetWorldTimerManager().SetTimer(
		WanderTimerHandle,
		this,
		&ATDSEnemyAIController::PickNewWanderTarget,
		Delay,
		false
	);
}

void ATDSEnemyAIController::PickNewWanderTarget()
{
	// Ensure we have a valid pawn reference before trying to pick a wander target
	if (!GetPawn()) return;

	// Get a random reachable point within WanderRadius of the AI's current location to use as the new wander target. This uses the navigation system to ensure the point is actually reachable, which helps prevent the AI from getting stuck trying to reach an unreachable location.
	const FVector Origin = GetPawn()->GetActorLocation();

	FNavLocation Result;
	if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		if (Nav->GetRandomReachablePointInRadius(Origin, WanderRadius, Result))
		{
			WanderTarget = Result.Location;
			bHasWanderTarget = true;
			TimeSinceLastMove = WanderRepathCooldown; // force move immediately
		}
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
	
	// Clear focus to allow movement again
	ClearFocus(EAIFocusPriority::Gameplay);
}

void ATDSEnemyAIController::DoMeleeAttack()
{
	// Ensure we have valid references
	if (!PlayerPawn || !GetPawn()) return;

	// Calculate distance to player
	const float Dist = FVector::Dist2D(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());

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
	Offset.X += SlotJitterOffset.X;
	Offset.Y += SlotJitterOffset.Y;

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
	// If we're idle or attacking, we don't consider ourselves stuck and just reset the timer and last location. Stuck detection is only relevant while we're trying to move towards a target (chasing), so we can ignore it in other states.
	if (State == EEnemyState::Idle || bIsAttacking)
	{
		StuckTime = 0.f;
		LastLocation = GetPawn()->GetActorLocation();
		return;
	}

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

void ATDSEnemyAIController::SetOrientRotationToMovement(bool bEnable)
{
	// Get the pawn
	if (APawn* P = GetPawn())
	{
		//If it's a character
		if (ACharacter* C = Cast<ACharacter>(P))
		{
			// Enable or disable orienting rotation to movement on the character's movement component, and adjust controller rotation settings accordingly
			if (UCharacterMovementComponent* Move = C->GetCharacterMovement())
			{
				Move->bOrientRotationToMovement = bEnable;
				C->bUseControllerRotationYaw = false;

				// rotation speed when orienting to movement
				Move->RotationRate = FRotator(0.f, 720.f, 0.f);
			}
		}
	}
}

