// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTService/BTS_CopyKeyValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CopyKeyValue::UBTS_CopyKeyValue()
{
	NodeName = "Copy Key Value";
}

void UBTS_CopyKeyValue::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	UBlackboardComponent* Blackboard = SearchData.OwnerComp.GetBlackboardComponent();

	auto CopySourceKeyID = Blackboard->GetKeyID(CopySourceKey.SelectedKeyName);	// ‚±‚ê‚¾‚Æ“®‚©‚È‚¢: CopySourceKey.GetSelectedKeyID()
	auto CopyTargetKeyID = Blackboard->GetKeyID(CopyTargetKey.SelectedKeyName);	// ‚±‚ê‚¾‚Æ“®‚©‚È‚¢: CopyTargetKey.GetSelectedKeyID()
	bool bCopySucceed = Blackboard->CopyKeyValue(CopySourceKeyID, CopyTargetKeyID);
	if (!bCopySucceed)
		UE_LOG(LogTemp, Error, TEXT("BTS_CopyKeyValue: Failed Copy"));
}

