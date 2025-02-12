// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_CopyKeyValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CopyKeyValue::UBTS_CopyKeyValue()
{
	NodeName = "Copy Key Value";
}

void UBTS_CopyKeyValue::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		// ‚±‚ê‚ðŒÄ‚Î‚È‚¢‚Æ FBlackboardKeySelector::GetSelectedKeyID() ‚Å³‚µ‚¢’l‚ªŽæ“¾‚Å‚«‚È‚¢
		CopySourceKey.ResolveSelectedKey(*BBAsset);
		CopyTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

void UBTS_CopyKeyValue::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	UBlackboardComponent* Blackboard = SearchData.OwnerComp.GetBlackboardComponent();

	bool bCopySucceed = Blackboard->CopyKeyValue(CopySourceKey.GetSelectedKeyID(), CopyTargetKey.GetSelectedKeyID());
	if (!bCopySucceed)
		UE_LOG(LogTemp, Error, TEXT("BTS_CopyKeyValue: Failed Copy"));
}

