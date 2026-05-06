#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueActionSystemComponent.generated.h"


class URogueActionBase;
class URogueActionSystemComponent;
struct FGameplayTag;

USTRUCT(Blueprintable)
struct FAttributeSet
{
	GENERATED_BODY()
	
	FAttributeSet()
	{
		MaxHealth = 100.f;
		Health = MaxHealth;
	}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, OldHealth);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API URogueActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category=Attribute)
	FOnHealthChanged OnHealthChanged;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Attribute)
	FAttributeSet AttributeSet;
	
	UPROPERTY(EditDefaultsOnly, Category=Action)
	TArray<TSubclassOf<URogueActionBase>> DefaultGrantActions;

	UPROPERTY(EditAnywhere, Category=Action)
	TArray<TObjectPtr<URogueActionBase>> GrantedActions;

public:
	URogueActionSystemComponent();
	virtual void InitializeComponent() override;
	
	void StartAction(FGameplayTag ActionName);
	void StopAction(FGameplayTag ActionName);
	
	void GrantAction(URogueActionBase* Action);
	
	bool ApplyHealthChange(float InHealthDelta);
	float GetCurrentHealth() { return AttributeSet.Health; }
	float GetMaxHealth() { return AttributeSet.MaxHealth; }
	bool IsHealthFull() { return FMath::IsNearlyEqual(AttributeSet.Health, AttributeSet.MaxHealth); }
};
