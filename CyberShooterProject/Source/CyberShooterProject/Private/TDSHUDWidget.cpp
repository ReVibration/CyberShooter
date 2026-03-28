// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TDSCharacter.h"

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