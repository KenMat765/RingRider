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
	UPROPERTY(EditAnywhere)
	float XAxisHalfRange;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStickSlidedDelegate, float, XAxisValue);
	FStickSlidedDelegate OnStickSlided;

	float GetXAxisAmount() const { return XAxisAmount; };


protected:
	virtual void NativeConstruct() override;

	virtual void ProcessOnTouchStarted(const FVector2D& _NormTouchStartPos) override;
	virtual void ProcessOnTouching(const FVector2D& _NormTouchingPos, const FVector2D& _NormTouchingVel) override;
	virtual void ProcessOnTouchEnded(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel) override;


private:
	class UCanvasPanelSlot* StickHandleSlot;

	FVector2D NormTouchStartPos;
	FVector2D NormDefaultHandlePos;
	float XAxisAmount = 0.f;

	void MoveHandlePosition(FVector2D _NewNormHandlePos);
};
