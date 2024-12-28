// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectableUserWidget.generated.h"

/**
 * ワールド座標をスクリーン座標に変換する機能を持つクラス
 * スクリーンに投影する機能が欲しいWidget（照準など）はこのクラスを継承する
 */

class ARider;

UCLASS()
class RINGRIDER_API UProjectableUserWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UProjectableUserWidget(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


protected:
	APlayerController* PlayerController;


protected:
	bool GetScreenPos(const FVector& WorldPos, FVector2D& ScreenPos);
};
