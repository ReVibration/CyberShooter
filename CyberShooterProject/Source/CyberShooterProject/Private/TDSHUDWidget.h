// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDSHUDWidget.generated.h"

// Forward declarations
class UProgressBar;
class UTextBlock;
class UVerticalBox;
class ATDSCharacter;
class ATDSEnemyCharacter;

/**
 * 
 */
UCLASS()
class UTDSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called when the widget is constructed
	void SetPlayer(ATDSCharacter* InPlayer);

	// Function to show a damage flash on the screen when the player takes damage
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageFlash();

	// Function to show the enemy health bar when attacking an enemy, and hide it when not attacking
	void ShowEnemyHealth(ATDSEnemyCharacter* HitEnemy);
	void HideEnemyHealth();
	
protected:
	// Tick function to update the health bar
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Reference to the health bar progress bar widget
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_Health = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerHealthText = nullptr;

	// Reference to the enemy health bar progress bar widget
	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyHealthBar = nullptr;

	// Reference to the enemy health text widget
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyHealthText = nullptr;

	// Container for the enemy health bar and text, so we can show/hide them together
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* EnemyHealthContainer = nullptr;

	// Reference to the currently tracked enemy, so we can update its health display and hide it when necessary
	UPROPERTY()
	ATDSEnemyCharacter* CurrentTrackedEnemy = nullptr;

	// Timer handle for hiding the enemy health bar after a delay when not attacking
	FTimerHandle EnemyHealthHideTimerHandle;

	void UpdateHealthDisplay();
	void UpdateEnemyHealthDisplay();
private:
	// Reference to the player character to access health information
	UPROPERTY()
	ATDSCharacter* Player = nullptr;
};
