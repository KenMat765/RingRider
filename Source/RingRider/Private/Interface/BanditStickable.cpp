// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/BanditStickable.h"
#include "Rider/Bandit/BanditBand.h"

void IBanditStickable::OnBanditSticked(UBanditBand* _OtherBanditBand) {}
void IBanditStickable::OnBanditPulledEnter(UBanditBand* _OtherBanditBand) {}
void IBanditStickable::OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime) {}
void IBanditStickable::OnBanditPulledExit(UBanditBand* _OtherBanditBand) {}
