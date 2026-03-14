// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDSEnemyCharacter.h"
#include "TDSEnemySpawner.generated.h"

UCLASS()
class ATDSEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDSEnemySpawner();

	/// Spawns an enemy of the specified class at the spawner's location and rotation.
	UFUNCTION(BlueprintCallable)
	ATDSEnemyCharacter* SpawnEnemy(TSubclassOf<ATDSEnemyCharacter> EnemyClass);

protected:
	
	// Called when the game starts or when spawned
	UPROPERTY(EditAnywhere, Category = "Spawning")
	USceneComponent* Root;


};
