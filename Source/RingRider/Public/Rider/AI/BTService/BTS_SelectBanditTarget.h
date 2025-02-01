// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_SelectBanditTarget.generated.h"


UCLASS()
class RINGRIDER_API UBTS_SelectBanditTarget : public UBTService
{
	GENERATED_BODY()


public:
	UBTS_SelectBanditTarget();

protected:
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;


public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector HostileRiderKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector FriendlyRiderKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector DashPoleKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector BanditTargetKey;
};
