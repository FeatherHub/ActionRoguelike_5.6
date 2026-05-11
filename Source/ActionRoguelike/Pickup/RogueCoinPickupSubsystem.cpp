#include "RogueCoinPickupSubsystem.h"

#include "ActionRoguelike.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "Player/RoguePlayerCharacter.h"

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
	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		float DistTo = FVector::Dist(PlayerLocation, CoinLocations[i]);
		if (DistTo < PickupRadius)
		{
			PickedCoinIndicies.Add(i);
		}
	}


	int32 TotalCoinCreditToGrant = 0.f;
	for (int i = PickedCoinIndicies.Num() - 1; i >= 0; --i)
	{
		int32 PickedCoinIndex = PickedCoinIndicies[i];
		
		TotalCoinCreditToGrant += CoinCredits[PickedCoinIndex];

		RemoveCoin(PickedCoinIndex);
	}
	
	UE_CLOG(TotalCoinCreditToGrant > 0, LogGame, Log, TEXT("Picked up Coin Total Credit: %d"), TotalCoinCreditToGrant)
	
	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		DrawDebugPoint(GetWorld(), CoinLocations[i], 8.f, FColor::White);
	}
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
	
	AddCoins(NewCoinLocations, NewCoinCredits);
}

void URogueCoinPickupSubsystem::AddCoins(const TArray<FVector>& NewCoinLocations, const TArray<int32>& NewCoinCredits)
{
	CoinLocations.Append(NewCoinLocations);
	CoinCredits.Append(NewCoinCredits);
}

void URogueCoinPickupSubsystem::RemoveCoin(int32 CoinIndex)
{
	CoinLocations.RemoveAt(CoinIndex);
	CoinCredits.RemoveAt(CoinIndex);
}
