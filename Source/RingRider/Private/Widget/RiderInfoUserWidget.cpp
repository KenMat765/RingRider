// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RiderInfoUserWidget.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"



URiderInfoUserWidget::URiderInfoUserWidget(const FObjectInitializer& ObjectInitializer):
	UUserWidget(ObjectInitializer)
{
}



void URiderInfoUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}


void URiderInfoUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}


void URiderInfoUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
}



void URiderInfoUserWidget::ShowSpeedText(float Speed)
{
	int SpeedInt = FMath::RoundToInt(Speed);
	FString SpeedStr = FString::FromInt(SpeedInt);
	SpeedText->SetText(FText::FromString(SpeedStr));
}


void URiderInfoUserWidget::ShowEnergyMeter(float EnergyRatio)
{
}
