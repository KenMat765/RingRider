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
	float MaxBanditShootDeg = 60.f;
};
