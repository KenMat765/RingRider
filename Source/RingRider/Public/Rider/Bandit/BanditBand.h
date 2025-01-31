// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/FsmComponent.h"
#include "BanditBand.generated.h"


class IBanditStickable;


/**
* BanditBand�̏�ԊǗ��̂��߂̗񋓌^ (BT�Ŏ�Ɏg�p)
*/
UENUM(BlueprintType)
enum class EBanditState : uint8
{
	STANDBY	UMETA(DisplayName = "Standby"),
	EXPAND	UMETA(DisplayName = "Expand"),
	STICK	UMETA(DisplayName = "Stick"),
	PULL	UMETA(DisplayName = "Pull")
};


/**
* BanditBand�̂����������܂Ƃ߂��\����
*/
USTRUCT()
struct FBanditStickInfo
{
	GENERATED_BODY()

	FBanditStickInfo();
	FBanditStickInfo(const FVector& _StickPos, AActor* _StickActor, UPrimitiveComponent* _StickComp, IBanditStickable* _BanditStickable);

	FVector StickPos;
	AActor* StickActor;
	UPrimitiveComponent* StickComp;
	IBanditStickable* BanditStickable;
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
	bool CanShoot() const { return bCanShoot; };
	float GetMaxLength() const { return MaxLength; };

private:
	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	bool bCanShoot = true;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (
		ToolTip="Band is automatically cut when it reaches this length"))
	float MaxLength = 15000.f;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (
		ToolTip="Movement speed of the band tip during launch."))
	float TipSpeed = 40000.f;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float TipRadius = 42;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (ClampMin="0.0",
		ToolTip="Duration [sec] for enabling aim after cut"))
	float ShootEnableDuration = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (ClampMin="0.0",
		ToolTip="Duration [sec] this band can exist after pulling"))
	float MaxLifetimeAfterPull = 5.f;

	const static FString BANDIT_BEAM_END;
	const static FString BANDIT_BEAM_WIDTH;
	const static FString BANDIT_COLOR;
	const static FString BANDIT_INTENSITY;



// Actions ///////////////////////////////////////////////////////////////////////////////////////
public:
	void ShootBand(const FVector& _ShootPos);
	void CutBand();
	void StickBand(FVector& _StickPos, AActor& _StickActor, UPrimitiveComponent& _StickComp, IBanditStickable& _BanditStickable);
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
	FVector ShootPos;
	bool bIsSticked = false;
	FBanditStickInfo StickInfo;
	FVector CurrentTipPos;

	void SetTipPos(const FVector& _TipPos);
	bool SearchStickableBySweep(FHitResult& _HitResult, const FVector& _StartPos, const FVector& _EndPos);



// States ////////////////////////////////////////////////////////////////////////////////////////
public:
	// �ˏo�O�̉������Ă��Ȃ����
	bool IsNullState() const { return Fsm->IsNullState(); }
	bool IsExpandState() const { return Fsm->GetCurrentState() == &ExpandState; }
	// IsSticked()�Ƃ͈قȂ�F��������_�b�V�������������Ă��邪�A���̊֐��͂��̏ꍇ�ł�false��Ԃ��B�A�N�V�������N���O�́A�����������Ă����Ԃ̂�true��Ԃ��B
	bool IsStickState() const { return Fsm->GetCurrentState() == &StickState; }
	bool IsPullState() const { return Fsm->GetCurrentState() == &PullState; }
	EBanditState GetBanditState() const
	{
		if		(IsNullState())	  return EBanditState::STANDBY;
		else if (IsExpandState()) return EBanditState::EXPAND;
		else if (IsStickState())  return EBanditState::STICK;
		else if (IsPullState())   return EBanditState::PULL;
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BanditBand: was not in any state. Returning EBanditState::STANDBY."));
			return EBanditState::STANDBY;
		}
	}

private:
	UFsmComponent* Fsm;

	UFsmComponent::TFsmStateFunc ExpandState;
	void ExpandStateFunc(const FFsmInfo& Info);

	UFsmComponent::TFsmStateFunc StickState;
	void StickStateFunc(const FFsmInfo& Info);

	UFsmComponent::TFsmStateFunc PullState;
	void PullStateFunc(const FFsmInfo& Info);
};
