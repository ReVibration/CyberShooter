// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDSCharacter.generated.h"

class UTDSHUDWidget;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDSCharacter();
	
	// ---------------- Public Functions ----------------

	// Returns the current health percentage
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const { return (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f; }

	// Applies healing to the character, ensuring it does not exceed MaxHealth
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every game tick to update
	virtual void Tick(float DeltaSeconds) override;

	// Called when the game starts to setup inputs
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// ---------------- Components ----------------

	// --- HUD ---

	// The HUD widget class to use for the player's HUD
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTDSHUDWidget> HUDWidgetClass;

	// The instance of the HUD widget that is currently being used
	UPROPERTY()
	UTDSHUDWidget* HUDWidget = nullptr;

	// --- Health ---

	// The maximum health of the character
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.f;

	// The current health of the character
	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth;

	// --- Camera ---

	// The spring arm that holds the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UTDSSpringArmComponent* CameraBoom;

	// The camera component
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* Camera;

	// The sphere to use to make sure we are not clipping into the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess="true"))
	class USphereComponent* CameraMinDistanceSphere;

	// --- Aiming ---

	// How fast the character rotates to face the mouse cursor
	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	float RotationSpeed = 2.f;

	// --- Enhanced Input ---

	// This is the mapping context for the controller
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext;

	// Mapping for movement
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;
	
	// Mapping for fire
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

	// --- Combat ---

	// This allows us to change the projectile when we want to
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	USceneComponent* Muzzle;

	// A property that can change the fire rate 
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float FireInterval = 0.25f;

	// Timer handle to manage the firing rate
	FTimerHandle FireTimerHandle;
	// Whether the player is currently firing
	bool bIsFiring = false;

	// ---------------- Functions ----------------

	// Called when the character takes damage
	UFUNCTION()
	void HandleTakeAnyDamage(
		AActor* DamagedActor,
		float Damage,
		const class UDamageType* DamageType,
		class AController* InstigatedBy, 
		AActor* DamageCauser
	);

	// Called when the player want's to move
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// Handles the rotation of the character to face the mouse cursor
	void FaceMouseCursor();

	// Handles starting the firing of projectiles
	void StartFiring();

	// Handles stopping the firing of projectiles
	void StopFiring();

	// Handles firing a single projectile
	void FireOnce();

	// This will get the mouse aim point on the player to make sure that walls are not affecting rotation
	bool GetMouseAimPointOnPlayerPlane(APlayerController& PC, FVector& OutAimPoint) const;
};
