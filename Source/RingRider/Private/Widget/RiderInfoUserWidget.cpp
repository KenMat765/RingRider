// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RiderInfoUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/RetainerBox.h"
#include "Rider/Rider.h"



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

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (ARider* Rider = Cast<ARider>(PlayerController->GetPawn()))
	{
		// Speedをそのまま表示させると、変化が激しすぎてチカチカするので、0.05を掛けて感度を下げる
		auto OnSpeedChangeDelegate = [this](float NewSpeed, float MaxSpeed) { ShowSpeedText(NewSpeed * 0.05f); };
		Rider->AddOnSpeedChangeAction(OnSpeedChangeDelegate);

		auto OnEnergyChangeDelegate = [this](float NewEnergy, float MaxEnergy) { ShowEnergyMeter(NewEnergy / MaxEnergy); };
		Rider->AddOnEnergyChangeAction(OnEnergyChangeDelegate);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not get Rider"));

	ShowEnergyMeter(0);
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
	UMaterialInstanceDynamic* EnergyMeter = EnergyRetainerBox->GetEffectMaterial();
	EnergyMeter->SetScalarParameterValue(FName("Value"), EnergyRatio);
}
