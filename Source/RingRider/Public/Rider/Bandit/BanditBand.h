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

	/*
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	*/


private:
	// !!! �I�[�i�[��[IMoveable]���������邱�� !!!
	class IMoveable* OwnerMoveable;
	class IPhysicsMoveable* OwnerPhysicsMoveable;

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

	// ��������_�b�V�����ɁA�������ΏۂƂ̋��������̒l�����������_�b�V�������Ƃ���
	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float NearDistanceOnPullDash;

	UPROPERTY(EditAnywhere, Category = "Bandit Properties|Pull Dash")
	float PullImpulse;

private:
	const static FString BANDIT_BEAM_END;
	const static FString BANDIT_BEAM_WIDTH;
	const static FString BANDIT_COLOR;
	const static FString BANDIT_INTENSITY;



// Aiming ////////////////////////////////////////////////////////////////////////////////////////
public:
	void StartAim(const FVector& _AimTarget);
	void EndAim();

	void SetAimTarget(const FVector& NewTarget)
	{
		AimTarget = NewTarget;
	}

public:
	FDelegateHandle AddOnStartAimAction(TFunction<void(const FVector& AimPos)> NewFunc);
	void RemoveOnStartAimAction(FDelegateHandle DelegateHandle);

	FDelegateHandle AddOnAimingAction(TFunction<void(const FVector& AimPos)> NewFunc);
	void RemoveOnAimingAction(FDelegateHandle DelegateHandle);

	FDelegateHandle AddOnEndAimAction(TFunction<void()> NewFunc);
	void RemoveOnEndAimAction(FDelegateHandle DelegateHandle);

private:
	FStartAimDelegate OnStartAimActions;
	FAimingDelegate OnAimingActions;
	FEndAimDelegate OnEndAimActions;

private:
	FVector AimTarget;

private:
	bool CheckSnap(const FVector& _AimTarget, FVector& SnapPos);



// Actions ///////////////////////////////////////////////////////////////////////////////////////
public:
	void ShootBand(const FVector* _AimTarget = nullptr);
	void CutBand();

	void StartPullDash();

private:
	FVector StickedPos;



// States ////////////////////////////////////////////////////////////////////////////////////////
private:
	UPsmComponent* Psm;

	UPsmComponent::TStateFunc AimState;
	void AimStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc ExpandState;
	void ExpandStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc StickState;
	void StickStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc PullDashState;
	void PullDashStateFunc(const FPsmInfo& Info);
};
