// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BanditAimUserWidget.generated.h"

class UImage;

UCLASS()
class RINGRIDER_API UBanditAimUserWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;


public:
	void ShowAimMark(bool _bLockOn = false);
	void MoveAimMark(const FVector& WorldPos);
	void HideAimMark();
	void ChangeMark(bool _bLockOn);
	bool IsShowing() const { return bIsShowing; }


private:
	bool bIsShowing = false;

	UImage* CurrentMark;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* Aim_NoLockOn;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* Aim_LockOn;
};
