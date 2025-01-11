// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RightButtonUserWidget.generated.h"

UCLASS()
class RINGRIDER_API URightButtonUserWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;


public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBanditPressedDelegate, const FVector2D&, StartPosition);
	UPROPERTY(BlueprintAssignable)
	FBanditPressedDelegate OnBanditPressed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBanditSlidedDelegate, const FVector2D&, SlideVector);
	UPROPERTY(BlueprintAssignable)
	FBanditSlidedDelegate OnBanditSlided;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBanditReleasedDelegate, const FVector2D&, EndPosition);
	UPROPERTY(BlueprintAssignable)
	FBanditReleasedDelegate OnBanditReleased;
};
