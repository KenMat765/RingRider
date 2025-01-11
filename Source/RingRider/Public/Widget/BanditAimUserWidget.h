// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/UtilityUserWidget.h"
#include "BanditAimUserWidget.generated.h"


class UImage;


UCLASS()
class RINGRIDER_API UBanditAimUserWidget : public UUtilityUserWidget
{
	GENERATED_BODY()


public:
	void ShowAimMark();
	void MoveAimMark(const FVector& WorldPos);
	void HideAimMark();


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* AimMark;

	FDelegateHandle OnStartAimDelegateHandle;
	FDelegateHandle OnAimingDelegateHandle;
	FDelegateHandle OnEndAimDelegateHandle;
};
