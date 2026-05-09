#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RogueAttributeSet.generated.h"

class APawn;
class URogueActionSystemComponent;

USTRUCT()
struct FRogueAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Base = 0.f;
	
	UPROPERTY(VisibleAnywhere)
	float Modifier = 0.f;

	UPROPERTY(VisibleAnywhere)
	float Multiplier = 1.f;
	
	float GetValue() const
	{
		return (Base + Modifier) * Multiplier;
	}
};


UCLASS(EditInlineNew)
class ACTIONROGUELIKE_API URogueAttributeSet : public UObject
{
	GENERATED_BODY()
	
protected:
	URogueActionSystemComponent* GetOwningComponent() const;
	
public:
	virtual void PostInitializeComponents() {}
	virtual void PostApplyChange() {}
};


UCLASS()
class ACTIONROGUELIKE_API URogueHealthAttributeSet : public URogueAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=Health)
	FRogueAttribute Health;

	UPROPERTY(EditAnywhere, Category=Health)
	FRogueAttribute HealthMax;
	
public:
	URogueHealthAttributeSet();
	virtual void PostApplyChange() override;
};

UCLASS()
class URogueCharacterAttributeSet : public URogueHealthAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=MoveSpeed)
	FRogueAttribute MoveSpeed;
	
public:
	virtual void PostInitializeComponents() override;
	void ApplyMoveSpeed();
	
protected:
	ACharacter* GetOwningCharacter() const;
	virtual void PostApplyChange() override;
};

UCLASS()
class URoguePlayerAttributeSet : public URogueCharacterAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=Rage)
	FRogueAttribute RageAmount;
	
public:
	URoguePlayerAttributeSet();
};

UCLASS()
class URogueMonsterAttributeSet : public URogueCharacterAttributeSet
{
	GENERATED_BODY()

public:
	URogueMonsterAttributeSet();
};