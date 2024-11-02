// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameModeBattle.h"


AGameModeBattle::AGameModeBattle()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameModeBattle::BeginPlay()
{
	Time = TimeLimitPerWave;
	Wave = 1;
}


void AGameModeBattle::Tick(float DeltaTime)
{
	Time -= DeltaTime;
	TriggerOnTimeUpdateActions(Time, TimeLimitPerWave);

	if (Time <= 0)
	{
		// Go to next wave.
		Wave++;
		TriggerOnWaveChangeActions(Wave);
		Time = TimeLimitPerWave;
	}
}



// Game Timer //////////////////////////////////////////////////////////////////
void AGameModeBattle::TriggerOnTimeUpdateActions(float NewTime, float MaxTime) const
{
	if (OnTimeUpdateActions.IsBound())
	{
		OnTimeUpdateActions.Broadcast(NewTime, MaxTime);
	}
}

FDelegateHandle AGameModeBattle::AddOnTimeUpdateAction(TFunction<void(float, float)> NewFunc)
{
	auto NewAction = FTimeUpdateDelegate::FDelegate::CreateLambda(NewFunc);
	return OnTimeUpdateActions.Add(NewAction);
}

void AGameModeBattle::RemoveOnTimeUpdateAction(FDelegateHandle DelegateHandle)
{
	OnTimeUpdateActions.Remove(DelegateHandle);
}



// Wave ////////////////////////////////////////////////////////////////////////
void AGameModeBattle::TriggerOnWaveChangeActions(int NewWave) const
{
	if (OnWaveChangeActions.IsBound())
	{
		OnWaveChangeActions.Broadcast(NewWave);
	}
}

FDelegateHandle AGameModeBattle::AddOnWaveChangeAction(TFunction<void(int)> NewFunc)
{
	auto NewAction = FWaveChangeDelegate::FDelegate::CreateLambda(NewFunc);
	return OnWaveChangeActions.Add(NewAction);
}

void AGameModeBattle::RemoveOnWaveChangeAction(FDelegateHandle DelegateHandle)
{
	OnWaveChangeActions.Remove(DelegateHandle);
}

