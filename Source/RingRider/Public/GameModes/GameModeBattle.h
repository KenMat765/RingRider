// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RingRider/RingRiderGameModeBase.h"
#include "GameModeBattle.generated.h"


UCLASS()
class RINGRIDER_API AGameModeBattle : public ARingRiderGameModeBase
{
	GENERATED_BODY()
	

public:
	AGameModeBattle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;



	// Widgets /////////////////////////////////////////////////////////////////////
protected:
	class UWaveInfoUserWidget* WaveInfoWidget;



	// Game Timer //////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category = "Game Properties|Time")
	float Time;

	UPROPERTY(EditAnywhere, Category = "Game Properties|Time")
	float TimeLimitPerWave;

public:
	float GetTime() const { return Time; }
	float GetTimeLimitPerWave() const { return TimeLimitPerWave; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTimeUpdateDelegate, float, NewTime, float, MaxTime);
	FTimeUpdateDelegate OnTimeUpdate;



	// Wave ////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere, Category = "Game Properties|Wave")
	int Wave = 1;

public:
	static const int MaxWave = 3;
	int GetWave() const { return Wave; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaveChangedDelegate, int, NewWave);
	FWaveChangedDelegate OnWaveChanged;
};
