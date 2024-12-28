// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/ProjectableUserWidget.h"
#include "BanditAimUserWidget.generated.h"


class UImage;


UCLASS()
class RINGRIDER_API UBanditAimUserWidget : public UProjectableUserWidget
{
	GENERATED_BODY()
	

public:
	UBanditAimUserWidget(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


public:
	void ShowAimMark();
	void MoveAimMark(const FVector& WorldPos);
	void HideAimMark();


private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* AimMark;


private:
	FDelegateHandle OnStartAimDelegateHandle;
	FDelegateHandle OnAimingDelegateHandle;
	FDelegateHandle OnEndAimDelegateHandle;
};
