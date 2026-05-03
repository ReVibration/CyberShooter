// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSCharacter.h"

#include "InputCoreTypes.h"

#include "TDSGameMode.h"
#include "TDSPlayerController.h"
#include "TDSGameInstance.h"
#include "Camera/CameraComponent.h"
#include "TDSSpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TDSUpgradeComponent.h"
#include "TDSProjectile.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "TDSHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundBase.h"

// Sets default values
ATDSCharacter::ATDSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Top-Down Feel: no pitch/roll, rotate manually
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Create the minimum safety distance sphere
	CameraMinDistanceSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CameraMinDistanceSphere"));
	CameraMinDistanceSphere->SetupAttachment(GetRootComponent());

	// Size = how close the camera is allowed to get.
	// Start around 80–120 and tune.
	CameraMinDistanceSphere->SetSphereRadius(100.f);

	// Only used for camera probing
	CameraMinDistanceSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CameraMinDistanceSphere->SetCollisionObjectType(ECC_WorldDynamic);

	// Ignore everything except the camera probe
	CameraMinDistanceSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CameraMinDistanceSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

	// Create the camera boom (spring arm)
	CameraBoom = CreateDefaultSubobject<UTDSSpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 900.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));

	// Allow to colide with walls
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeChannel = ECC_Camera;
	CameraBoom->ProbeSize = 16.f;

	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// Create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	// ===== Prevent mesh from blocking camera =====
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Get the player mesh (for use in blueprints)
	PlayerMesh = GetMesh();


	// Create the weapon mesh
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(false);

	// Create the upgrade component
	UpgradeComponent = CreateDefaultSubobject<UTDSUpgradeComponent>(TEXT("UpgradeComponent"));
}

// Called when the game starts or when spawned
void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentMaxHealth = BaseMaxHealth;
	CurrentMoveSpeed = BaseMoveSpeed;
	CurrentFireInterval = BaseFireInterval;
	CurrentProjectileDamage = BaseProjectileDamage;
	CurrentProjectileSpeed = BaseProjectileSpeed;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = CurrentMoveSpeed;
	}

	if (UpgradeComponent)
	{
		UpgradeComponent->RefreshFromRunData();
	}

	// Initialize health
	RestoreHealthFromGameInstance();

	// Bind to take damage event
	OnTakeAnyDamage.AddDynamic(this, &ATDSCharacter::HandleTakeAnyDamage);

	// Add Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the local player subsystem
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			// Get the Enhanced Input Local Player Subsystem
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Ensure we have a valid mapping context
				if (DefaultMappingContext)
					// Add the mapping context
					Subsystem->AddMappingContext(DefaultMappingContext, 0);

			}
		}
	}

	// Store the weapon's default relative location for use in recoil
	if (WeaponMesh)
	{
		WeaponDefaultRelativeLocation = WeaponMesh->GetRelativeLocation();
	}

}

void ATDSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// Only rotate to face the mouse cursor if we're alive
	if (!bIsDead) {
		FaceMouseCursor();
	}
		

	// Handle recoil return
	if (WeaponMesh)
	{
		// Smoothly interpolate the current weapon offset back to zero over time
		WeaponCurrentOffset = FMath::VInterpTo(
			WeaponCurrentOffset,
			FVector::ZeroVector,
			DeltaSeconds,
			RecoilReturnSpeed
		);

		// Update the weapon mesh's relative location based on the default location plus the current recoil offset
		WeaponMesh->SetRelativeLocation(WeaponDefaultRelativeLocation + WeaponCurrentOffset);
	}

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

	// Bind firing actions
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATDSCharacter::StartFiring);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATDSCharacter::StopFiring);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ATDSCharacter::StopFiring);

	// Bind pause action
	if (PauseAction)
	{
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ATDSCharacter::HandlePausePressed);
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
	AddMovementInput(FVector::ForwardVector, Input.Y);
	AddMovementInput(FVector::RightVector, Input.X);

}

// Handles starting the firing of projectiles
void ATDSCharacter::StartFiring()
{
	if (bIsFiring && !GetWorldTimerManager().IsTimerActive(FireTimerHandle))
	{
		bIsFiring = false;
	}

	if (bIsFiring)
	{
		return;
	}

	bIsFiring = true;

	const float FireInterval = FMath::Max(CurrentFireInterval, 0.01f);

	// Fire immediately
	FireOnce();

	// Then start looping
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ATDSCharacter::FireOnce,
		FireInterval,
		true
	);
}

