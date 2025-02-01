// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/AI/BTTask/BTT_RiderMoveTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/RiderAIController.h"
#include "Rider/AIRider.h"
#include "Utility/TransformUtility.h"


UBTT_RiderMoveTo::UBTT_RiderMoveTo()
{
	NodeName = "Rider Move To";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_RiderMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard))
		return EBTNodeResult::Type::Failed;
	FVector TargetLoc = Blackboard->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	auto AiController = Cast<ARiderAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(AiController))
		return EBTNodeResult::Type::Failed;

	AAIRider* AiRider = AiController->GetAiRider();
	if (!IsValid(AiRider))
		return EBTNodeResult::Type::Failed;

	return EBTNodeResult::Type::InProgress;
}

void UBTT_RiderMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	FVector TargetLoc = Blackboard->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	auto AiController = Cast<ARiderAIController>(OwnerComp.GetAIOwner());
	AAIRider* AiRider = AiController->GetAiRider();

	FVector DiffLoc = TargetLoc - AiRider->GetActorLocation();
	FVector2D DiffLoc_XY = FVector2D(DiffLoc.X, DiffLoc.Y);
	FVector2D RiderForward_XY = FVector2D(AiRider->GetActorForwardVector().X, AiRider->GetActorForwardVector().Y);
	float DegToTarget = FMath::RadiansToDegrees(FVectorUtility::RadianBetweenVectors(RiderForward_XY, DiffLoc_XY));

	float TiltRatio = DegToTarget / MaxTiltDeg;
	AiRider->TiltBike(TiltRatio); // 内部でTiltRatioは [-1.f, 1.f] にクランプされる

	if (AiRider->IsDrifting())
	{
		// 角度が小さくなったらドリフト終了
		if (FMath::Abs(DegToTarget) < DriftEndDeg)
		{
			AiRider->StopDrift();
		}
	}
	else
	{
		// 角度が大きければドリフト
		if (FMath::Abs(DegToTarget) > DriftStartDeg)
		{
			ARider::EDriftDirection DriftDir = DegToTarget < 0 ? ARider::EDriftDirection::LEFT : ARider::EDriftDirection::RIGHT;
			AiRider->StartDrift(DriftDir);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Distance: %f"), DiffLoc.Size());
	if (DiffLoc.SizeSquared() < FMath::Pow(AcceptableRadius, 2))
		FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
}
