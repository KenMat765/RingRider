// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RiderInfoUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ScaleBox.h"
#include "Components/RetainerBox.h"


void URiderInfoUserWidget::ShowSpeed(float Speed)
{
	// Speedをそのまま表示させると変化が激しすぎてチカチカするので、0.05を掛けて感度を下げる
	int SpeedInt = FMath::RoundToInt(Speed * 0.05f);
	FString SpeedStr = FString::FromInt(SpeedInt);
	SpeedText->SetText(FText::FromString(SpeedStr));

	if (MaxSpeed == MinSpeed)
		return;
	float SpeedRatio = (Speed - MinSpeed) / (MaxSpeed - MinSpeed);
	float IndicatorAngle = -100.f + SpeedRatio * 200.f;
	SB_SpeedIndicator->SetRenderTransformAngle(IndicatorAngle);
}

void URiderInfoUserWidget::ShowEnergyMeter(float EnergyRatio)
{
	UMaterialInstanceDynamic* EnergyMeter = EnergyRetainerBox->GetEffectMaterial();
	EnergyMeter->SetScalarParameterValue(FName("Value"), EnergyRatio);
}
