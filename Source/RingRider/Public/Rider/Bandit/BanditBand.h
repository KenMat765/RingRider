// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/FsmComponent.h"
#include "BanditBand.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FStartAimDelegate, const FVector&)
DECLARE_MULTICAST_DELEGATE_OneParam(FAimingDelegate, const FVector&)
DECLARE_MULTICAST_DELEGATE(FEndAimDelegate)


/**
* �M�~�b�N���ɂ������Ďg�p���A�A�N�^�[�ɐV���Ȉړ��A�N�V������񋟂���
*
* !!! [IMoveable]�����������A�N�^�[�ɕt���邱�� !!!
*
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UBanditBand : public UNiagaraComponent
{
	GENERATED_BODY()

public:	
	UBanditBand();

protected:
	virtual void BeginPlay() override;


private:
	// !!! �I�[�i�[��[IMoveable]���������邱�� !!!
	class IMoveable* OwnerMoveable;

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

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash", meta = (
		ToolTip="Rotation speed at which the owner is oriented toward the sticked target during pull dash"))
	float TurnSpeedOnPullDash;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash", meta = (
		ToolTip="Speed continuously added during pull dash"))
	float AccelOnPullDash;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash", meta = (
		ToolTip="Speed added at the start of pull dash"))
	float BoostOnPullDash;

private:
	const static FString BANDIT_BEAM_END;
	const static FString BANDIT_BEAM_WIDTH;
	const static FString BANDIT_COLOR;
	const static FString BANDIT_INTENSITY;



// Actions ///////////////////////////////////////////////////////////////////////////////////////
public:
	void ShootBand(const FVector& _ShootPos);
	void CutBand();
	void StickBand(const FVector& _StickPos, AActor* _StickActor);
	bool IsSticked() const { return bIsSticked; };
	FVector GetStickedPos() const { return StickedPos; };
	AActor* GetStickedActor() const { return StickedActor; };
	void StartPullDash();

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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCutBandDelegate);
	FCutBandDelegate OnCutBand;

private:
	FVector ShootPos;
	bool bIsSticked = false;
	FVector StickedPos;
	AActor* StickedActor;
	FVector CurrentTipPos;

	void SetTipPos(const FVector& _TipPos);
	bool SearchStickableBySweep(FHitResult& _HitResult, const FVector& _StartPos, const FVector& _EndPos);



// States ////////////////////////////////////////////////////////////////////////////////////////
public:
	bool IsExpandState()   { return Fsm->GetCurrentState() == &ExpandState;	  };
	// IsSticked()�Ƃ͈قȂ�F��������_�b�V�������������Ă��邪�A���̊֐��͂��̏ꍇ�ł�false��Ԃ��B�A�N�V�������N���O�́A�����������Ă����Ԃ̂�true��Ԃ��B
	bool IsStickState()	   { return Fsm->GetCurrentState() == &StickState;	  };
	bool IsPullDashState() { return Fsm->GetCurrentState() == &PullDashState; };

private:
	UFsmComponent* Fsm;

	UFsmComponent::TFsmStateFunc ExpandState;
	void ExpandStateFunc(const FFsmInfo& Info);

	UFsmComponent::TFsmStateFunc StickState;
	void StickStateFunc(const FFsmInfo& Info);

	UFsmComponent::TFsmStateFunc PullDashState;
	void PullDashStateFunc(const FFsmInfo& Info);
};
