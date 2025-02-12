// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CopyKeyValue.generated.h"

UCLASS()
class RINGRIDER_API UBTS_CopyKeyValue : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CopyKeyValue();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;

public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector CopySourceKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector CopyTargetKey;
};
