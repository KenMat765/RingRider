// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/BanditStickable.h"

void IBanditStickable::OnBanditSticked(UBanditBand* _OtherBanditBand)
{
	if(!GetStickedBands().Contains(_OtherBanditBand))
		AddStickedBand(_OtherBanditBand);
}

void IBanditStickable::OnBanditReleased(UBanditBand* _OtherBanditBand)
{
	if(GetStickedBands().Contains(_OtherBanditBand))
		RemoveStickedBand(_OtherBanditBand);
}

void IBanditStickable::OnBanditPulledEnter(UBanditBand* _OtherBanditBand) {}
void IBanditStickable::OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime) {}
void IBanditStickable::OnBanditPulledExit(UBanditBand* _OtherBanditBand) {}
