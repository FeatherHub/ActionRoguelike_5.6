#include "RogueActionBase.h"

#include "RogueActionSystemComponent.h"
#include "GameFramework/Character.h"

void URogueActionBase::StartAction_Implementation()
{
	bIsRunning = true;
	
	float CurrentTime = GetWorld()->TimeSeconds;
	CooldownEndTime = CurrentTime + CooldownTime;
	
	UE_LOGFMT(LogTemp, Log, "Start Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), CurrentTime);
}

void URogueActionBase::StopAction_Implementation()
{
	bIsRunning = false;
	
	float GameTime = GetWorld()->TimeSeconds;
	UE_LOGFMT(LogTemp, Log, "Stop Action '{ActionName}' at {GameTime}", ActionName.GetTagName(), GameTime);
}

bool URogueActionBase::CanStart() const
{
	if (IsRunning())
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot Start Action {ActionName}. Because it is in running", ActionName.GetTagName());
		return false;
	}
	
	float CooldownRemaining = GetCooldownRemaining();
	if (CooldownRemaining > 0.f)
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot Start Action {ActionName}. Because Cooldown remains: {CooldownRemaining}"
			, ActionName.GetTagName(), CooldownRemaining);
		return false;
	}

	return true;
}

bool URogueActionBase::CanStop() const
{
	if (!IsRunning())
	{
		UE_LOGFMT(LogTemp, Error, "Cannot Stop Action {ActionName}. Becuase it is not in running.", ActionName.GetTagName());
		return false;	
	}
	
	return true;
}


float URogueActionBase::GetCooldownRemaining() const
{
	return FMath::Max(0.f, CooldownEndTime - GetWorld()->TimeSeconds);
}

URogueActionSystemComponent* URogueActionBase::GetOwningComponent() const
{
	return Cast<URogueActionSystemComponent>(GetOuter());
}

ACharacter* URogueActionBase::GetOwningCharacter() const
{
	return Cast<ACharacter>(GetOuter()->GetOuter());
}
