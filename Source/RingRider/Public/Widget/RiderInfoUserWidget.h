// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RiderInfoUserWidget.generated.h"


UCLASS()
class RINGRIDER_API URiderInfoUserWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void ShowSpeed(float Speed);
	void ShowEnergyMeter(float EnergyRatio);

	void SetMinMaxSpeed(float _MinSpeed, float _MaxSpeed)
	{
		MinSpeed = _MinSpeed;
		MaxSpeed = _MaxSpeed;
	}


private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* SpeedText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UScaleBox* SB_SpeedIndicator;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class URetainerBox* EnergyRetainerBox;

	float MinSpeed;
	float MaxSpeed;
};
