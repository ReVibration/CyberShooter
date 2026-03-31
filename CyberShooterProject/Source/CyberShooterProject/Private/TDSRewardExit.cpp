// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSRewardExit.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "TDSGameInstance.h"

// Sets default values
ATDSRewardExit::ATDSRewardExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the root component and set it as the root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Create the static mesh component and attach it to the root
	ExitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExitMesh"));
	ExitMesh->SetupAttachment(SceneRoot);

	// Create the box component and attach it to the root
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(SceneRoot);

	// Set the collision settings for the trigger volume
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerVolume->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void ATDSRewardExit::BeginPlay()
{
	Super::BeginPlay();

	// Bind the overlap event to the function that will handle it
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATDSRewardExit::HandleTriggerBeginOverlap);

	// Initialize the exit as locked or unlocked based on the bStartUnlocked variable
	bExitUsed = false;
	SetExitUnlocked(bStartUnlocked);
}

// This function is called to unlock the exit, allowing the player to use it to progress to the next room.
void ATDSRewardExit::UnlockExit()
{
	SetExitUnlocked(true);
}

// This function is called to lock the exit, preventing the player from using it to progress to the next room.
void ATDSRewardExit::LockExit()
{
	SetExitUnlocked(false);
}

// This function is called to set the exit's unlocked state and update the trigger volume's collision settings accordingly. 
// It also calls a Blueprint event to allow for visual or audio feedback when the exit state changes
void ATDSRewardExit::SetExitUnlocked(bool bNewUnlocked)
{
	bExitUnlocked = bNewUnlocked;
	UpdateTriggerState();
	BP_OnExitStateChanged(bExitUnlocked);
}

// This function updates the collision settings of the trigger volume based on whether the exit is unlocked or not.
void ATDSRewardExit::UpdateTriggerState()
{
	if (!TriggerVolume)
	{
		return;
	}

	TriggerVolume->SetCollisionEnabled(
		bExitUnlocked ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

// Handle the event when the player enters the trigger box
void ATDSRewardExit::HandleTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// Check if the overlapping actor is the player character
	APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (!OverlappingPawn || !OverlappingPawn->IsPlayerControlled())
	{
		return;
	}
	AdvanceRunAndLoadNextRoom();
}


void ATDSRewardExit::AdvanceRunAndLoadNextRoom()
{
	// Get the game instance and load the next room
	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}
	// Increment the current room index and load the next room
	GI->CurrentRoomIndex++;
	GI->LoadNextRoom();
}
