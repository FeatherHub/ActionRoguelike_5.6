#include "RogueActionEffect.h"

#include "RogueActionSystemComponent.h"

void URogueActionEffect::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	if (EffectDuration > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(EffectTimerHandle, this, &ThisClass::StopAction, EffectDuration);
	}
}

void URogueActionEffect::StopAction_Implementation()
{
	Super::StopAction_Implementation();

	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	
	GetOwningComponent()->RemoveAction(this);
}
