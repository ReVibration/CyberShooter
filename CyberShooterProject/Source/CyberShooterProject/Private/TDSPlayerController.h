// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDSGameOverWidget.h"
#include "TDSPauseMenuWidget.h"
#include "TDSPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class ATDSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	// Function to show the game over screen
	void ShowGameOver();
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable)
	void HidePauseMenu();
	void ShowHUD();
	void HideHUD();

	void TogglePauseMenu();

	// Function to show a damage flash on the screen when the player takes damage
	UFUNCTION()
	void ShowDamageFlash();

	// Function to get the active HUD widget, so other classes can call functions on it (like showing the damage flash)
	UFUNCTION(BlueprintCallable)
	UTDSHUDWidget* GetHUDWidget() const { return ActiveHUD; }

protected:
	

	//Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// UI BUILDERS to build UI for different game states
	void ShowMainMenu();

	
	// INPUT MODES for different game states
	void SetGameInputMode();
	void SetMenuInputMode();
	void SetGameOverInputMode();
	void SetPauseMenuInputMode();

	//Reference to the main menu widget class, set in the editor
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UUserWidget> MainMenuClass;

	//Reference to the game over widget class, set in the editor
	UPROPERTY(EditAnywhere, Category= "UI")
	TSubclassOf<UTDSGameOverWidget> GameOverClass;

	// Reference to the HUD of the game for the player
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UTDSHUDWidget> HUDClass;

	// Reference to the pause menu widget class, set in the editor
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTDSPauseMenuWidget> PauseMenuClass;

	// ACTIVE WIDGET instances for different menus
	UPROPERTY()
	UUserWidget* ActiveMainMenu = nullptr;

	UPROPERTY()
	UTDSHUDWidget* ActiveHUD = nullptr;

	UPROPERTY()
	UTDSGameOverWidget* ActiveGameOver = nullptr;

	UPROPERTY()
	UTDSPauseMenuWidget* ActivePauseMenu = nullptr;

	// A method to clear all UI
	void ClearAllUI();
};

