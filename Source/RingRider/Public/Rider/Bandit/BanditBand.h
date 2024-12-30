// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PsmComponent.h"
#include "BanditBand.generated.h"


class UNiagaraComponent;
class USphereComponent;


DECLARE_MULTICAST_DELEGATE_OneParam(FStartAimDelegate, const FVector&)
DECLARE_MULTICAST_DELEGATE_OneParam(FAimingDelegate, const FVector&)
DECLARE_MULTICAST_DELEGATE(FEndAimDelegate)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UBanditBand : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	UBanditBand();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//protected:
//	UFUNCTION()
//    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
//                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
//                        bool bFromSweep, const FHitResult& SweepResult);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bandit Properties")
	bool bCanShoot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bandit Properties")
	float MaxLength = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bandit Properties")
	float ExpandSpeed = 1000;



// Band VFX ////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* BanditVFX;

private:
	const static FString BANDIT_BEAM_END;
	const static FString BANDIT_BEAM_WIDTH;
	const static FString BANDIT_COLOR;
	const static FString BANDIT_INTENSITY;



// Band Tip ////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* BandTip;



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
	bool bIsAiming = false;
	FVector AimTarget;

private:
	bool CheckSnap(const FVector& _AimTarget, FVector& SnapPos);



// Shoot Out ////////////////////////////////////////////////////////////////////////////////////
public:
	void ShootBand(const FVector* _AimTarget = nullptr);
	void CutBand();

private:
	UPROPERTY(VisibleAnywhere)
	UPsmComponent* Psm;

	UPsmComponent::TStateFunc ExpandState;
	void ExpandStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc StickState;
	void StickStateFunc(const FPsmInfo& Info);
};
