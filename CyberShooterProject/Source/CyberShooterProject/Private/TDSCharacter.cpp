// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

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

	// Create the camera boom (spring arm)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 900.0f;
	SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;

	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	// Create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	// Create the muzzle location
	const FVector SpawnLocation = Muzzle ? Muzzle->GetComponentLocation()
		: GetActorLocation() + GetActorForwardVector() * 80.f;
	const FRotator SpawnRotation = GetActorRotation();



}

// Called when the game starts or when spawned
void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (DefaultMappingContext)
					// Add the mapping context
					Subsystem->AddMappingContext(DefaultMappingContext, 0);

			}
		}
		PC->bShowMouseCursor = true;
		PC->DefaultMouseCursor = EMouseCursor::Crosshairs;
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

	EnhancedInputComponent->BindAction(
		FireAction,
		ETriggerEvent::Started,
		this,
		&ATDSCharacter::StartFiring
	);

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
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		return;

	FVector ToTarget = Hit.ImpactPoint - GetActorLocation();
	ToTarget.Z = 0.f; // keep rotation flat

	if (ToTarget.IsNearlyZero())
		return;

	FRotator TargetRotation = ToTarget.Rotation();

	FRotator SmoothedRotation = FMath::RInterpTo(
		GetActorRotation(),
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		RotationSpeed
	);

	SetActorRotation(SmoothedRotation);
}
