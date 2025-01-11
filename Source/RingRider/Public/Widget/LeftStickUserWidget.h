// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TouchUserWidget.h"
#include "LeftStickUserWidget.generated.h"


UCLASS()
class RINGRIDER_API ULeftStickUserWidget : public UTouchUserWidget
{
	GENERATED_BODY()
	

public:
	bool IsTouching() const { return bIsTouching; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStickPressedDelegate, const FVector2D&, NormalizedStartPosition);
	UPROPERTY(BlueprintAssignable)
	FStickPressedDelegate OnStickPressed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStickSlidedDelegate, const FVector2D&, NormalizedSlideVector);
	UPROPERTY(BlueprintAssignable)
	FStickSlidedDelegate OnStickSlided;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStickReleasedDelegate, const FVector2D&, NormalizedEndPosition);
	UPROPERTY(BlueprintAssignable)
	FStickReleasedDelegate OnStickReleased;


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	// StickHandleの位置を制御するために必要
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UPanelWidget* SB_StickHandle;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* StickHandle;


private:
	bool bIsTouching = false;
	FVector2D TouchStartPos;
	uint32 TouchId;

	FVector2D DefaultHandlePos;

	UPROPERTY(EditAnywhere, Category = "LeftStick Properties")
	float XAxisHalfRange;

	bool MoveHandlePosision(FVector2D _NewHandlePos);
};
