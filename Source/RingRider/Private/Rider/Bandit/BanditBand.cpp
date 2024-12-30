// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditBand.h"
#include "NiagaraComponent.h"
#include "GameInfo.h"
#include "Components/SphereComponent.h"


const FString UBanditBand::BANDIT_BEAM_END	 = TEXT("BeamEnd");
const FString UBanditBand::BANDIT_BEAM_WIDTH = TEXT("BeamWidth");
const FString UBanditBand::BANDIT_COLOR		 = TEXT("Color");
const FString UBanditBand::BANDIT_INTENSITY	 = TEXT("Intensity");


UBanditBand::UBanditBand()
{
	PrimaryComponentTick.bCanEverTick = true;


	BanditVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Bandit Band"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BanditNS(TEXT("/Game/Rider/Bandit/NS_BanditBand"));
	if (BanditNS.Succeeded())
		BanditVFX->SetAsset(BanditNS.Object);
	BanditVFX->Deactivate();

	BandTip = CreateDefaultSubobject<USphereComponent>(TEXT("Band Tip"));
	BandTip->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BandTip->SetCollisionProfileName(TEXT("OverlapAll"));
	BandTip->SetGenerateOverlapEvents(true);

	Psm = CreateDefaultSubobject<UPsmComponent>(TEXT("Bandit PSM"));
	ExpandState = [this](const FPsmInfo& Info) { this->ExpandStateFunc(Info); };
	Psm->AddState(ExpandState);
	StickState = [this](const FPsmInfo& Info) { this->StickStateFunc(Info); };
	Psm->AddState(StickState);
}


void UBanditBand::BeginPlay()
{
	Super::BeginPlay();
}


void UBanditBand::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAiming)
	{
		FVector SnapPos;
		if (CheckSnap(AimTarget, SnapPos))
		{
			AimTarget = SnapPos;
		}

		if (OnAimingActions.IsBound())
			OnAimingActions.Broadcast(AimTarget);
	}
}

//void UBanditBand::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
//								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
//								 bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && OtherActor != GetOwner())
//	{
//		if (!Psm->IsStateOn(ExpandState))
//			return;
//		if (!OtherActor->ActorHasTag(FTagList::TAG_BANDIT_STICKABLE))
//			return;
//
//		UE_LOG(LogTemp, Log, TEXT("Sticked to Actor: %s, Comp: %s"), OtherActor->GetFName(), OtherComp->GetFName());
//		Psm->TurnOffState(ExpandState);
//		Psm->TurnOnState(StickState);
//	}
//}



// Aiming ////////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::StartAim(const FVector& _AimTarget)
{
	UE_LOG(LogTemp, Log, TEXT("Start Aim"));

	AimTarget = _AimTarget;
	FVector SnapPos;
	if (CheckSnap(AimTarget, SnapPos))
	{
		AimTarget = SnapPos;
	}

	bIsAiming = true;
	if (OnStartAimActions.IsBound())
		OnStartAimActions.Broadcast(AimTarget);
}

void UBanditBand::EndAim()
{
	UE_LOG(LogTemp, Log, TEXT("End Aim"));

	bIsAiming = false;
	if (OnEndAimActions.IsBound())
		OnEndAimActions.Broadcast();
}

FDelegateHandle UBanditBand::AddOnStartAimAction(TFunction<void(const FVector& AimPos)> NewFunc)
{
	auto NewAction = FStartAimDelegate::FDelegate::CreateLambda(NewFunc);
	return OnStartAimActions.Add(NewAction);
}

void UBanditBand::RemoveOnStartAimAction(FDelegateHandle DelegateHandle)
{
	OnStartAimActions.Remove(DelegateHandle);
}

FDelegateHandle UBanditBand::AddOnAimingAction(TFunction<void(const FVector&)> NewFunc)
{
	auto NewAction = FAimingDelegate::FDelegate::CreateLambda(NewFunc);
	return OnAimingActions.Add(NewAction);
}

void UBanditBand::RemoveOnAimingAction(FDelegateHandle DelegateHandle)
{
	OnAimingActions.Remove(DelegateHandle);
}

FDelegateHandle UBanditBand::AddOnEndAimAction(TFunction<void()> NewFunc)
{
	auto NewAction = FEndAimDelegate::FDelegate::CreateLambda(NewFunc);
	return OnEndAimActions.Add(NewAction);
}

void UBanditBand::RemoveOnEndAimAction(FDelegateHandle DelegateHandle)
{
	OnEndAimActions.Remove(DelegateHandle);
}

bool UBanditBand::CheckSnap(const FVector& _AimTarget, FVector& SnapPos)
{
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel2);

	FCollisionQueryParams QueryParam;
	QueryParam.AddIgnoredActor(this->GetOwner());

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetComponentLocation(),
		_AimTarget,
		ObjQueryParam,
		QueryParam
	);

	if (bHit)
	{
		FVector HitActorWorldLoc = Hit.Actor->GetActorLocation();
		FVector HitCompLocalLoc = Hit.Component->GetComponentLocation();
		SnapPos = HitActorWorldLoc + HitCompLocalLoc;
	}

	return bHit;
}



// Shoot Out ////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ShootBand(const FVector* _AimTarget)
{
	UE_LOG(LogTemp, Log, TEXT("Shoot Band"));
	if (_AimTarget)
		AimTarget = *_AimTarget;
	Psm->TurnOnState(ExpandState);
}

void UBanditBand::CutBand()
{
	// TODO
	UE_LOG(LogTemp, Log, TEXT("Cut Band"));
	BanditVFX->Deactivate();
}

void UBanditBand::ExpandStateFunc(const FPsmInfo& Info)
{
	static float CurrentLength = 0;
	static FVector StartWorldPos;
	static FVector ShootWorldDir;

	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		UE_LOG(LogTemp, Log, TEXT("Enter bandit expand state"));
		CurrentLength = 0;
		StartWorldPos = GetComponentLocation();
		ShootWorldDir = (AimTarget - StartWorldPos).GetSafeNormal();
		BanditVFX->SetNiagaraVariableVec3(BANDIT_BEAM_END, StartWorldPos);
		BanditVFX->Activate();
	}
	break;

	case EPsmCondition::STAY:
	{
		CurrentLength += ExpandSpeed * Info.DeltaTime;
		FVector TipWorldPos = StartWorldPos + ShootWorldDir * CurrentLength;

		// Band‚ªL‚Ñ‚«‚Á‚½‚çI—¹
		if (CurrentLength > MaxLength)
		{
			UE_LOG(LogTemp, Log, TEXT("Bandit reached max length"));
			Psm->TurnOffState(ExpandState);
			CutBand();
		}

		BanditVFX->SetNiagaraVariableVec3(BANDIT_BEAM_END, TipWorldPos);
		BandTip->SetWorldLocation(TipWorldPos);
	}
	break;

	case EPsmCondition::EXIT:
	{
	}
	break;
	}
}

void UBanditBand::StickStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
	}
	break;

	case EPsmCondition::STAY:
	{
	}
	break;

	case EPsmCondition::EXIT:
	{
	}
	break;
	}
}
