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
	bool IsTouching() const { return bIsTouching; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonPressedDelegate);
	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnButtonPressed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonSlidedDelegate, const FVector2D&, SlideVector);
	UPROPERTY(BlueprintAssignable)
	FButtonSlidedDelegate OnButtonSlided;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonReleasedDelegate);
	UPROPERTY(BlueprintAssignable)
	FButtonReleasedDelegate OnButtonReleased;


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;


private:
	bool bIsTouching = false;
	FVector2D TouchStartPos;
	uint32 TouchId;

	UPROPERTY(EditAnywhere)
	float MaxSlideRadius = 0.1f;
};
