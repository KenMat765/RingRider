// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameModeBattle.h"


AGameModeBattle::AGameModeBattle()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameModeBattle::BeginPlay()
{
	Time = TimeLimitPerWave;
}


void AGameModeBattle::Tick(float DeltaTime)
{
	Time -= DeltaTime;
	TriggerOnTimeUpdateActions(Time, TimeLimitPerWave);

	if (Time <= 0)
	{
		// Next Wave ...
		UE_LOG(LogTemp, Log, TEXT("Reached time limit!!"));
	}
}



// Game Timer //////////////////////////////////////////////////////////////////
void AGameModeBattle::TriggerOnTimeUpdateActions(float NewTime, float MaxTime)
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

