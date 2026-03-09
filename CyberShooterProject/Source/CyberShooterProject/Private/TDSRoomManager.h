// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

private:
	// The number of alive enemies in the room. This will be updated as enemies are killed.
	UPROPERTY(VisibleAnywhere, Category= "Room")
	int32 AliveEnemyCount = 0;

};
