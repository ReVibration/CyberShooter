// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TDSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UTDSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	/// Function to load the main menu level, can be called from Blueprints
	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	/// Function to start a new run by loading the main game level, can be called from Blueprints
	UFUNCTION(BlueprintCallable)
	void StartNewRun();
	
};
