#include "RogueActionSystemComponent.h"

#include "RogueActionBase.h"
#include "GameplayTagContainer.h"
#include "RogueAttributeSet.h"

URogueActionSystemComponent::URogueActionSystemComponent()
{
	bWantsInitializeComponent = true;
}

void URogueActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (TSubclassOf<URogueActionBase> ActionClass : DefaultGrantActions)
	{
		if (ActionClass)
		{
			URogueActionBase* NewAction = NewObject<URogueActionBase>(this, ActionClass);
			GrantAction(NewAction);
		}
	}
	
	if (!AttributeSet)
	{
		AttributeSet = NewObject<URogueAttributeSet>(this);
	}
	
	for (TFieldIterator<FProperty> PropertyIt(AttributeSet->GetClass()); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;
		
		FName AttributeTagName = FName("Attribute." + Property->GetName());
		FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(AttributeTagName);
		
		FRogueAttribute* Attribute = Property->ContainerPtrToValuePtr<FRogueAttribute>(AttributeSet);
		
		CachedAttributeMap.Add(AttributeTag, Attribute);
	}
}

void URogueActionSystemComponent::SetDefaultAttributeSet(TSubclassOf<URogueAttributeSet> AttributeSetClass)
{
	FObjectInitializer& ObjectInitializer = FObjectInitializer::Get();
	
	AttributeSet = Cast<URogueAttributeSet>(ObjectInitializer.CreateDefaultSubobject(this, TEXT("AttributeSet"), AttributeSetClass, AttributeSetClass));
}

void URogueActionSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AttributeSet->PostInitializeComponents();
}

void URogueActionSystemComponent::GrantAction(URogueActionBase* Action)
{
	GrantedActions.Add(Action);
}

void URogueActionSystemComponent::StartAction(FGameplayTag ActionName)
{
	for (URogueActionBase* Action : GrantedActions)
	{
		if (ActionName == Action->GetActionName())
		{
			if (Action->CanStart())
			{
				Action->StartAction();
			}
			return;
		}
	}
	
	UE_LOGFMT(LogTemp, Warning, "Failed to Start Action '{ActionName}'", ActionName.GetTagName());
}

void URogueActionSystemComponent::StopAction(FGameplayTag ActionName)
{
	for (URogueActionBase* Action : GrantedActions)
	{
		if (ActionName == Action->GetActionName())
		{
			if (Action->CanStop())
			{
				Action->StopAction();
			}
			return;
		}
	}
	
	UE_LOGFMT(LogTemp, Warning, "Failed to Stop Action '{ActionName}'", ActionName.GetTagName());
}

bool URogueActionSystemComponent::ApplyAttributeChange(FGameplayTag AttributeTag, float InValue, EAttributeChangeType ChangeType)
{
	FRogueAttribute* Attribute = GetAttribute(AttributeTag);
	if (!Attribute)
	{
		return false;
	}
	float OldValue = Attribute->GetValue();
	
	switch (ChangeType) {
	case BaseDelta:
		Attribute->Base += InValue;
		break;
	case ModifierDelta:
		Attribute->Modifier += InValue;
		break;
	case BaseOverride:
		Attribute->Base = InValue;
		break;
	case MultiplierDelta:
		Attribute->Multiplier += InValue;
		break;
	case MultiplierOverride:
		Attribute->Multiplier = InValue;
		break;
	default:
		check(false)
		break;
	}
	
	AttributeSet->PostApplyChange();

	float NewValue = Attribute->GetValue();
	
	bool bHasChanged = false;
	if (!FMath::IsNearlyEqual(NewValue, OldValue))
	{
		bHasChanged = true;
		
		// Native C++ Listeners
		if (FOnAttributeChanged* NativeListener = OnAttributeChangedListeners.Find(AttributeTag))
		{
			NativeListener->Broadcast(NewValue, OldValue);
		}

		// Dynamic Blueprint Listeners
		if (TArray<FOnAttributeChanged_Dynamic>* DynamicListeners = OnAttributeChangedListeners_Dynamic.Find(AttributeTag))
		{
			for (int i = DynamicListeners->Num() - 1; i >= 0; --i)
			{
				FOnAttributeChanged_Dynamic& Listener = (*DynamicListeners)[i];
				
				bool bIsBound = Listener.ExecuteIfBound(NewValue, OldValue);
				if (!bIsBound)
				{
					DynamicListeners->RemoveAt(i);
					
					UE_LOG(LogTemp, Log, TEXT("Successfully removed unbound OnAttributeChanged_Dynamic for %s")
						, *AttributeTag.ToString());
				}
			}			
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("[%s]-[%s] New: %-6.1f, Old: %-6.1f Type: %s")
		, *GetFNameSafe(GetOuter()).ToString().Left(25), *AttributeTag.ToString(), NewValue, OldValue, *UEnum::GetValueAsString(ChangeType));

	return bHasChanged;
}

FOnAttributeChanged& URogueActionSystemComponent::GetOnAttributeChangedListener(FGameplayTag AttributeTag)
{
	return OnAttributeChangedListeners.FindOrAdd(AttributeTag);
}

FRogueAttribute* URogueActionSystemComponent::GetAttribute(FGameplayTag AttributeTag) const
{
	FRogueAttribute* const* FoundAttribute = CachedAttributeMap.Find(AttributeTag);
	
	if (FoundAttribute)
	{
		return *FoundAttribute;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attribute %s not found on %s's ActionSystemComponent")
		, *AttributeTag.ToString(), *GetNameSafe(GetOuter()));
	
	return nullptr;
}

float URogueActionSystemComponent::GetAttributeValue(FGameplayTag AttributeTag) const
{
	FRogueAttribute* Attribute = GetAttribute(AttributeTag);
	if (Attribute)
	{
		return Attribute->GetValue();
	}
	
	ensure(false);
	return 0.f;
}

void URogueActionSystemComponent::AddOnAttributeChangedListener_Dynamic(FGameplayTag AttributeTag, FOnAttributeChanged_Dynamic OnAttributeChanged)
{
	TArray<FOnAttributeChanged_Dynamic>& Listeners = OnAttributeChangedListeners_Dynamic.FindOrAdd(AttributeTag);
	Listeners.Add(OnAttributeChanged);
}

void URogueActionSystemComponent::RemoveOnAttributeChangedListener_Dynamic(FOnAttributeChanged_Dynamic ListenerToRemove)
{
	for (TPair<FGameplayTag, TArray<FOnAttributeChanged_Dynamic>>& ListenersEntry : OnAttributeChangedListeners_Dynamic)
	{
		if (ListenersEntry.Value.RemoveSingle(ListenerToRemove) > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully removed OnAttributeChanged_Dynamic for %s")
				, *ListenersEntry.Key.ToString())

			return;
		}
	}
}
