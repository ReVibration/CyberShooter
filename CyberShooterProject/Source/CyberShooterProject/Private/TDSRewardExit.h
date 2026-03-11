// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDSRewardExit.generated.h"

class UBoxComponent;

UCLASS()
class ATDSRewardExit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDSRewardExit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// This is the component that will be used as the root component for this actor
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Root;

	// This is the box component that will be used as the trigger for the reward exit
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> TriggerBox;

	// This function will be called when the player enters the trigger box
    UFUNCTION()
    void OnTriggerEntered(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );


};
