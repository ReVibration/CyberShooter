// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UTDSGameInstance::LoadMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UTDSGameInstance::StartNewRun() 
{ 
	UGameplayStatics::OpenLevel(this, FName("GameLevel")); 
}