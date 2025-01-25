// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameModeBattle.h"
#include "Widget/WaveInfoUserWidget.h"


AGameModeBattle::AGameModeBattle()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameModeBattle::BeginPlay()
{
	Super::BeginPlay();

	Time = TimeLimitPerWave;
	Wave = 1;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	UClass* WaveInfoWidgetClass = LoadClass<UWaveInfoUserWidget>(nullptr, TEXT("/Game/UI/WB_WaveInfo.WB_WaveInfo_C"));
	ensureMsgf(WaveInfoWidgetClass, TEXT("Could not load WB_WaveInfo"));
	WaveInfoWidget = CreateWidget<UWaveInfoUserWidget>(PlayerController, WaveInfoWidgetClass);
	ensureMsgf(WaveInfoWidget, TEXT("Could not create WaveInfoWidget"));
	WaveInfoWidget->AddToViewport();
	WaveInfoWidget->ShowWaveText(1);
}


void AGameModeBattle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Time -= DeltaTime;
	WaveInfoWidget->ShowTimeText(Time);
	WaveInfoWidget->ShowTimeMeter(Time / TimeLimitPerWave);
	if (OnTimeUpdate.IsBound())
		OnTimeUpdate.Broadcast(Time, TimeLimitPerWave);

	if (Time <= 0)
	{
		// Go to next wave.
		Wave++;
		Time = TimeLimitPerWave;
		WaveInfoWidget->ShowWaveText(Wave);
		if (OnWaveChanged.IsBound())
			OnWaveChanged.Broadcast(Wave);
	}
}

