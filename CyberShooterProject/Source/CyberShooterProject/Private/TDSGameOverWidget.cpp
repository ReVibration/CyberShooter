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
			FText::FromString(FString::Printf(TEXT("Rooms Cleared: %d"), CachedRunStats.RoomsCleared))
		);
	}

	// Update the Enemies Eliminated text block
	if (EnemiesEliminatedText)
	{
		EnemiesEliminatedText->SetText(
			FText::FromString(FString::Printf(TEXT("Enemies Eliminated: %d"), CachedRunStats.EnemiesEliminated))
		);
	}

	// Update the Time Survived text block with formatted time
	if (TimeSurvivedText)
	{
		TimeSurvivedText->SetText(
			FText::FromString(FString::Printf(TEXT("Time Survived: %s"), *FormatTime(CachedRunStats.TimeSurvivedSeconds)))
		);
	}
}

// FormatTime converts a total time in seconds into a MM:SS format string for display on the game over screen.
FString UTDSGameOverWidget::FormatTime(float TotalSeconds) const
{
	// Ensure that we don't display negative time and that we round down to the nearest whole second for display purposes.
	const int32 TotalWholeSeconds = FMath::Max(0, FMath::FloorToInt(TotalSeconds));
	const int32 Minutes = TotalWholeSeconds / 60;
	const int32 Seconds = TotalWholeSeconds % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}
