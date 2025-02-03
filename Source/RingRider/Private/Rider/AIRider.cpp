// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AIRider.h"
#include "Components/FallZoneDetector.h"

AAIRider::AAIRider()
{
	FallZoneDetector = CreateDefaultSubobject<UFallZoneDetector>(TEXT("Fall Zone Detector"));
	FallZoneDetector->SetupAttachment(RootComponent);
	FallZoneDetector->SetComponentTickInterval(0.1f);
	FallZoneDetector->CheckPointNum = 6;
	FallZoneDetector->CheckPointRadius = 8000.f;
	FallZoneDetector->CheckPointRayDistance = 500.f;
	FallZoneDetector->CheckObjectType = ECollisionChannel::ECC_GameTraceChannel1;
}
