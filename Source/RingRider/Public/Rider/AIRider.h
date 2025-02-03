// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Rider/Rider.h"
#include "AIRider.generated.h"


UCLASS()
class RINGRIDER_API AAIRider : public ARider
{
	GENERATED_BODY()

public:
	AAIRider();

	
public:
	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BTAsset;

	UPROPERTY(EditDefaultsOnly)
	class UFallZoneDetector* FallZoneDetector;
};
