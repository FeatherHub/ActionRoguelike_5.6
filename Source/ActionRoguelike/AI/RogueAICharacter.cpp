#include "RogueAICharacter.h"

#include "ActionSystem/RogueActionSystemComponent.h"
#include "ActionSystem/RogueAttributeSet.h"
#include "Core/RogueGameplayTag.h"


ARogueAICharacter::ARogueAICharacter()
{
	ActionSystemComp = CreateDefaultSubobject<URogueActionSystemComponent>(TEXT("ActionSystemComp"));
	ActionSystemComp->SetDefaultAttributeSet(URogueMonsterAttributeSet::StaticClass());
}

float ARogueAICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ActionSystemComp->ApplyAttributeChange(RogueGameplayTag::Attribute_Health, -ActualDamageAmount, BaseDelta);

	return ActualDamageAmount;
}
