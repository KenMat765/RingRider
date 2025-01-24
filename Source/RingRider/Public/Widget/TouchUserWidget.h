// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TouchUserWidget.generated.h"


UENUM()
enum class ESwipeDirection : uint8
{
	UP, DOWN, LEFT, RIGHT
};


/**
 * タッチ関連の便利関数をまとめたクラス
 */
UCLASS()
class RINGRIDER_API UTouchUserWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	bool IsTouching() const { return bIsTouching; }

	UPROPERTY(EditAnywhere, meta = (ToolTip="Recognize touch movements faster than this value as swipe"))
	float SwipeSpeedThresh = 0.5f;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTouchEnterDelegate,
		const FVector2D&, NormTouchStartPos);
	FTouchEnterDelegate OnTouchEnter;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTouchingDelegate,
		const FVector2D&, NormTouchingPos,
		const FVector2D&, NormTouchVelocity);
	FTouchingDelegate OnTouching;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTouchExitDelegate,
		const FVector2D&, NormTouchLatestPos,
		const FVector2D&, NormTouchLatestVel);
	FTouchExitDelegate OnTouchExit;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSwipeDelegate,
		ESwipeDirection, SwipeDirection);
	FSwipeDelegate OnSwipe;


protected:
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	// NativeOnTouchMoved <- これは指が動いてないと呼ばれないため、タッチ中の処理はNativeTickで行う
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	// 継承先のクラスが処理を追加する用
	virtual void ProcessOnTouchStarted(const FVector2D& _NormTouchStartPos);
	virtual void ProcessOnTouching(const FVector2D& _NormTouchingPos, const FVector2D& _NormTouchingVel);
	virtual void ProcessOnTouchEnded(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel);

	bool GetTouchPosition(uint32 _TouchId, FVector2D& _TouchPos) const;

	bool bIsTouching = false;
	uint32 TouchId;


private:
	FVector2D NormTouchPrevPos;
	FVector2D NormTouchPrevVel;
};
