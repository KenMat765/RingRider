// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeBattle.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FTimeUpdateDelegate, float, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWaveChangeDelegate, int)


UCLASS()
class RINGRIDER_API AGameModeBattle : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	AGameModeBattle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;



	// Game Timer //////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category = "Game Properties|Time")
	float Time;

	UPROPERTY(EditAnywhere, Category = "Game Properties|Time")
	float TimeLimitPerWave;

public:
	float GetTime() const { return Time; }
	float GetTimeLimitPerWave() const { return TimeLimitPerWave; }

private:
	FTimeUpdateDelegate OnTimeUpdateActions;
	void TriggerOnTimeUpdateActions(float NewTime, float MaxTime) const;

public:
	FDelegateHandle AddOnTimeUpdateAction(TFunction<void(float, float)> NewFunc);
	void RemoveOnTimeUpdateAction(FDelegateHandle DelegateHandle);



	// Wave ////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere, Category = "Game Properties|Wave")
	int Wave = 1;

public:
	static const int MaxWave = 3;
	int GetWave() const { return Wave; }

private:
	FWaveChangeDelegate OnWaveChangeActions;
	void TriggerOnWaveChangeActions(int NewWave) const;

public:
	FDelegateHandle AddOnWaveChangeAction(TFunction<void(int)> NewFunc);
	void RemoveOnWaveChangeAction(FDelegateHandle DelegateHandle);
};
