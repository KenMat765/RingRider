// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveInfoUserWidget.generated.h"


class UTextBlock;
class URetainerBox;


UCLASS()
class RINGRIDER_API UWaveInfoUserWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UWaveInfoUserWidget(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
public:
	void ShowWaveText(int WaveNum);
	void ShowTimeText(float TimeInSec);
	void ShowTimeMeter(float TimeRatio);

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* WaveText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TimeText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	URetainerBox* TimeRetainerBox;

private:
	FDelegateHandle OnTimeUpdateDelegateHandle;
	void OnTimeUpdate(float NewTime, float MaxTime);

	FDelegateHandle OnWaveChangeDelegateHandle;
	void OnWaveChange(int NewWave);
};
