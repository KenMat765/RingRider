// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/UtilityUserWidget.h"
#include "TouchUserWidget.generated.h"

/**
 * �^�b�`�֘A�֗̕��֐����܂Ƃ߂��N���X
 */
UCLASS()
class RINGRIDER_API UTouchUserWidget : public UUtilityUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	bool GetTouchPosition(uint32 _TouchId, FVector2D& _TouchPos) const;
};
