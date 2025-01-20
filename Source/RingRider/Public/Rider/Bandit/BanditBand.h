// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/PsmComponent.h"
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

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float MaxLength = 25000;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float ExpandSpeed = 50000;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties")
	float TipRadius = 42;

	// Owner��i�s�����֌����鑬�x
	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float TurnSpeed;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float AccelOnPullDash;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float BoostOnPullDash;

	// ��������_�b�V�����ɁA�������ΏۂƂ̋��������̒l�����������_�b�V�������Ƃ���
	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float NearDistanceOnPullDash;

private:
	const static FString BANDIT_BEAM_END;
	const static FString BANDIT_BEAM_WIDTH;
	const static FString BANDIT_COLOR;
	const static FString BANDIT_INTENSITY;



// Actions ///////////////////////////////////////////////////////////////////////////////////////
public:
	void ShootBand(const FVector& _AimTarget);
	void CutBand();

	void StartPullDash();

	FVector GetStickedPos() const { return StickedPos; };
	AActor* GetStickedActor() const { return StickedActor; };

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCutBandDelegate);
	FCutBandDelegate OnCutBand;

private:
	FVector AimTarget;
	FVector StickedPos;
	AActor* StickedActor;



// States ////////////////////////////////////////////////////////////////////////////////////////
public:
	bool IsExpandState() { return Psm->IsStateOn(ExpandState); };
	bool IsStickState() { return Psm->IsStateOn(StickState); };
	bool IsPullDashState() { return Psm->IsStateOn(PullDashState); };

private:
	UPsmComponent* Psm;

	UPsmComponent::TPsmStateFunc ExpandState;
	void ExpandStateFunc(const FPsmInfo& Info);

	UPsmComponent::TPsmStateFunc StickState;
	void StickStateFunc(const FPsmInfo& Info);

	UPsmComponent::TPsmStateFunc PullDashState;
	void PullDashStateFunc(const FPsmInfo& Info);
};
