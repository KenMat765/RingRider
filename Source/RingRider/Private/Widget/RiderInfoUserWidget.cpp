// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RiderInfoUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/RetainerBox.h"


void URiderInfoUserWidget::ShowSpeedText(float Speed)
{
	int SpeedInt = FMath::RoundToInt(Speed);
	FString SpeedStr = FString::FromInt(SpeedInt);
	SpeedText->SetText(FText::FromString(SpeedStr));
}

void URiderInfoUserWidget::ShowEnergyMeter(float EnergyRatio)
{
	UMaterialInstanceDynamic* EnergyMeter = EnergyRetainerBox->GetEffectMaterial();
	EnergyMeter->SetScalarParameterValue(FName("Value"), EnergyRatio);
}
