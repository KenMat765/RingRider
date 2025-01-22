// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/BanditStickable.h"

void IBanditStickable::OnBanditSticked(UBanditBand* _OtherBanditBand, AActor* _OtherActor) {}
void IBanditStickable::OnBanditPulledEnter(UBanditBand* _OtherBanditBand, AActor* _OtherActor) {}
void IBanditStickable::OnBanditPulledStay(UBanditBand* _OtherBanditBand, AActor* _OtherActor, float _DeltaTime) {}
void IBanditStickable::OnBanditPulledExit(UBanditBand* _OtherBanditBand, AActor* _OtherActor) {}
