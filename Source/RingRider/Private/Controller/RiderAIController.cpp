// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderAIController.h"
#include "Rider/AIRider.h"
#include "Rider/Bandit/BanditBand.h"
#include "Components/FallZoneDetector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Gimmick/DashPole.h"
#include "Gimmick/Ring.h"
#include "Gimmick/Stone.h"


ARiderAIController::ARiderAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AIPerception->ConfigureSense(*SightSenseConfig);
	AIPerception->SetDominantSense(SightSenseConfig->GetSenseImplementation());
}

void ARiderAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float RiderZ = AiRider->GetActorLocation().Z;
	if (RiderZ < KillZ)
	{
		OnRiderFellOff();
	}

	if (!IsValid(Blackboard))
		return;

	if (IsValid(AiRider))
	{
		FVector FallPoint;
		if (AiRider->FallZoneDetector->DetectedFallZone(FallPoint))
			Blackboard->SetValueAsVector("FallPoint", FallPoint);
		else
			Blackboard->ClearValue("FallPoint");

		auto StickedBands = AiRider->GetStickedBands();
		if (StickedBands.Num() > 0)
		{
			auto StickedRider = Cast<ARider>(StickedBands[0]->GetOwner());
			if (!IsValid(StickedRider))
			{
				UE_LOG(LogTemp, Error, TEXT("RiderAIController: Sticked Actor was not Rider"));
				return;
			}
			auto TeamAttitude = FGenericTeamId::GetAttitude(AiRider, StickedRider);
			Blackboard->SetValueAsObject("StickedRider", StickedRider);
			Blackboard->SetValueAsEnum("TeamAttitudeOfStickedRider", TeamAttitude);
		}
		else
		{
			Blackboard->ClearValue("StickedRider");
			Blackboard->ClearValue("TeamAttitudeOfStickedRider");
		}

		float Energy = AiRider->GetEnergy();
		if (Energy != Blackboard->GetValueAsFloat("Energy"))
			Blackboard->SetValueAsFloat("Energy", Energy);
	}

	if (IsValid(BanditBand))
	{
		Blackboard->SetValueAsEnum("BanditState", static_cast<uint8>(BanditBand->GetBanditState()));
	}

	// くぐろうとしていたRingが誰かにくぐられてしまったらKeyをクリア
	if (auto Ring = Cast<ARing>(Blackboard->GetValueAsObject("Ring")))
	{
		if (Ring->IsPassed())
			Blackboard->ClearValue("Ring");
	}
}

