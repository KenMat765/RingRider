// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RiderPlayerController.generated.h"


UCLASS()
class RINGRIDER_API ARiderPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (
		ToolTip="Recognize touch movements faster than this value as swipe"))
	float SwipeSpeedThresh = 0.5f;

	UPROPERTY(EditAnywhere, Category = "BanditBand")
	float MaxBanditShootDeg = 30.f;

	UPROPERTY(EditAnywhere, Category = "BanditBand", meta = (ClampMin="1.0", ClampMax="10.0", UIMin="1.0", UIMax="10.0"))
	float AimSensitivity = 2.f;

	UPROPERTY(EditAnywhere, Category = "BanditBand", meta = (ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0",
		ToolTip="Reducing this value limits the vertical movement range of the reticle on the screen. It is used to adjust so that the reticle does not go off-screen."))
	float YAttenuation = 0.5f;

	UPROPERTY(EditAnywhere, Category = "BanditBand", meta = (
		ToolTip="Obtains bonus when cut band below this length"))
	float GreatCutLength = 600.f;

	UPROPERTY(EditAnywhere, Category = "BanditBand", meta = (
		ToolTip="Obtains large bonus when cut band below this length"))
	float PerfectCutLength = 400.f;


protected:
	virtual void BeginPlay() override;

	class ARider* Rider;
	class UBanditBand* BanditBand;

	// === プレイヤーの操作に関与するウィジェット === //
	class ULeftStickUserWidget* LeftStickWidget;
	class URightButtonUserWidget* RightButtonWidget;
	class UBanditAimUserWidget* BanditAimWidget;

	UFUNCTION()
	virtual void OnTouchEnter(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
	virtual void OnTouchExit(ETouchIndex::Type FingerIndex, FVector Location);

	enum class ESwipeDirection {UP, DOWN, LEFT, RIGHT};
	virtual void OnSwipe(ESwipeDirection _SwipeDirection);

	UFUNCTION()
	virtual void OnLeftStickSlided(float _XAxisValue);

	UFUNCTION()
	virtual void OnLeftStickExit(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel);

	UFUNCTION()
	virtual void OnRightButtonEnter(const FVector2D& _NormTouchStartPos);

	UFUNCTION()
	virtual void OnRightButtonSlided(const FVector2D& _NormSlideVector);

	UFUNCTION()
	virtual void OnRightButtonExit(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel);


private:
	// <TouchId, (PrevPos, PrevVel))>
	using FPosAndVel = TTuple<FVector2D, FVector2D>;
	TMap<uint32, FPosAndVel> Touches;

	FVector BanditAimTarget;

	// BanditBandの照準を特定の対象にスナップさせるために必要
	const ECollisionChannel BanditSnapChannel = ECollisionChannel::ECC_GameTraceChannel2;
	bool CheckBanditSnap(const FVector& _AimTarget, FVector& _SnapPos);
};
