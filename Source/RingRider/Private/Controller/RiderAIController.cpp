// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderAIController.h"
#include "Rider/AIRider.h"
#include "BehaviorTree/BlackboardComponent.h"


void ARiderAIController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	AAIRider* AiRider = Cast<AAIRider>(_Pawn);
	if (AiRider && AiRider->BTAsset)
	{
		if (RunBehaviorTree(AiRider->BTAsset))
		{
			// === For Test === //
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
				ARider* StickedRider = Cast<ARider>(GetWorld()->GetFirstPlayerController()->GetPawn());
				GetBlackboardComponent()->SetValueAsObject("StickedRider", StickedRider);
				});
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Failed to run BehaviorTree"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Could not load BTAsset from %s"), *_Pawn->GetName());
}
