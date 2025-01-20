// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/UtilityUserWidget.h"
#include "BanditAimUserWidget.generated.h"


UCLASS()
class RINGRIDER_API UBanditAimUserWidget : public UUtilityUserWidget
{
	GENERATED_BODY()


public:
	void ShowAimMark();
	void MoveAimMark(const FVector& WorldPos);
	void HideAimMark();


private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* AimMark;
};
