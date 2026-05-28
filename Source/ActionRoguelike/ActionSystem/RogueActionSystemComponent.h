#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "RogueActionSystemComponent.generated.h"


struct FRogueAttribute;
class URogueAttributeSet;
class URogueActionBase;
class URogueActionSystemComponent;
struct FGameplayTag;

UENUM()
enum EAttributeChangeType
{
	BaseDelta,
	ModifierDelta,
	BaseOverride,
	MultiplierDelta,
	MultiplierOverride,
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, float /*NewValue*/, float /*OldValue*/);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAttributeChanged_Dynamic, float, NewValue, float, OldValue);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API URogueActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=Action)
	FGameplayTagContainer ActiveTags;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	TArray<TSubclassOf<URogueActionBase>> DefaultGrantActions;

	UPROPERTY(EditAnywhere, Category=Action)
	TArray<TObjectPtr<URogueActionBase>> GrantedActions;

	UPROPERTY(EditAnywhere, Instanced, Category=Attribute)
	TObjectPtr<URogueAttributeSet> AttributeSet;

	TMap<FGameplayTag, FRogueAttribute*> CachedAttributeMap;
	
	TMap<FGameplayTag, FOnAttributeChanged> OnAttributeChangedListeners;

	TMap<FGameplayTag, TArray<FOnAttributeChanged_Dynamic>> OnAttributeChangedListeners_Dynamic;

public:
	URogueActionSystemComponent();
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	
	void StartAction(FGameplayTag ActionName);
	void StopAction(FGameplayTag ActionName);
	
	UFUNCTION(BlueprintCallable)
	void GrantAction(TSubclassOf<URogueActionBase> ActionClass);

	void RemoveAction(URogueActionBase* Action);
	
	void SetDefaultAttributeSet(TSubclassOf<URogueAttributeSet> AttributeSetClass);
	FRogueAttribute* GetAttribute(FGameplayTag AttributeTag) const;
	FOnAttributeChanged& GetOnAttributeChangedListener(FGameplayTag AttributeTag);

	UFUNCTION(BlueprintCallable)
	float GetAttributeValue(FGameplayTag AttributeTag) const;
	
	UFUNCTION(BlueprintCallable)
	bool ApplyAttributeChange(FGameplayTag AttributeTag, float InValue, EAttributeChangeType ChangeType);
	
	UFUNCTION(BlueprintCallable, DisplayName="Add Attribute Changed Listener", meta=(Keywords="Event, Delegate"))
	void AddOnAttributeChangedListener_Dynamic(FGameplayTag AttributeTag, FOnAttributeChanged_Dynamic OnAttributeChanged);
	
	UFUNCTION(BlueprintCallable, DisplayName="Remove Attribute Changed Listener", meta=(Keywords="Event, Delegate"))
	void RemoveOnAttributeChangedListener_Dynamic(FOnAttributeChanged_Dynamic ListenerToRemove);
};
