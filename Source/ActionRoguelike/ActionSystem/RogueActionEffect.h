#pragma once

#include "CoreMinimal.h"
#include "RogueActionBase.h"
#include "RogueActionEffect.generated.h"

UCLASS()
class ACTIONROGUELIKE_API URogueActionEffect : public URogueActionBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	float EffectDuration = 0.f;

	FTimerHandle EffectTimerHandle;

public:
	virtual void StartAction_Implementation() override;
	virtual void StopAction_Implementation() override;
};
