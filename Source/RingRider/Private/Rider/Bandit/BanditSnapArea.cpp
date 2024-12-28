// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditSnapArea.h"
#include "Components/SphereComponent.h"


UBanditSnapArea::UBanditSnapArea()
{
	PrimaryComponentTick.bCanEverTick = false;

	SnapArea = CreateDefaultSubobject<USphereComponent>(TEXT("Snap Area"));
	SnapArea->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
}


void UBanditSnapArea::BeginPlay()
{
	Super::BeginPlay();
}
