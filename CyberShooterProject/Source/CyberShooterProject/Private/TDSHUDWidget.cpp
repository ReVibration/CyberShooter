// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "TDSCharacter.h"
#include "TDSGameInstance.h"
#include "TDSEnemyCharacter.h"
#include "TimerManager.h"

// Set the player to access health information
void UTDSHUDWidget::SetPlayer(ATDSCharacter* InPlayer)
{
	Player = InPlayer;
}

void UTDSHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// Always call the parent implementation first
	Super::NativeTick(MyGeometry, InDeltaTime);
	// Update the health bar if we have a valid player reference
	UpdateHealthDisplay();
	RefreshStatsText();

}

void UTDSHUDWidget::UpdateHealthDisplay()
{
	// If we don't have a valid player reference, we can't update the health display, so we return early.
	if (!Player)
	{
		return;
	}

	// Get the current health and max health from the player character, and calculate the health percentage for the progress bar.
	const float CurrentHealth = Player->GetCurrentHealth();
	const float MaxHealth = Player->GetMaxHealth();

	// Update the health bar progress and the health text if the widgets are valid.
	if (PB_Health)
	{
		PB_Health->SetPercent(Player->GetHealthPercent());
	}

	// Update the health text to show the current health and max health in a "Health X/Y" format, rounding the values to integers for display.
	if (PlayerHealthText)
	{
		PlayerHealthText->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("Health %d/%d"),
					FMath::RoundToInt(CurrentHealth),
					FMath::RoundToInt(MaxHealth)
				)
			)
		);
	}
}

// Show the enemy health bar and text when attacking an enemy, and set a timer to hide it after a short duration of not attacking
void UTDSHUDWidget::ShowEnemyHealth(ATDSEnemyCharacter* HitEnemy)
{
	// If we don't have a valid enemy reference, we can't show the enemy health, so we return early.
	if (!HitEnemy)
	{
		return;
	}

	// Set the current tracked enemy to the hit enemy, so we can update its health display and hide it when necessary.
	CurrentTrackedEnemy = HitEnemy;
	UpdateEnemyHealthDisplay();

	// Show the enemy health container if it's valid, and set a timer to hide it after 2 seconds of not attacking.
	if (EnemyHealthContainer)
	{
		EnemyHealthContainer->SetVisibility(ESlateVisibility::Visible);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(EnemyHealthHideTimerHandle);
		World->GetTimerManager().SetTimer(
			EnemyHealthHideTimerHandle,
			this,
			&UTDSHUDWidget::HideEnemyHealth,
			2.0f,
			false
		);
	}
}

// Hide the enemy health bar and text when not attacking, and clear the current tracked enemy reference
void UTDSHUDWidget::HideEnemyHealth()
{
	// Clear the current tracked enemy reference, so we stop updating its health display and hide it when necessary.
	CurrentTrackedEnemy = nullptr;

	//	Hide the enemy health container if it's valid, so we stop showing the enemy health bar and text when not attacking.
	if (EnemyHealthContainer)
	{
		EnemyHealthContainer->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Update the enemy health bar and text based on the current tracked enemy's health information, if we have a valid reference to an enemy
void UTDSHUDWidget::UpdateEnemyHealthDisplay()
{
	// If we don't have a valid reference to an enemy, we can't update the enemy health display, so we return early.
	if (!CurrentTrackedEnemy)
	{
		return;
	}

	// Get the current health and max health from the current tracked enemy, and calculate the health percentage for the progress bar.
	const float CurrentHealth = CurrentTrackedEnemy->GetCurrentHealth();
	const float MaxHealth = CurrentTrackedEnemy->GetMaxHealth();

	const float HealthPercent = MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;

	// Update the enemy health bar progress and the enemy health text if the widgets are valid.
	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetPercent(HealthPercent);
	}

	// Update the enemy health text to show the current health and max health in a "Target X/Y" format, rounding the values to integers for display.
	if (EnemyHealthText)
	{
		EnemyHealthText->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("Target %d/%d"),
					FMath::RoundToInt(CurrentHealth),
					FMath::RoundToInt(MaxHealth)
				)
			)
		);
	}
}

void UTDSHUDWidget::RefreshStatsText()
{
	if (UTDSGameInstance* GI = GetGameInstance<UTDSGameInstance>()) {
		CurrentRunStats = GI->GetCurrentRunStats();
	}

	// This function can be used to update any additional stats text on the HUD, such as enemies killed or objectives.
	if (EnemiesKilledText) {
		EnemiesKilledText->SetText(
			FText::FromString(FString::Printf(TEXT("Enemies Eliminated: %d"), CurrentRunStats.EnemiesEliminated)));
	}
}

void UTDSHUDWidget::SetObjectiveText(const FString& InObjectiveText)
{
	if (ObjectiveText)
	{
		ObjectiveText->SetText(FText::FromString(InObjectiveText));
	}
}