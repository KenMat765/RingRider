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
* ギミック等にくっつけて使用し、アクターに新たな移動アクションを提供する
*
* !!! [IMoveable]を実装したアクターに付けること !!!
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
	// !!! オーナーは[IMoveable]を実装すること !!!
	class IMoveable* OwnerMoveable;

public:
	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	bool bCanShoot = true;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float MaxLength = 25000;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float ExpandSpeed = 50000;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float TipRadius = 42;

	// Ownerを進行方向へ向ける速度
	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float TurnSpeed;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float AccelOnPullDash;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float BoostOnPullDash;

	// 引っ張りダッシュ時に、くっつき対象との距離がこの値を下回ったらダッシュ完了とする
	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float NearDistanceOnPullDash;

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
	FVector GetTipPos() const;
	float GetBandLength() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCutBandDelegate);
	FCutBandDelegate OnCutBand;

private:
	FVector ShootPos;
	bool bIsSticked = false;
	FVector StickedPos;
	AActor* StickedActor;
	FVector CurrentTipPos;
	float CurrentBandLength = 0.f;

	void SetTipPos(const FVector& _TipPos);
	bool SearchStickableBySweep(FHitResult& _HitResult, const FVector& _StartPos, const FVector& _EndPos);



// States ////////////////////////////////////////////////////////////////////////////////////////
public:
	bool IsExpandState()   { return Fsm->GetCurrentState() == &ExpandState;	  };
	// IsSticked()とは異なる：引っ張りダッシュ中もくっついているが、この関数はその場合でもfalseを返す。アクションを起す前の、ただくっついている状態のみtrueを返す。
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
