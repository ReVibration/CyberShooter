// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
	void ShowHUD();
	void HideHUD();

protected:
	

	//Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// UI BUILDERS to build UI for different game states
	void ShowMainMenu();

	
	// INPUT MODES for different game states
	void SetGameInputMode();
	void SetMenuInputMode();
	void SetGameOverInputMode();

	//Reference to the main menu widget class, set in the editor
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UUserWidget> MainMenuClass;

	//Reference to the game over widget class, set in the editor
	UPROPERTY(EditAnywhere, Category= "UI")
	TSubclassOf<class UUserWidget> GameOverClass;

	// Reference to the HUD of the game for the player
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UTDSHUDWidget> HUDClass;

	// ACTIVE WIDGET instances for different menus
	UPROPERTY()
	UUserWidget* ActiveMainMenu = nullptr;

	UPROPERTY()
	UTDSHUDWidget* ActiveHUD = nullptr;

	UPROPERTY()
	UUserWidget* ActiveGameOver = nullptr;

	// A method to clear all UI
	void ClearAllUI();
};
