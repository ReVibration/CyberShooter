// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TDSRoomDefinition.h"
#include "TDSGameInstance.generated.h"


UCLASS()
class UTDSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	/// Function to load the main menu level, can be called from Blueprints
	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	// Array of room definitions that will be used to generate the rooms for the current run.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Run")
	TArray<TObjectPtr<UTDSRoomDefinition>> CombatRooms;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Run")
	TArray<TObjectPtr<UTDSRoomDefinition>> RewardRooms;

	// This is the index of the current room that the player is in. It can be used to determine which room definition to use when generating the next room.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
	int32 CurrentRoomIndex = 0;

	// This is the seed that will be used to generate the rooms for the current run.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
	int32 RunSeed = 12345;

	// Function to start a new run with a specific seed, can be called from Blueprints
	UFUNCTION(BlueprintCallable, Category = "Run")
	void StartNewRun(int32 NewSeed);

	// Function to get the next room definition based on the current room index and the type of room that should be generated. 
	UFUNCTION(BlueprintCallable, Category = "Run")
	UTDSRoomDefinition* GetNextRoomDefinition();

	// Function to load the next room based on the current room index and the type of room that should be generated.
	UFUNCTION(BlueprintCallable, Category = "Run")
	void LoadNextRoom();
	
};