ETeamAttitude::Type ARiderAIController::GetTeamAttitudeTowards(const AActor& _OtherActor) const
{
	// _OtherActorのチーム判定を行い、判定したチームを返す
	if (auto OtherTeamInterface = Cast<IGenericTeamAgentInterface>(&_OtherActor))
	{
		FGenericTeamId OtherGenericTeamId = OtherTeamInterface->GetGenericTeamId();
		if (OtherGenericTeamId == FGenericTeamId::NoTeam)
			return ETeamAttitude::Neutral;
		else if(OtherGenericTeamId == GenericTeamId)
			return ETeamAttitude::Friendly;
		else
			return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Neutral;
}

void ARiderAIController::OnRiderFellOff()
{
	// TODO:
	// 1. Riderが弾けるエフェクトを再生
	// 2. エフェクトが再生し終わるまで待つ
	AiRider->SetActorTransform(StartTransform);
	AiRider->SetSpeed(AiRider->GetDefaultSpeed());
	if (AiRider->IsDrifting())
		AiRider->StopDrift();
}

void ARiderAIController::OnPerception(AActor* _PerceivedActor, FAIStimulus _Stimulus)
{
	// 新たにアクターを検知
	if (_Stimulus.WasSuccessfullySensed())
	{
		auto TeamAttitude = FGenericTeamId::GetAttitude(GetPawn(), _PerceivedActor);
		switch (TeamAttitude)
		{
		// 敵Riderを検知
		case ETeamAttitude::Hostile:
		{
			if(Blackboard->GetValueAsObject("HostileRider") == NULL)
				Blackboard->SetValueAsObject("HostileRider", _PerceivedActor);
		} break;

		// 味方Riderを検知
		case ETeamAttitude::Friendly:
		{
			if(Blackboard->GetValueAsObject("FriendlyRider") == NULL)
				Blackboard->SetValueAsObject("FriendlyRider", _PerceivedActor);
		} break;

		// ギミックを検知
		case ETeamAttitude::Neutral:
		{
			if (auto DashPole = Cast<ADashPole>(_PerceivedActor))
			{
				if (Blackboard->GetValueAsObject("DashPole") == NULL)
					GetBlackboardComponent()->SetValueAsObject("DashPole", _PerceivedActor);
			}

			else if (auto Ring = Cast<ARing>(_PerceivedActor))
			{
				if (Blackboard->GetValueAsObject("Ring") == NULL && !Ring->IsPassed())
					GetBlackboardComponent()->SetValueAsObject("Ring", _PerceivedActor);
			}

			else if (auto Stone = Cast<AStone>(_PerceivedActor))
			{
				if (Blackboard->GetValueAsObject("Stone") == NULL)
					GetBlackboardComponent()->SetValueAsObject("Stone", _PerceivedActor);
			}
		} break;
		}
	}

	// アクターを見失った
	else
	{
		auto TeamAttitude = FGenericTeamId::GetAttitude(GetPawn(), _PerceivedActor);
		switch (TeamAttitude)
		{
		// 敵Riderを検知
		case ETeamAttitude::Hostile:
		{
			if(Blackboard->GetValueAsObject("HostileRider") == _PerceivedActor)
				Blackboard->ClearValue("HostileRider");
		} break;

		// 味方Riderを検知
		case ETeamAttitude::Friendly:
		{
			if(Blackboard->GetValueAsObject("FriendlyRider") == _PerceivedActor)
				Blackboard->ClearValue("FriendlyRider");
		} break;

		// ギミックを検知
		case ETeamAttitude::Neutral:
		{
			if (auto DashPole = Cast<ADashPole>(_PerceivedActor))
			{
				if (Blackboard->GetValueAsObject("DashPole") == _PerceivedActor)
					Blackboard->ClearValue("DashPole");
			}

			else if (auto Ring = Cast<ARing>(_PerceivedActor))
			{
				if(Blackboard->GetValueAsObject("Ring") == _PerceivedActor)
					Blackboard->ClearValue("Ring");
			}

			else if (auto Stone = Cast<AStone>(_PerceivedActor))
			{
				if(Blackboard->GetValueAsObject("Stone") == _PerceivedActor)
					Blackboard->ClearValue("Stone");
			}
		} break;
		}
	}
}

void ARiderAIController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	AiRider = Cast<AAIRider>(_Pawn);
	if (!AiRider)
	{
		UE_LOG(LogTemp, Error, TEXT("RiderAIController: Failed to possess AIRider!!"));
		return;
	}
	StartTransform = AiRider->GetActorTransform();

	// Bandit Band
	BanditBand = AiRider->FindComponentByClass<UBanditBand>();
	if (!BanditBand)
	{
		UE_LOG(LogTemp, Error, TEXT("RiderAIController: Failed to get BanditBand from: %s"), *AiRider->GetName());
		return;
	}

	// Behavior Tree
	if (AiRider->BTAsset)
	{
		if (!RunBehaviorTree(AiRider->BTAsset))
			UE_LOG(LogTemp, Error, TEXT("Failed to run BehaviorTree"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Could not load BTAsset from %s"), *_Pawn->GetName());

	// AI Perception
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ARiderAIController::OnPerception);
	GenericTeamId = FGenericTeamId(AiRider->TeamId);
}
