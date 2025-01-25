// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveInfoUserWidget.generated.h"


UCLASS()
class RINGRIDER_API UWaveInfoUserWidget : public UUserWidget
{
	GENERATED_BODY()

	
public:
	void ShowWaveText(int WaveNum);
	void ShowTimeText(float TimeInSec);
	void ShowTimeMeter(float TimeRatio);


private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* WaveText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* TimeText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class URetainerBox* TimeRetainerBox;
};
