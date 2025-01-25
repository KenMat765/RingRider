// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PsmComponent.h"


FPsmInfo::FPsmInfo(float DeltaTime, EPsmCondition Condition)
	:DeltaTime(DeltaTime), Condition(Condition)
{
}


// Sets default values for this component's properties
UPsmComponent::UPsmComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPsmComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UPsmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (StateMap.Num() == 0)
	{
		return;
	}

	for (const auto& State : StateMap)
	{
		bool bIsOn = State.Value;
		if (bIsOn)
		{
			FPsmInfo PsmInfo(DeltaTime, EPsmCondition::STAY);
			(*State.Key)(PsmInfo);
		}
	}
}


void UPsmComponent::AddState(TPsmStateFunc& StateFunc)
{
	StateMap.Add(&StateFunc, false);
}

void UPsmComponent::RemoveState(TPsmStateFunc& StateFunc)
{
	StateMap.Remove(&StateFunc);
}

void UPsmComponent::ClearState()
{
	StateMap.Empty();
}


void UPsmComponent::TurnOnState(TPsmStateFunc& StateFunc)
{
	if (StateMap.Contains(&StateFunc))
	{
		bool* bIsOn = &StateMap[&StateFunc];
		if (!(*bIsOn))
		{
			FPsmInfo PsmInfo(0.f, EPsmCondition::ENTER);
			StateFunc(PsmInfo);	// ENTER process.
			*bIsOn = true;		// STAY process starts here.
		}
	}
}

void UPsmComponent::TurnOffState(TPsmStateFunc& StateFunc)
{
	if (StateMap.Contains(&StateFunc))
	{
		bool* bIsOn = &StateMap[&StateFunc];
		if (*bIsOn)
		{
			FPsmInfo PsmInfo(0.f, EPsmCondition::EXIT);
			*bIsOn = false;		// STAY process ends here.
			StateFunc(PsmInfo);	// EXIT process.
		}
	}
}

void UPsmComponent::TurnOnAll()
{
	for (auto& State : StateMap)
	{
		TurnOnState(*State.Key);
	}
}

void UPsmComponent::TurnOffAll()
{
	for (auto& State : StateMap)
	{
		TurnOffState(*State.Key);
	}
}

bool UPsmComponent::IsStateOn(TPsmStateFunc& StateFunc) const
{
	if (StateMap.Contains(&StateFunc))
	{
		bool bIsOn = StateMap[&StateFunc];
		return bIsOn;
	}
	return false;
}
