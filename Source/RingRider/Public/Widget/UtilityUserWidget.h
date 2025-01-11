// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UtilityUserWidget.generated.h"

/**
 * �E�B�W�F�b�g�֘A�̔ėp�I�ȕ֗��֐����܂Ƃ߂��N���X�i��ʂ̉𑜓x�ł̐��K���Ȃǁj
 */
UCLASS()
class RINGRIDER_API UUtilityUserWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;

	APlayerController* PlayerController;

	float GetViewportScale() const;
	FVector2D GetViewportSize() const;
	FVector2D GetNormalizedScreenPosition(const FVector2D& _ScreenPos) const;
	bool GetScreenFromWorld(const FVector& _WorldPos, FVector2D& _ScreenPos) const;
};
