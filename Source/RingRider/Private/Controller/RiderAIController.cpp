// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderAIController.h"
#include "Rider/AIRider.h"
#include "Rider/Bandit/BanditBand.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Gimmick/DashPole.h"


ARiderAIController::ARiderAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AIPerception->ConfigureSense(*SightSenseConfig);
	AIPerception->SetDominantSense(SightSenseConfig->GetSenseImplementation());
}

ETeamAttitude::Type ARiderAIController::GetTeamAttitudeTowards(const AActor& _OtherActor) const
{
	UE_LOG(LogTemp, Log, TEXT("GetTeamAttitudeTowards %s"), *_OtherActor.GetName());

	// _OtherActor�̃`�[��������s���A���肵���`�[����Ԃ�
	if (auto OtherTeamInterface = Cast<IGenericTeamAgentInterface>(&_OtherActor))
	{
		FGenericTeamId OtherGenericTeamId = OtherTeamInterface->GetGenericTeamId();
		UE_LOG(LogTemp, Log, TEXT("MyTeamID: %d, OtherTeamID: %d"), GenericTeamId.GetId(), OtherGenericTeamId.GetId());
		if (OtherGenericTeamId == FGenericTeamId::NoTeam)
			return ETeamAttitude::Neutral;
		else if(OtherGenericTeamId == GenericTeamId)
			return ETeamAttitude::Friendly;
		else
			return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Neutral;
}

void ARiderAIController::OnPerception(AActor* _PerceivedActor, FAIStimulus _Stimulus)
{
	UE_LOG(LogTemp, Log, TEXT("Found: %s"), *_PerceivedActor->GetName());

	// �V���ɃA�N�^�[�����m
	if (_Stimulus.WasSuccessfullySensed())
	{
		if (auto BanditStickable = Cast<IBanditStickable>(_PerceivedActor))
		{
			auto TeamAttitude = FGenericTeamId::GetAttitude(GetPawn(), _PerceivedActor);
			UE_LOG(LogTemp, Log, TEXT("Found: %s, TeamAttitude: %d"), *_PerceivedActor->GetName(), TeamAttitude);
			switch (TeamAttitude)
			{
			// �GRider�����m
			case ETeamAttitude::Hostile:
			{
				if(Blackboard->GetValueAsObject("HostileRider") == NULL)
					Blackboard->SetValueAsObject("HostileRider", _PerceivedActor);
			} break;

			// ����Rider�����m
			case ETeamAttitude::Friendly:
			{
				if(Blackboard->GetValueAsObject("FriendlyRider") == NULL)
					Blackboard->SetValueAsObject("FriendlyRider", _PerceivedActor);
			} break;

			// �M�~�b�N(Pole��)�����m
			case ETeamAttitude::Neutral:
			{
				if (auto DashPole = Cast<ADashPole>(_PerceivedActor))
					if(Blackboard->GetValueAsObject("DashPole") == NULL)
						GetBlackboardComponent()->SetValueAsObject("DashPole", _PerceivedActor);
			} break;
			}
		}
	}

	// �A�N�^�[����������
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Lost: %s"), *_PerceivedActor->GetName());
		auto TeamAttitude = FGenericTeamId::GetAttitude(GetPawn(), _PerceivedActor);
		switch (TeamAttitude)
		{
		// �GRider�����m
		case ETeamAttitude::Hostile:
		{
			if(Blackboard->GetValueAsObject("HostileRider") == _PerceivedActor)
				Blackboard->SetValueAsObject("HostileRider", nullptr);
		} break;

		// ����Rider�����m
		case ETeamAttitude::Friendly:
		{
			if(Blackboard->GetValueAsObject("FriendlyRider") == _PerceivedActor)
				Blackboard->SetValueAsObject("FriendlyRider", nullptr);
		} break;

		// �M�~�b�N(Pole��)�����m
		case ETeamAttitude::Neutral:
		{
			if (auto DashPole = Cast<ADashPole>(_PerceivedActor))
				if(Blackboard->GetValueAsObject("DashPole") == _PerceivedActor)
					GetBlackboardComponent()->SetValueAsObject("DashPole", nullptr);
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
