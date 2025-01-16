// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RiderPlayerController.generated.h"


UCLASS()
class RINGRIDER_API ARiderPlayerController : public APlayerController
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

	// === プレイヤーの操作に関与するウィジェット === //
	class ULeftStickUserWidget* LeftStickWidget;
	class URightButtonUserWidget* RightButtonWidget;


private:
	UFUNCTION()
	void OnLeftStickSlided(float _XAxisValue);

	UFUNCTION()
	void OnLeftStickReleased();

	UFUNCTION()
	void OnRightButtonPressed();

	UFUNCTION()
	void OnRightButtonSlided(const FVector2D& _NormSlideVector);

	UFUNCTION()
	void OnRightButtonReleased();

	class ARider* Rider;
	class UBanditBand* BanditBand;

	UPROPERTY(EditAnywhere, Category = "BanditBand")
	float MaxBanditShootDeg = 30.f;

	UPROPERTY(EditAnywhere, Category = "BanditBand", meta = (ClampMin="1.0", ClampMax="10.0", UIMin="1.0", UIMax="10.0"))
	float AimSensitivity = 2.f;

	UPROPERTY(EditAnywhere, Category = "BanditBand", meta = (ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0",
		ToolTip="Reducing this value limits the vertical movement range of the reticle on the screen. It is used to adjust so that the reticle does not go off-screen."))
	float YAttenuation = 0.5f;
};