void ATDSCharacter::StopFiring()
{
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}
void ATDSCharacter::FireOnce()
{
	// Make sure we have a projectile class to spawn
	if (!ProjectileClass) return;
	if (!WeaponMesh) return;

	// Default fallback
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 80.f + FVector(0.f, 0.f, 20.f);

	// Use muzzle socket location if it exists
	if (WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		SpawnLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
	}

	// Keep using the character's facing direction for projectile movement
	const FRotator SpawnRotation = GetActorRotation();

	// Add a small offset to the spawn location to prevent immediate collision with the player
	SpawnLocation += GetActorForwardVector() * 20.f;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATDSProjectile* Projectile = GetWorld()->SpawnActor<ATDSProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		Params
	);

	if (Projectile)
	{
		Projectile->InitialiseProjectile(CurrentProjectileDamage, CurrentProjectileSpeed);
	}

	// Spawn muzzle flash effect if we have one and the socket exists
	if (MuzzleFlashEffect && WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		const FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		const FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(TEXT("Muzzle"));

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			MuzzleFlashEffect,
			MuzzleLocation,
			MuzzleRotation
		);
	}

	// Apply recoil by setting the current weapon offset to the recoil distance.
	WeaponCurrentOffset.X = -RecoilDistance;

	// Add the camera shake if we have a class for it
	if (FireCameraShakeClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->ClientStartCameraShake(FireCameraShakeClass);
		}
	}

	// Play fire sound if we have one
	if (FireSound)
	{
		// Randomize the pitch slightly for variety
		const float RandomPitch = FireSoundPitch + FMath::RandRange(
			-FireSoundPitchVariation,
			FireSoundPitchVariation
		);


		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			GetActorLocation(),
			FireSoundVolume,
			FireSoundPitch
		);
	}

}


void ATDSCharacter::FaceMouseCursor()
{
	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetController());
	// Ensure it is valid
	if (!PC) return;


	FVector AimPoint;
	if (GetMouseAimPointOnPlayerPlane(*PC, AimPoint))
	{
		FVector ToAim = AimPoint - GetActorLocation();
		ToAim.Z = 0.f;

		if (!ToAim.IsNearlyZero())
		{
			const float Yaw = ToAim.Rotation().Yaw;
			SetActorRotation(FRotator(0.f, Yaw, 0.f));
		}
	}
}

void ATDSCharacter::HandleTakeAnyDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	// Ignore non-positive damage
	if (Damage <= 0.f) return;

	// If we're already dead, ignore further damage
	if (bIsDead) return;

	// Play damage sound if we have one
	if (DamageSound)
	{

		UGameplayStatics::PlaySoundAtLocation(
			this,
			DamageSound,
			GetActorLocation(),
			DamageSoundVolume
		);
	}

	// Show damage flash on the player's HUD if we have a player controller
	if (ATDSPlayerController* PC = Cast<ATDSPlayerController>(GetController()))
	{
		PC->ShowDamageFlash();
	}

	// Add camera shake if we have a class for it 
	if (DamageCameraShakeClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->StartCameraShake(DamageCameraShakeClass);
			}
		}
	}

	// Decrease health and clamp to valid range
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, CurrentMaxHealth);

	// Save updated health for the run
	SaveHealthToGameInstance();

	// Check for death
	if(CurrentHealth <= 0.f)
	{
		// Handle death (play animation, disable input, trigger game over after delay)
		HandleDeath();
	}
}

void ATDSCharacter::Heal(float Amount)
{
	if (Amount <= 0.f) return; // Ignore non-positive healing
	// Increase health and clamp to valid range
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, CurrentMaxHealth);

	// Save updated health for the run
	SaveHealthToGameInstance();
}

bool ATDSCharacter::GetMouseAimPointOnPlayerPlane(APlayerController& PC, FVector& OutAimPoint) const
{
	// Get the ray direction and orgin
	FVector RayOrigin, RayDir;
	if (!PC.DeprojectMousePositionToWorld(RayOrigin, RayDir))
	{
		return false;
	}

	// Plane at the player's current Z
	const FVector PlanePoint = GetActorLocation();
	const FVector PlaneNormal = FVector::UpVector;

	// Denomise the product and if it's nearly zero do nothing
	const float Denom = FVector::DotProduct(RayDir, PlaneNormal);
	if (FMath::IsNearlyZero(Denom))
	{
		return false;
	}

	const float T = FVector::DotProduct((PlanePoint - RayOrigin), PlaneNormal) / Denom;
	if (T < 0.f)
	{
		return false;
	}

	OutAimPoint = RayOrigin + RayDir * T;
	return true;
}

