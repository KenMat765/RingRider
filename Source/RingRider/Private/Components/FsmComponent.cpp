// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FsmComponent.h"


FFsmInfo::FFsmInfo(float DeltaTime, EFsmCondition Condition)
	:DeltaTime(DeltaTime), Condition(Condition) {}


UFsmComponent::UFsmComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFsmComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFsmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState)
	{
		FFsmInfo FsmInfo(DeltaTime, EFsmCondition::STAY);
		(*CurrentState)(FsmInfo);
	}
}


void UFsmComponent::AddState(TFsmStateFunc& _FsmStateFunc)
{
	if (!RegisteredStates.Contains(&_FsmStateFunc))
		RegisteredStates.Add(&_FsmStateFunc);
}

void UFsmComponent::RemoveState(TFsmStateFunc& _FsmStateFunc)
{
	if (RegisteredStates.Contains(&_FsmStateFunc))
		RegisteredStates.Remove(&_FsmStateFunc);
}

bool UFsmComponent::SwitchState(TFsmStateFunc* _FsmStateFunc)
{
	if (_FsmStateFunc && !RegisteredStates.Contains(_FsmStateFunc))
		return false;
	if (CurrentState == _FsmStateFunc)
		return true;
	else
	{
		if (CurrentState)
		{
			FFsmInfo OldFsmInfo(0.f, EFsmCondition::EXIT);
			(*CurrentState)(OldFsmInfo);
		}

		CurrentState = _FsmStateFunc;

		if (CurrentState)
		{
			FFsmInfo NewFsmInfo(0.f, EFsmCondition::ENTER);
			(*CurrentState)(NewFsmInfo);
		}

		return true;
	}
}
