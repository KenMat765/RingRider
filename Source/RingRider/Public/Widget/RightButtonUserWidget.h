// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TouchUserWidget.h"
#include "RightButtonUserWidget.generated.h"

UCLASS()
class RINGRIDER_API URightButtonUserWidget : public UTouchUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere)
	float MaxSlideRadius = 0.1f;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonSlidedDelegate, const FVector2D&, NormSlideVector);
	FButtonSlidedDelegate OnButtonSlided;


protected:
	virtual void ProcessOnTouchStarted(const FVector2D& _NormTouchStartPos) override;
	virtual void ProcessOnTouching(const FVector2D& _NormTouchingPos, const FVector2D& _NormTouchingVel) override;


private:
	FVector2D NormTouchStartPos;
};
