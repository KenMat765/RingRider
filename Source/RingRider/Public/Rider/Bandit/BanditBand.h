// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/FsmComponent.h"
#include "BanditBand.generated.h"


class IBanditStickable;


/**
* BanditBandの状態管理のための列挙型 (BTで主に使用)
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
* BanditBandのくっつき情報をまとめた構造体
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
* ギミック等にくっつけて使用し、引っ張ることでアクターに新たなアクションを起す
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

protected:
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* BanditStickFX;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* BanditWinderFX;

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

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0",
		ToolTip="Intensity of chase when target is locked on"))
	float TipChaseRatio = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (ClampMin="0.0", UIMin="0.0",
		ToolTip="Duration [sec] for enabling aim after cut"))
	float ShootEnableDuration = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties", meta = (ClampMin="0.0", UIMin="0.0",
		ToolTip="Duration [sec] this band can exist after pulling"))
	float MaxLifetimeAfterPull = 5.f;

	const static FString BANDIT_BEAM_END;
	const static FString BANDIT_BEAM_WIDTH;
	const static FString BANDIT_COLOR;
	const static FString BANDIT_INTENSITY;
	const static FString BANDIT_STICK_POS;

	// Band先端のリングのエフェクトを表示するかどうか (Expand中は表示するが、Stick中は非表示にする)
	void ShowTipRing(bool bShow)
	{
		float RingAlpha = bShow ? 1.f : 0.f;
		SetNiagaraVariableFloat(TEXT("RingAlpha"), RingAlpha);
	}

	// Bandの巻き取りエフェクトをどの程度表示させるか (0:非表示 ~ 1:全表示)
	void ShowBanditWinder(float _ShowRatio)
	{
		float ShowRatio = FMath::Clamp(_ShowRatio, 0.f, 1.f);
		float ThreshAngle = 180.f * (1.f - ShowRatio);
		BanditWinderFX->SetScalarParameterValueOnMaterials(TEXT("ThreshDeg"), ThreshAngle);
	}



// Actions ///////////////////////////////////////////////////////////////////////////////////////
public:
	void ShootBand(const FVector& _ShootPos);
	void ShootBandTowardComp(UPrimitiveComponent& _ShootTargetComp);
	void CutBand();
	void StickBand(FVector& _StickPos, AActor& _StickActor, UPrimitiveComponent& _StickComp, IBanditStickable& _BanditStickable);
	bool IsSticked() const { return bIsSticked; };
	FBanditStickInfo GetStickInfo() const { return StickInfo; }
	void PullBand();

	FVector GetTipPos() const
	{
		// 射出前の状態であれば、先端の位置はコンポーネントの位置と同じ
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
	UPrimitiveComponent* ShootTargetComp;
	bool bIsSticked = false;
	FBanditStickInfo StickInfo;
	FVector CurrentTipPos;
	FVector StickPos_Local; // くっつき対象が動いても、先端の位置を更新し続けるために必要

	void SetTipPos(const FVector& _TipPos);
	bool SearchStickableBySweep(FHitResult& _HitResult, const FVector& _StartPos, const FVector& _EndPos);



// States ////////////////////////////////////////////////////////////////////////////////////////
public:
	// 射出前の何もしていない状態
	bool IsNullState() const { return Fsm->IsNullState(); }
	bool IsExpandState() const { return Fsm->GetCurrentState() == &ExpandState; }
	// IsSticked()とは異なる：引っ張りダッシュ中もくっついているが、この関数はその場合でもfalseを返す。アクションを起す前の、ただくっついている状態のみtrueを返す。
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
