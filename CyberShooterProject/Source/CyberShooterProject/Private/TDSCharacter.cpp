// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSCharacter.h"

#include "TDSGameMode.h"
#include "TDSPlayerController.h"

#include "Camera/CameraComponent.h"
#include "TDSSpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"
#include "Engine/Engine.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "TDSHUDWidget.h"
#include "Blueprint/UserWidget.h"

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

	// Create the muzzle location
	const FVector SpawnLocation = Muzzle ? Muzzle->GetComponentLocation()
		: GetActorLocation() + GetActorForwardVector() * 80.f;
	const FRotator SpawnRotation = GetActorRotation();

	// ===== Prevent mesh from blocking camera =====
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

}

// Called when the game starts or when spawned
void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize health
	CurrentHealth = MaxHealth;

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

}

void ATDSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FaceMouseCursor();
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

	// Bind the fire action
	EnhancedInputComponent->BindAction(
		FireAction,
		ETriggerEvent::Started,
		this,
		&ATDSCharacter::StartFiring
	);

	// Bind the stop fire action
	EnhancedInputComponent->BindAction(
		FireAction,
		ETriggerEvent::Completed,
		this,
		&ATDSCharacter::StopFiring
	);

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
	//  Avoid starting if already firing
	if (bIsFiring) return;
	// Set the firing flag
	bIsFiring = true;

	FireOnce(); // shoot immediately

	// then keep firing repeatedly
	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ATDSCharacter::FireOnce,
		FireRate,
		true
	);
}

void ATDSCharacter::StopFiring()
{
	// If not firing, do nothing
	bIsFiring = false;
	// Clear the timer
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ATDSCharacter::FireOnce()
{
	// Make sure we have a projectile class to spawn
	if (!ProjectileClass) return;

	// Determine spawn location and rotation
	const FVector SpawnLocation = Muzzle
		? Muzzle->GetComponentLocation() + FVector(0, 0, 20)
		: GetActorLocation() + GetActorForwardVector() * 80.f + FVector(0, 0, 20);

	// Set spawn rotation to be the same as the character's rotation
	const FRotator SpawnRotation = GetActorRotation();

	// Set spawn parameters
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn the projectile
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
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

	// Decrease health and clamp to valid range
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	// Check for death
	if(CurrentHealth <= 0.f)
	{
		ATDSGameMode* GM = Cast<ATDSGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->HandleGameOver(GetController());
		}
		
	}
}

void ATDSCharacter::Heal(float Amount)
{
	if (Amount <= 0.f) return; // Ignore non-positive healing
	// Increase health and clamp to valid range
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
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
