// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameMode.h"
#include "TDSPlayerController.h"

ATDSGameMode::ATDSGameMode()
{
	// Set this game mode to use our custom player controller class 
	PlayerControllerClass = ATDSPlayerController::StaticClass();
}

