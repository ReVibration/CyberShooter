// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameOverWidget.h"
#include "Components/TextBlock.h"
#include "Math/UnrealMathUtility.h"

// NativeConstruct is called when the widget is constructed and added to the viewport. We use it to initialize the text blocks with the cached run stats.
void UTDSGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshStatsText();
}

// SetRunStats is called to update the run stats displayed on the game over screen. It caches the stats and refreshes the text blocks.
void UTDSGameOverWidget::SetRunStats(const FTDSRunStats& InRunStats)
{
	CachedRunStats = InRunStats;
	RefreshStatsText();
}

// RefreshStatsText updates the text blocks with the current run stats. It formats the time survived into a MM:SS format for better readability.
void UTDSGameOverWidget::RefreshStatsText()
{
	// Update the Rooms Cleared text block
	if (RoomsClearedText)
	{
		RoomsClearedText->SetText(
			FText::FromString(FString::Printf(TEXT("Enemy Rooms Cleared: %d"), CachedRunStats.RoomsCleared))
		);
	}

	// Update the Enemies Eliminated text block
	if (EnemiesEliminatedText)
	{
		EnemiesEliminatedText->SetText(
			FText::FromString(FString::Printf(TEXT("Enemies Eliminated: %d"), CachedRunStats.EnemiesEliminated))
		);
	}


}

