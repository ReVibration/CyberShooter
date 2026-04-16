// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSUpgradePickup.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TDSCharacter.h"
#include "TDSGameInstance.h"
#include "TDSRunData.h"
#include "TDSUpgradeDefinition.h"

// Sets default values
ATDSUpgradePickup::ATDSUpgradePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and set up components
	// Create a default root component and set it as the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Create a static mesh component for the pickup's visual representation and attach it to the root
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(Root);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create a box component for detecting overlaps with the player character and attach it to the root
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(Root);
	OverlapBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ATDSUpgradePickup::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("UpgradePickup BeginPlay: %s"), *GetName());
	// Bind the overlap event to the OnOverlapBegin function. If the OverlapBox component is missing, log an error.
	if (OverlapBox)
	{
		OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ATDSUpgradePickup::OnOverlapBegin);

		UE_LOG(LogTemp, Warning, TEXT("UpgradePickup overlap bound: %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UpgradePickup has no OverlapBox: %s"), *GetName());
	}
}

// On the overlap event.
void ATDSUpgradePickup::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("UpgradePickup overlap triggered. OtherActor: %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("None"));

	// If the pickup has already been collected, do nothing
	if (bCollected)
	{
		return;
	}

	// If the overlapping actor is not a player character, do nothing
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(OtherActor);
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap was not player character."));
		return;
	}

	// Try to grant the upgrade to the player character. If it fails (e.g. due to upgrade limits), do nothing
	if (!TryGrantUpgrade(PlayerCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade failed."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Collected upgrade pickup: %s"), *UpgradeDefinition->DisplayName.ToString());

	// Mark the pickup as collected, broadcast the event, and destroy the pickup if necessary
	bCollected = true;
	OnUpgradeCollected.Broadcast(UpgradeDefinition);

	// If the pickup should be destroyed on collect, destroy it
	if (bDestroyOnCollect)
	{
		Destroy();
	}
}

bool ATDSUpgradePickup::TryGrantUpgrade(ATDSCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: entered"));

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: PlayerCharacter is null"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: PlayerCharacter valid"));

	if (!UpgradeDefinition)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: UpgradeDefinition is null"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: UpgradeDefinition valid: %s"), *UpgradeDefinition->GetName());

	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance());
	if (!GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: GameInstance cast failed"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: GameInstance valid"));

	UTDSRunData* RunData = GI->GetRunData();
	if (!RunData)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: RunData is null"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: RunData valid"));

	const bool bGranted = RunData->GrantUpgrade(UpgradeDefinition);
	UE_LOG(LogTemp, Warning, TEXT("TryGrantUpgrade: GrantUpgrade returned %s"), bGranted ? TEXT("true") : TEXT("false"));

	return bGranted;
} 

void ATDSUpgradePickup::SetUpgradeDefinition(UTDSUpgradeDefinition* InUpgradeDefinition)
{
	UpgradeDefinition = InUpgradeDefinition;
}