void ATDSCharacter::HandleDeath()
{
	// If already dead, do nothing (prevents multiple triggers if taking damage after death)
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	// Stop firing immediately
	StopFiring();

	// Stop movement
	GetCharacterMovement()->DisableMovement();

	// Optional: disable collision so enemies stop interacting physically
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Prevent further input
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	float DeathDelay = 1.0f;

	// Play death sound if available
	if (DeathSound)
		{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DeathSound,
			GetActorLocation(),
			DeathSoundVolume
		);
	}

	// Play death animation if available
	if (GetMesh())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (DeathMontage)
			{
				const float MontageLength = AnimInstance->Montage_Play(DeathMontage);
				if (MontageLength > 0.f)
				{
					DeathDelay = MontageLength - 0.5;
				}
			}
		}
	}

	// Delay game over until animation finishes
	FTimerHandle DeathTimerHandle;
	GetWorldTimerManager().SetTimer(
		DeathTimerHandle,
		[this]()
		{
			if (ATDSGameMode* GM = Cast<ATDSGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GM->HandleGameOver(GetController());
			}
		},
		DeathDelay,
		false
	);

}

// Handles the pause input action
void ATDSCharacter::HandlePausePressed()
{
	if (ATDSPlayerController* PC = Cast<ATDSPlayerController>(GetController()))
	{
		PC->TogglePauseMenu();
	}
}

// Restores health from the game instance if available, otherwise initializes it to max health and stores it in the game instance
void ATDSCharacter::RestoreHealthFromGameInstance()
{
	if (UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance()))
	{
		if (GI->HasRunPlayerHealth())
		{
			CurrentHealth = FMath::Clamp(GI->GetRunCurrentHealth(), 0.f, CurrentMaxHealth);
		}
		else
		{
			CurrentHealth = CurrentMaxHealth;
			GI->InitialiseRunPlayerHealth(CurrentMaxHealth);
		}
	}
	else
	{
		CurrentHealth = CurrentMaxHealth;
	}

	bRunHealthInitialised = true;
	SaveHealthToGameInstance();
}

// Saves the current health values to the game instance for persistence across levels
void ATDSCharacter::SaveHealthToGameInstance()
{
	if (UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance()))
	{
		GI->SaveRunHealth(CurrentHealth, CurrentMaxHealth);
	}
}

// Applies recalculated stats to the character, ensuring they are within valid ranges and updating related properties like movement speed and health accordingly.
void ATDSCharacter::ApplyRecalculatedStats(
	float NewMaxHealth,
	float NewMoveSpeed,
	float NewFireInterval,
	float NewProjectileDamage,
	float NewProjectileSpeed)
{
	// Store the old max health before applying the new one, so we can adjust current health accordingly.
	const float OldMaxHealth = CurrentMaxHealth;

	// Clamp new values to valid ranges and apply them to the character's current stats.
	CurrentMaxHealth = FMath::Max(1.f, NewMaxHealth);
	CurrentMoveSpeed = FMath::Max(0.f, NewMoveSpeed);
	CurrentFireInterval = FMath::Max(0.01f, NewFireInterval);
	CurrentProjectileDamage = FMath::Max(0.f, NewProjectileDamage);
	CurrentProjectileSpeed = FMath::Max(0.f, NewProjectileSpeed);

	// Update movement speed in the character movement component
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = CurrentMoveSpeed;
	}

	// Only adjust current health after the run health has been initialized.
	// This avoids fake healing during first spawn before persistent health is restored.
	if (bRunHealthInitialised)
	{
		const float MaxHealthDelta = CurrentMaxHealth - OldMaxHealth;

		if (MaxHealthDelta > 0.f)
		{
			// Reward-style behaviour: gain the extra max health immediately
			CurrentHealth = FMath::Clamp(CurrentHealth + MaxHealthDelta, 0.f, CurrentMaxHealth);
		}
		else
		{
			CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, CurrentMaxHealth);
		}

		SaveHealthToGameInstance();
	}
}