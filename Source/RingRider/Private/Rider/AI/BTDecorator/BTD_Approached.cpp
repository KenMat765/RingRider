// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTDecorator/BTD_Approached.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_Approached::UBTD_Approached()
{
	NodeName = "Approached to";
	
	AcceptableRadius = 100.f;

	ApproachOriginKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTD_Approached, ApproachTargetKey), AActor::StaticClass());
	ApproachOriginKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTD_Approached, ApproachOriginKey));

	ApproachTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTD_Approached, ApproachTargetKey), AActor::StaticClass());
	ApproachTargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTD_Approached, ApproachTargetKey));

	bNotifyTick = true;
}

void UBTD_Approached::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		// ‚±‚ê‚ðŒÄ‚Î‚È‚¢‚Æ FBlackboardKeySelector::GetSelectedKeyID() ‚Å³‚µ‚¢’l‚ªŽæ“¾‚Å‚«‚È‚¢
		ApproachOriginKey.ResolveSelectedKey(*BBAsset);
		ApproachTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTD_Approached::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return CalcConditionImpl(OwnerComp, NodeMemory);
}

void UBTD_Approached::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const bool bResult = CalcConditionImpl(OwnerComp, NodeMemory);
	const bool bIsExecutable = IsInversed() != bResult;
	if (bIsExecutable)
	{
		// “à•”‚Å‘¼‚Ìƒm[ƒh‚ÌAbortˆ—‚ªŒÄ‚Î‚ê‚é
		OwnerComp.RequestExecution(this);
	}
}

bool UBTD_Approached::CalcConditionImpl(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	FVector OriginLocation, TargetLocation;
	if (Blackboard->GetLocationFromEntry(ApproachOriginKey.GetSelectedKeyID(), OriginLocation) &&
		Blackboard->GetLocationFromEntry(ApproachTargetKey.GetSelectedKeyID(), TargetLocation))
	{
		float Distance = FVector::Dist(OriginLocation, TargetLocation);
		return Distance < AcceptableRadius;
	}
	
	return false;
}
