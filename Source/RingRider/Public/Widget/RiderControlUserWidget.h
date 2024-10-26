// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RiderControlUserWidget.generated.h"


UCLASS()
class RINGRIDER_API URiderControlUserWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	URiderControlUserWidget(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
