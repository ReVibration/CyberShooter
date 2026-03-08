// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TDSRoomDefinition.generated.h"

// This is the enumeration for the different types of rooms in the game. It can be used to define the type of room in the TDSRoomDefinition data asset.
UENUM(BlueprintType)
enum class ETDSRoomType : uint8
{
    Combat,
    Reward,
    Boss
};

UCLASS(BlueprintType)
class CYBERSHOOTERPROJECT_API UTDSRoomDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// The type of room, which can be Combat, Reward, or Boss. This will determine the type of enemies and rewards that will be present in the room.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    ETDSRoomType RoomType = ETDSRoomType::Combat;

	// The name of the level that will be loaded when the player enters this room. This should correspond to a level that has been created in the Unreal Editor.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    FName LevelName;

};
