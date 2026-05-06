#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "RogueActionBase.generated.h"

class URogueActionSystemComponent;
class ACharacter;

UCLASS(Abstract, Blueprintable)
class ACTIONROGUELIKE_API URogueActionBase : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Action)
	FGameplayTag ActionName;
	
	UPROPERTY(EditDefaultsOnly, Category=Action)
	float CooldownTime = 0.f;
	
	UPROPERTY(Transient)
	float CooldownEndTime = 0.f;
	
	UPROPERTY(Transient)
	bool bIsRunning = false;
	
protected:
	UFUNCTION(BlueprintCallable)
	URogueActionSystemComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintCallable)
	ACharacter* GetOwningCharacter() const;
	
public:
	UFUNCTION(BlueprintNativeEvent, Category=Action)
	void StartAction();
	
	UFUNCTION(BlueprintNativeEvent, Category=Action)
	void StopAction();
	
	bool CanStart() const;
	bool CanStop() const;
	bool IsRunning() const { return bIsRunning; }
	float GetCooldownRemaining() const;
	FGameplayTag GetActionName() const { return ActionName; }
};
