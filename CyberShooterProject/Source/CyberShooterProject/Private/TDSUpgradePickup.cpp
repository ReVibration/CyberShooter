// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSUpgradePickup.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TDSCharacter.h"
#include "Kismet/GameplayStatics.h"
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

	// Bind the overlap event to the OnOverlapBegin function. If the OverlapBox component is missing, log an error.
	if (OverlapBox)
	{
		OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ATDSUpgradePickup::OnOverlapBegin);
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

	// If the pickup has already been collected, do nothing
	if (bCollected)
	{
		return;
	}

	// If the overlapping actor is not a player character, do nothing
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(OtherActor);
	if (!PlayerCharacter)
	{
		return;
	}

	// If a collect sound is specified, play it at the pickup's location
	if (CollectSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation(), CollectSoundVolume);
	}

	// Try to grant the upgrade to the player character. If it fails (e.g. due to upgrade limits), do nothing
	if (!TryGrantUpgrade(PlayerCharacter))
	{
		return;
	}


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

	if (!PlayerCharacter)
	{
		return false;
	}

	if (!UpgradeDefinition)
	{
		return false;
	}

	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance());
	if (!GI)
	{
		return false;
	}

	UTDSRunData* RunData = GI->GetRunData();
	if (!RunData)
	{
		return false;
	}

	const bool bGranted = RunData->GrantUpgrade(UpgradeDefinition);

	return bGranted;
} 

void ATDSUpgradePickup::SetUpgradeDefinition(UTDSUpgradeDefinition* InUpgradeDefinition)
{
	UpgradeDefinition = InUpgradeDefinition;
}