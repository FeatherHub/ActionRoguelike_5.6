#include "RogueCoinPickupSubsystem.h"

#include "ActionRoguelike.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "Components/AudioComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Core/RoguePickupSystemSetting.h"
#include "Player/RoguePlayerCharacter.h"
#include "ProfilingDebugging/CountersTrace.h"

TRACE_DECLARE_INT_COUNTER(TotalCoinCount, TEXT("TotalCoinCount"));

void URogueCoinPickupSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	TRACE_COUNTER_SET(TotalCoinCount, 0);
	
	Super::OnWorldBeginPlay(InWorld);
	
	UWorld* World = GetWorld();
	
	CoinISMComp = NewObject<UInstancedStaticMeshComponent>(World, NAME_None, RF_Transient);
	CoinISMComp->RegisterComponentWithWorld(World);
	CoinISMComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	CoinPickupAudioComp = NewObject<UAudioComponent>(World, NAME_None, RF_Transient);
	CoinPickupAudioComp->SetAutoActivate(false);
	CoinPickupAudioComp->bAllowSpatialization = false;
	CoinPickupAudioComp->RegisterComponentWithWorld(World);

	const URoguePickupSystemSetting* PickupSystemSetting = GetDefault<URoguePickupSystemSetting>();
	
	CoinPickupTriggerName = PickupSystemSetting->CoinPickupTriggerParameterName;
	
	PickupSystemSetting->CoinMeshSoftAsset.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(
		this, &ThisClass::OnCoinMeshLoadComplete));
	
	PickupSystemSetting->CoinPickupSoundSoftAsset.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(
		this, &ThisClass::OnCoinPickupSoundLoadComplete));
}

void URogueCoinPickupSubsystem::OnCoinMeshLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedAsset)
{
	CoinISMComp->SetStaticMesh(Cast<UStaticMesh>(LoadedAsset));
}

void URogueCoinPickupSubsystem::OnCoinPickupSoundLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedAsset)
{
	CoinPickupAudioComp->SetSound(Cast<USoundBase>(LoadedAsset));
}

void URogueCoinPickupSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector PlayerLocation = FVector::ZeroVector;
	float PickupRadius = 0.f;
	for (ARoguePlayerCharacter* PlayerCharacter : TActorRange<ARoguePlayerCharacter>(GetWorld()))
	{
		PlayerLocation = PlayerCharacter->GetActorLocation();
		PickupRadius = PlayerCharacter->GetPickupRadius();
	}


	TArray<int32> PickedCoinIndicies;
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(URogueCoinPickupSubsystem::Tick::DistanceCheck);
		for (int i = 0; i < CoinLocations.Num(); ++i)
		{
			float DistTo = FVector::Dist(PlayerLocation, CoinLocations[i]);
			if (DistTo < PickupRadius)
			{
				PickedCoinIndicies.Add(i);
			}
		}
	}


	int32 TotalCoinCreditToGrant = 0.f;

	{
		TRACE_CPUPROFILER_EVENT_SCOPE(URogueCoinPickupSubsystem::Tick::PickupCoins);
		
		for (int i = PickedCoinIndicies.Num() - 1; i >= 0; --i)
		{
			int32 PickedCoinIndex = PickedCoinIndicies[i];
			
			TotalCoinCreditToGrant += CoinCredits[PickedCoinIndex];

			RemoveCoin(PickedCoinIndex);
		}
	}

	
	if (TotalCoinCreditToGrant > 0)
	{
		if (!CoinPickupAudioComp->IsPlaying())
		{
			CoinPickupAudioComp->Play();
		}
		CoinPickupAudioComp->SetTriggerParameter(CoinPickupTriggerName);
	}
	
	UE_CLOG(TotalCoinCreditToGrant > 0, LogGame, Log, TEXT("Picked up Coin Total Credit: %d"), TotalCoinCreditToGrant)
}

void URogueCoinPickupSubsystem::SpawnCoins(int32 CoinCount, const FVector& Location, int32 Radius)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	
	TArray<FVector> NewCoinLocations;
	NewCoinLocations.Reserve(CoinCount);
	TArray<int32> NewCoinCredits;
	NewCoinCredits.Reserve(CoinCount);
	
	for (int i = 0; i < CoinCount; ++i)
	{
		FNavLocation NavLocation;
		NavSystem->GetRandomPointInNavigableRadius(Location, Radius, NavLocation);
		
		NewCoinLocations.Add(NavLocation.Location);
		NewCoinCredits.Add(10);
	}
	

	TArray<FTransform> NewCoinTransforms;
	NewCoinTransforms.Reserve(CoinCount);
	
	for (int i = 0; i < CoinCount; ++i)
	{
		FTransform NewCoinTransform = FTransform(NewCoinLocations[i] + FVector{0.f, 0.f, 50.f} ); 
		NewCoinTransforms.Add(NewCoinTransform);
	}

	TArray<FPrimitiveInstanceId> NewCoinInstanceIds = CoinISMComp->AddInstancesById(NewCoinTransforms, true, false);
	
	AddCoins(NewCoinLocations, NewCoinCredits, NewCoinInstanceIds);
}

void URogueCoinPickupSubsystem::AddCoins(const TArray<FVector>& NewCoinLocations, const TArray<int32>& NewCoinCredits, const TArray<FPrimitiveInstanceId>& NewCoinInstanceIds)
{
	CoinLocations.Append(NewCoinLocations);
	CoinCredits.Append(NewCoinCredits);
	CoinMeshInstanceIds.Append(NewCoinInstanceIds);
	
	TRACE_COUNTER_SET(TotalCoinCount, CoinLocations.Num());
}

void URogueCoinPickupSubsystem::RemoveCoin(int32 CoinIndex)
{
	CoinLocations.RemoveAt(CoinIndex);
	CoinCredits.RemoveAt(CoinIndex);
	
	CoinISMComp->RemoveInstanceById(CoinMeshInstanceIds[CoinIndex]);
	CoinMeshInstanceIds.RemoveAt(CoinIndex);
	
	TRACE_COUNTER_SET(TotalCoinCount, CoinLocations.Num());
}
