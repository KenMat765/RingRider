// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/FsmComponent.h"
#include "BanditBand.generated.h"


/**
* BanditBand�̂����������܂Ƃ߂��\����
*/
USTRUCT()
struct FBanditStickInfo
{
	GENERATED_BODY()

	FBanditStickInfo();
	FBanditStickInfo(const FVector& _StickPos, AActor* _StickActor, UPrimitiveComponent* _StickComp);

	FVector StickPos;
	AActor* StickActor;
	UPrimitiveComponent* StickComp;
};


/**
* �M�~�b�N���ɂ������Ďg�p���A�������邱�ƂŃA�N�^�[�ɐV���ȃA�N�V�������N��
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UBanditBand : public UNiagaraComponent
{
	GENERATED_BODY()

public:	
	UBanditBand();

protected:
	virtual void BeginPlay() override;


public:
	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	bool bCanShoot = true;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (
		ToolTip="Band is automatically cut when it reaches this length"))
	float MaxLength = 20000.f;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (
		ToolTip="Movement speed of the band tip during launch."))
	float TipSpeed = 50000;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float TipRadius = 42;

private:
	const static FString BANDIT_BEAM_END;
	const static FString BANDIT_BEAM_WIDTH;
	const static FString BANDIT_COLOR;
	const static FString BANDIT_INTENSITY;



// Actions ///////////////////////////////////////////////////////////////////////////////////////
public:
	void ShootBand(const FVector& _ShootPos);
	void CutBand();
	void StickBand(const FBanditStickInfo& _StickInfo);
	bool IsSticked() const { return bIsSticked; };
	FBanditStickInfo GetStickInfo() const { return StickInfo; }
	void PullBand();

	FVector GetTipPos() const
	{
		// �ˏo�O�̏�Ԃł���΁A��[�̈ʒu�̓R���|�[�l���g�̈ʒu�Ɠ���
		if (Fsm->IsNullState())
			return GetComponentLocation();
		return CurrentTipPos;
	};

	float GetBandLength() const
	{
		return (GetTipPos() - GetComponentLocation()).Size();
	}

	FVector GetBandDirection() const
	{
		return (GetTipPos() - GetComponentLocation()).GetSafeNormal();
	}

private:
	class IBanditStickable* BanditStickable;

	FVector ShootPos;
	bool bIsSticked = false;
	FBanditStickInfo StickInfo;
	FVector CurrentTipPos;

	void SetTipPos(const FVector& _TipPos);
	bool SearchStickableBySweep(FHitResult& _HitResult, const FVector& _StartPos, const FVector& _EndPos);



// States ////////////////////////////////////////////////////////////////////////////////////////
public:
	bool IsExpandState()   { return Fsm->GetCurrentState() == &ExpandState;	  };
	// IsSticked()�Ƃ͈قȂ�F��������_�b�V�������������Ă��邪�A���̊֐��͂��̏ꍇ�ł�false��Ԃ��B�A�N�V�������N���O�́A�����������Ă����Ԃ̂�true��Ԃ��B
	bool IsStickState()	   { return Fsm->GetCurrentState() == &StickState;	  };
	bool IsPullState() { return Fsm->GetCurrentState() == &PullState; };

private:
	UFsmComponent* Fsm;

	UFsmComponent::TFsmStateFunc ExpandState;
	void ExpandStateFunc(const FFsmInfo& Info);

	UFsmComponent::TFsmStateFunc StickState;
	void StickStateFunc(const FFsmInfo& Info);

	UFsmComponent::TFsmStateFunc PullState;
	void PullStateFunc(const FFsmInfo& Info);
};
