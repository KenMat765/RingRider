// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RiderInfoUserWidget.generated.h"


class UTextBlock;
class URetainerBox;
class UMaterialInstanceDynamic;


UCLASS()
class RINGRIDER_API URiderInfoUserWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	URiderInfoUserWidget(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	

public:
	void ShowSpeedText(float Speed);
	void ShowEnergyMeter(float EnergyRatio);

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* SpeedText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	URetainerBox* EnergyRetainerBox;

	UMaterialInstanceDynamic* EnergyMeter;
};
