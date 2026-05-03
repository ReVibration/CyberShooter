// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "Camera/CameraShakeBase.h"
#include "Animation/AnimMontage.h"
#include "TDSCharacter.generated.h"


class UTDSHUDWidget;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ATDSProjectile;

UCLASS()
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDSCharacter();
	
	// ---------------- Public Functions & Properties ----------------

	// This function will apply the recalculated stats from the upgrade component to the character's current stats, and will be called whenever an upgrade is applied or removed
	void ApplyRecalculatedStats(
		float NewMaxHealth,
		float NewMoveSpeed,
		float NewFireInterval,
		float NewProjectileDamage,
		float NewProjectileSpeed);

	// The static mesh component for the player's weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	// Returns the current health percentage
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const { return (CurrentMaxHealth > 0.f) ? (CurrentHealth / CurrentMaxHealth) : 0.f; }

	// Returns the current health value
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	// Returns the maximum health value
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const { return CurrentMaxHealth; }


	float GetBaseMaxHealth() const { return BaseMaxHealth; }
	float GetBaseMoveSpeed() const { return BaseMoveSpeed; }
	float GetBaseFireInterval() const { return BaseFireInterval; }
	float GetBaseProjectileDamage() const { return BaseProjectileDamage; }
	float GetBaseProjectileSpeed() const { return BaseProjectileSpeed; }

	float GetCurrentFireInterval() const { return CurrentFireInterval; }
	float GetCurrentProjectileDamage() const { return CurrentProjectileDamage; }
	float GetCurrentProjectileSpeed() const { return CurrentProjectileSpeed; }

	// Applies healing to the character, ensuring it does not exceed MaxHealth
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	// Create a player mesh component to visualize the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* PlayerMesh;

	// Whether the character is currently dead
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	// The animation montage to play when the character dies
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	// The component that manages the character's upgrades and their effects on stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UTDSUpgradeComponent> UpgradeComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every game tick to update
	virtual void Tick(float DeltaSeconds) override;

	// Called when the game starts to setup inputs
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// ---------------- Audio ----------------
	// The sound to play when firing the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Combat")
	TObjectPtr<USoundBase> FireSound;

	// The volume to play when firing the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Combat", meta = (ClampMin = "0.0"))
	float FireSoundVolume = 1.0f;

	// The pitch to play the fire sound at
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Combat", meta = (ClampMin = "0.0"))
	float FireSoundPitch = 1.0f;

	// The amount of random variation to apply to the fire sound's pitch each time it is played, 
	// used to prevent the sound from becoming repetitive
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Combat", meta = (ClampMin = "0.0"))
	float FireSoundPitchVariation = 0.05f;

	// The sound to play when the character takes damage
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Damage")
	TObjectPtr<USoundBase> DamageSound;

	// The volume to play when the character takes damage
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Damage", meta = (ClampMin = "0.0"))
	float DamageSoundVolume = 1.0f;

	// The sound to play when the character dies
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Damage")
	TObjectPtr<USoundBase> DeathSound;

	// The volume to play when the character dies
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Damage", meta = (ClampMin = "0.0"))
	float DeathSoundVolume = 1.0f;

private:
	// ---------------- Components ----------------

	// --- RECOIL ---
	// The distance the weapon will move back when firing, and how quickly it will return to its original position.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float RecoilDistance = 6.f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float RecoilReturnSpeed = 25.f;

	// The Niagara system to use for the muzzle flash effect when firing.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Effects")
	UNiagaraSystem* MuzzleFlashEffect;

	// The camera shake class to use when firing the weapon.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Effects")
	TSubclassOf<UCameraShakeBase> FireCameraShakeClass;

	// The default relative location of the weapon mesh, and the current offset from that location due to recoil.
	FVector WeaponDefaultRelativeLocation = FVector::ZeroVector;
	FVector WeaponCurrentOffset = FVector::ZeroVector;

	// --- HUD ---

	// The HUD widget class to use for the player's HUD
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTDSHUDWidget> HUDWidgetClass;

	// The instance of the HUD widget that is currently being used
	UPROPERTY()
	UTDSHUDWidget* HUDWidget = nullptr;


	// --- Health ---

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Base")
	float BaseMaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Current")
	float CurrentMaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth = 0.f;

	// --- Movement ---

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Base")
	float BaseMoveSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Current")
	float CurrentMoveSpeed = 600.f;

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

	// Mapping for pause
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PauseAction;

	// --- Combat ---

	// This allows us to change the projectile when we want to
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class ATDSProjectile> ProjectileClass;

	// A property that can change the fire rate 
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float BaseFireInterval = 0.25f;

	// This is the current fire interval, which can be modified by upgrades and will be used to manage the firing rate
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float CurrentFireInterval = BaseFireInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Base")
	float BaseProjectileDamage = 25.f;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Current")
	float CurrentProjectileDamage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Base")
	float BaseProjectileSpeed = 2000.f;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Current")
	float CurrentProjectileSpeed = 2000.f;

	// Timer handle to manage the firing rate
	FTimerHandle FireTimerHandle;
	// Whether the player is currently firing
	bool bIsFiring = false;

	// The camera shake class to use when taking damage
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Effects")
	TSubclassOf<UCameraShakeBase> DamageCameraShakeClass;

	// Tracks whether run health has been restored/initialized yet
	bool bRunHealthInitialised = false;

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
	
	// Called when the player wants to pause the game
	UFUNCTION()
	void HandlePausePressed();

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

	// Handles the character's death, including playing the death animation, spawning effects, and disabling input
	void HandleDeath();

	void RestoreHealthFromGameInstance();
	void SaveHealthToGameInstance();


};
