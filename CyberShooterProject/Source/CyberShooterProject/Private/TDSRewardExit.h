// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDSRewardExit.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USceneComponent;
class UPrimitiveComponent;
class AActor;
class USoundBase;

UCLASS()
class ATDSRewardExit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDSRewardExit();

	// This functions as the unlock and lock mechanism for the exit.
	UFUNCTION(BlueprintCallable, Category = "Exit")
	void UnlockExit();

	UFUNCTION(BlueprintCallable, Category = "Exit")
	void LockExit();

	// This function checks if the exit is currently unlocked.
	UFUNCTION(BlueprintPure, Category = "Exit")
	bool IsExitUnlocked() const { return bExitUnlocked; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// This is the component that will be used as the root component for this actor
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> SceneRoot;

	// This is the static mesh component that will be used to visually represent the exit
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ExitMesh;

	// This is the box component that will be used as the trigger for the reward exit
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> TriggerVolume;

	// This variable determines whether the start is unlocked or not.
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Exit")
	bool bStartUnlocked = false;

	// This variable determines whether the exit is unlocked or not.
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Exit")
	bool bExitUnlocked = false;

	// This variable determines whether the exit has been used or not. 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Exit")
	bool bExitUsed = false;

	// This function will be called when the player enters the trigger box
    UFUNCTION()
    void HandleTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

	// Use this in Blueprint to swap materials, VFX, lights, widget prompts, etc.
	UFUNCTION(BlueprintImplementableEvent, Category = "Exit")
	void BP_OnExitStateChanged(bool bNowUnlocked);

	// Sound to play when the exit is unlocked
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Exit")
	TObjectPtr<USoundBase> UnlockSound;

	// Volume to play the unlock sound at
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Exit", meta = (ClampMin = "0.0"))
	float UnlockSoundVolume = 1.0f;

private:
	// This function will set the exit to be unlocked or locked and update the trigger state accordingly.
	void SetExitUnlocked(bool bNewUnlocked);
	void UpdateTriggerState();
	// This function will handle the logic for advancing the run and loading the next room when the exit is used.
	void AdvanceRunAndLoadNextRoom();
};
