// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditSnapArea.h"


UBanditSnapArea::UBanditSnapArea()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}


void UBanditSnapArea::BeginPlay()
{
	Super::BeginPlay();

	EnableSnap(bIsSnappable);
}
