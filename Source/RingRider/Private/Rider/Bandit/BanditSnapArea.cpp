// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditSnapArea.h"


UBanditSnapArea::UBanditSnapArea()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetGenerateOverlapEvents(false);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}


void UBanditSnapArea::BeginPlay()
{
	Super::BeginPlay();

	EnableSnap(bIsSnappable);
}
