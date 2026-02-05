// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDSHUDWidget.generated.h"

// Forward declarations
class UProgressBar;
class ATDSCharacter;

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
	
protected:
	// Tick function to update the health bar
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Reference to the health bar progress bar widget
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_Health = nullptr;

private:
	// Reference to the player character to access health information
	UPROPERTY()
	ATDSCharacter* Player = nullptr;
};
