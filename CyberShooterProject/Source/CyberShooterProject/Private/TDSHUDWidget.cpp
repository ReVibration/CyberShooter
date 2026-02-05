// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSHUDWidget.h"
#include "Components/ProgressBar.h"
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
	if (Player && PB_Health)
	{
		// Update the health bar percentage based on the player's current health
		PB_Health->SetPercent(Player->GetHealthPercent());
	}
	else
	{
		return;
	}
}
