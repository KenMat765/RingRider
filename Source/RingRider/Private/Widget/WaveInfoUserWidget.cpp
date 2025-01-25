// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WaveInfoUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/RetainerBox.h"


void UWaveInfoUserWidget::ShowWaveText(int WaveNum)
{
	FString WaveStr = TEXT("WAVE ") + FString::FromInt(WaveNum);
	WaveText->SetText(FText::FromString(WaveStr));
}

void UWaveInfoUserWidget::ShowTimeText(float TimeInSec)
{
	int TimeInSec_Int = FMath::CeilToInt(TimeInSec);
	int Min = TimeInSec_Int / 60;
	int Sec = TimeInSec_Int % 60;
	FString MinStr = FString::FromInt(Min);
	FString SecStr = FString::FromInt(Sec);
	if (Sec < 10) SecStr = TEXT("0") + SecStr;
	FString WaveStr = MinStr + TEXT(":") + SecStr;
	TimeText->SetText(FText::FromString(WaveStr));
}

void UWaveInfoUserWidget::ShowTimeMeter(float TimeRatio)
{
	UMaterialInstanceDynamic* TimeMeter = TimeRetainerBox->GetEffectMaterial();
	TimeMeter->SetScalarParameterValue(FName("Value"), TimeRatio);
}

