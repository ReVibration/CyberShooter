// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDSEnemyCharacter.h"
#include "TDSRoomManager.generated.h"


UCLASS()
class ATDSRoomManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDSRoomManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Handle the death of an enemy and update the alive enemy count
	UFUNCTION()
	void HandleEnemyDied(AActor* DeadEnemy);

	// When the room is cleared, this function will be called to perform any necessary actions (e.g., opening doors, spawning rewards)
	void OnRoomCleared();

	// This function will be called to spawn enemies in the room based on the current room index and the number of available spawners.
	void SpawnRoomEnemies();

private:
	// The number of alive enemies in the room. This will be updated as enemies are killed.
	UPROPERTY(VisibleAnywhere, Category= "Room")
	int32 AliveEnemyCount = 0;

	// The default enemy class to spawn in
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ATDSEnemyCharacter> DefaultEnemyClass;

	// The minimum number of enemies to spawn in the room
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 BaseEnemyCount = 1;

	// The number of additional enemies to spawn for every X rooms cleared, allowing the difficulty to scale as the player progresses
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 AdditionalEnemyEveryXRooms = 2;

	// The maximum number of enemies that can be spawned in the room, to prevent overwhelming the player
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 MaxEnemyCount = 6;

	// Calculate the number of enemies to spawn helper function based on the current room index and the number of available spawners in the room. 
	// This allows for dynamic scaling of enemy count while ensuring it does not exceed the number of spawners.
	int32 CalculateSpawnCount(int32 RoomIndex, int32 AvailableSpawnerCount) const;

};
