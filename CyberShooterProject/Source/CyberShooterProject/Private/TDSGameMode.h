// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TDSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ATDSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	// Sets default values for this game mode's properties
	ATDSGameMode();

	// Handle the game over if the player has died
	UFUNCTION()
	void HandleGameOver(AController* DeadController);

	// Load the next room when the player reaches the end of the current room
	UFUNCTION(BlueprintCallable, Category = "Rooms")
	void LoadNextRoom();

};
