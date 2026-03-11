// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSRewardExit.h"
#include "Components/BoxComponent.h"
#include "TDSCharacter.h"
#include "TDSGameInstance.h"

// Sets default values
ATDSRewardExit::ATDSRewardExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component and set it as the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Create the trigger box component and attach it to the root
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
}

// Called when the game starts or when spawned
void ATDSRewardExit::BeginPlay()
{
	Super::BeginPlay();

	// Bind the overlap event to the OnTriggerEntered function
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATDSRewardExit::OnTriggerEntered);
}

// Handle the event when the player enters the trigger box
void ATDSRewardExit::OnTriggerEntered(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
	// Check if the overlapping actor is the player character
    ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(OtherActor);
    if (!PlayerCharacter)
    {
        return;
    }

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

