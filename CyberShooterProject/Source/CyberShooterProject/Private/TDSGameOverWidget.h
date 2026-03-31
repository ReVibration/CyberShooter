// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDSRunStats.h"
#include "TDSGameOverWidget.generated.h"

class UTextBlock;

UCLASS()
class UTDSGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// Sets the run stats to be displayed on the game over screen and updates the text blocks accordingly.
	void SetRunStats(const FTDSRunStats& InRunStats);

protected:
	virtual void NativeConstruct() override;
	// Bindings to the text blocks in the widget blueprint
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoomsClearedText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemiesEliminatedText;

private:
	// Cache the run stats to avoid unnecessary updates
	FTDSRunStats CachedRunStats;

	// Update the text blocks with the current run stats
	void RefreshStatsText();
};

