// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BanditStickable.generated.h"

class UBanditBand;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBanditStickable : public UInterface
{
	GENERATED_BODY()
};

/**
 * BanditBandのくっつき対象が実装するインターフェース
 */
class RINGRIDER_API IBanditStickable
{
	GENERATED_BODY()

public:
	virtual void OnBanditSticked(UBanditBand* _OtherBanditBand, AActor* _OtherActor);
	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand, AActor* _OtherActor);
	virtual void OnBanditPulledStay(UBanditBand* _OtherBanditBand, AActor* _OtherActor, float _DeltaTime);
	virtual void OnBanditPulledExit(UBanditBand* _OtherBanditBand, AActor* _OtherActor);
};
