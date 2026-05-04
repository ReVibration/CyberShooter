// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TDSRoomDefinition.h"
#include "TDSRunStats.h"
#include "TDSGameInstance.generated.h"

// Forward declaration of UTDSRunData, which is used to manage the player's owned upgrades during a run. 
// This is not included in this header to avoid circular dependencies, 
// as UTDSRunData does not need to be directly referenced in most of the functions defined in this class.
class UTDSRunData;

class USoundBase;
class UAudioComponent;

UCLASS()
class UTDSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// This is the run data object that will be used to manage the player's owned upgrades during the current run. 
	// It is created when the game instance is initialized and reset when a new run is started.
	UFUNCTION(BlueprintCallable)
	UTDSRunData* GetRunData();

	// This struct holds the current run stats
	UPROPERTY(BlueprintReadOnly)
	FTDSRunStats CurrentRunStats;

	UFUNCTION(BlueprintCallable)
	void ResetRunData();

	/// Functions to manage the run stats, these can be called from Blueprints or C++ code to update the stats during the run and to finalise them when the run ends.
	void ResetRunStats();
	void RecordRoomCleared();
	void RecordEnemyEliminated();
	const FTDSRunStats& GetCurrentRunStats() const;

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
	
	// This ensures that the same room is not generated twice in a row, by keeping track of the last generated room index for both combat and reward rooms.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
	int32 LastCombatRoomIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
	int32 LastRewardRoomIndex = INDEX_NONE;

	// Functions to manage the player's health during the run, these can be called from Blueprints.
	UFUNCTION(BlueprintCallable)
	void InitialiseRunPlayerHealth(float InMaxHealth);

	UFUNCTION(BlueprintCallable)
	void SaveRunHealth(float InCurrentHealth, float InMaxHealth);

	UFUNCTION(BlueprintCallable)
	bool HasRunPlayerHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetRunCurrentHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetRunMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	void ClearRunPlayerHealth();

	UFUNCTION(BlueprintCallable, Category = "Audio|Music")
	void PlayMenuMusic();

	UFUNCTION(BlueprintCallable, Category = "Audio|Music")
	void PlayGameplayMusic();

	UFUNCTION(BlueprintCallable, Category = "Audio|Music")
	void StopMusic();

	virtual void Init() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Music")
	TObjectPtr<USoundBase> MenuMusic;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Music")
	TObjectPtr<USoundBase> GameplayMusic;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Music", meta = (ClampMin = "0.0"))
	float MusicVolume = 0.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Music", meta = (ClampMin = "0.0"))
	float MusicFadeTime = 1.0f;

private:

	/// This is the run data object that will be used to manage the player's owned upgrades during the current run.
	UPROPERTY()
	TObjectPtr<UTDSRunData> RunData;

	/// These variables are used to store the player's health information during the run, they are private and can only be accessed through the public functions defined above.
	UPROPERTY()
	bool bHasStoredRunPlayerHealth = false;

	UPROPERTY()
	float StoredRunCurrentHealth = 0.0f;

	UPROPERTY()
	float StoredRunMaxHealth = 0.0f;

	UPROPERTY()
	TObjectPtr<UAudioComponent> CurrentMusicComponent;

	UPROPERTY()
	TObjectPtr<USoundBase> CurrentMusic;

	void PlayMusic(USoundBase* NewMusic);

	bool bIsLoadingRoom = false;

	void HandlePostLoadMapWithWorld(UWorld* LoadedWorld);

};
