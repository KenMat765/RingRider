// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectableUserWidget.generated.h"

/**
 * ���[���h���W���X�N���[�����W�ɕϊ�����@�\�����N���X
 * �X�N���[���ɓ��e����@�\���~����Widget�i�Ə��Ȃǁj�͂��̃N���X���p������
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
