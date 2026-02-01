// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TDSEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ATDSEnemyAIController : public AAIController
{
	GENERATED_BODY()


public:
	// Sets default values for this controller's properties
	virtual void OnPossess(APawn* InPawn) override;
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	// Distance at which the AI will start chasing the player
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ChaseDistance = 2000.0f;

	// Distance at which the AI will stop moving towards the player
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float StopDistance = 150.0f;

private:
	// Reference to the player pawn
	UPROPERTY()
	APawn* PlayerPawn = nullptr;

};


