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

protected:
	
	//Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Functions to show the main menu and game over screens, implemented in Blueprints
	void ShowMainMenu();
	void ShowGameOver();

	//Reference to the main menu widget class, set in the editor
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UUserWidget> MainMenuClass;

	//Reference to the game over widget class, set in the editor
	UPROPERTY(EditAnywhere, Category= "UI")
	TSubclassOf<class UUserWidget> GameOverClass;
};
