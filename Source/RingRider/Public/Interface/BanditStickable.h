// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Rider/Bandit/BanditBand.h"
#include "BanditStickable.generated.h"

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
	virtual bool IsStickable() const = 0;
	virtual void SetStickable(bool _bStickable) = 0;

	virtual TArray<UBanditBand*> GetStickedBands() const = 0;
	virtual void AddStickedBand(UBanditBand* _StickedBand) = 0;
	virtual void RemoveStickedBand(UBanditBand* _StickedBand) = 0;

	virtual void OnBanditSticked(UBanditBand* _OtherBanditBand);
	virtual void OnBanditReleased(UBanditBand* _OtherBanditBand);
	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand);
	virtual void OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime);
	virtual void OnBanditPulledExit(UBanditBand* _OtherBanditBand);
};
