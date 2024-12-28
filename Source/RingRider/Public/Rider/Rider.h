// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PsmComponent.h"
#include "GameInfo.h"
#include "Rider.generated.h"


class UBoxComponent;
class USpringArmComponent;
class UNiagaraComponent;
class UAfterImageComponent;
class USearchLightComponent;
class UBanditBand;


DECLARE_MULTICAST_DELEGATE_TwoParams(FSpeedChangeDelegate, float, float)
DECLARE_MULTICAST_DELEGATE_TwoParams(FEnergyChangeDelegate, float, float)


UCLASS()
class RINGRIDER_API ARider : public APawn
{
	GENERATED_BODY()

public:
	ARider();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyHit(
		class UPrimitiveComponent* MyComp,
		class AActor* Other,
		class UPrimitiveComponent* OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;



	// Components //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* RootBox;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* BikeBase;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Bike;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Wheel;

	UPROPERTY(VisibleAnywhere)
	UPsmComponent* Psm;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SparkComp;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SpinComp;

	UPROPERTY(VisibleAnywhere)
	UAfterImageComponent* ImageComp;

	UPROPERTY(VisibleAnywhere)
	USearchLightComponent* SearchLightComp;

	UPROPERTY(VisibleAnywhere)
	UBanditBand* BanditBand;


	// Team ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Team")
	ETeam Team;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }



	// Speed ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Speed")
	float Speed;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Speed")
	float MaxSpeed;	
	
	UPROPERTY(EditAnywhere, Category="Rider Properties|Speed")
	float DefaultSpeed;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Speed")
	bool bCanMoveForward = true;

public:
	float GetSpeed() const { return Speed; }
	float GetMaxSpeed() const { return MaxSpeed; }
	float GetDefaultSpeed() const { return DefaultSpeed; }

	void SetSpeed(float NewSpeed) { Speed = NewSpeed; TriggerOnSpeedChangeActions(Speed, MaxSpeed); }
	void AddSpeed(float DeltaSpeed) { Speed += DeltaSpeed; TriggerOnSpeedChangeActions(Speed, MaxSpeed); }

private:
	FSpeedChangeDelegate OnSpeedChangeActions;
	void TriggerOnSpeedChangeActions(float _NewSpeed, float _MaxSpeed) const;

public:
	FDelegateHandle AddOnSpeedChangeAction(TFunction<void(float, float)> NewFunc);
	void RemoveOnSpeedChangeAction(FDelegateHandle DelegateHandle);



	// Energy ///////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float Energy;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float MaxEnergy;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Energy")
	float EnergyStealRate;

public:
	float GetEnergy() const { return Energy; }
	void SetEnergy(float NewEnergy) { Energy = NewEnergy; TriggerOnEnergyChangeActions(Energy, MaxEnergy); }
	void AddEnergy(float DeltaEnergy) { Energy += DeltaEnergy; TriggerOnEnergyChangeActions(Energy, MaxEnergy); }

	float GetMaxEnergy() const { return MaxEnergy; }
	void SetMaxEnergy(float NewMaxEnergy) { MaxEnergy = NewMaxEnergy; }

	float GetEnergyStealRate() const { return EnergyStealRate; }
	void SetEnergyStealRate(float NewStealRate) { EnergyStealRate = NewStealRate; }

private:
	FEnergyChangeDelegate OnEnergyChangeActions;
	void TriggerOnEnergyChangeActions(float _NewEnergy, float _MaxEnergy) const;

public:
	FDelegateHandle AddOnEnergyChangeAction(TFunction<void(float, float)> NewFunc);
	void RemoveOnEnergyChangeAction(FDelegateHandle DelegateHandle);

private:
	void StealEnergy(ARider* RiderToStealFrom);



	// Tilt & Rotation ///////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Rotation")
	float MaxRotationSpeed;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Rotation")
	float MaxTilt;	// 通常走行時の最大の傾き

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Rotation")
	bool bCanTilt = true;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Rotation")
	bool bCanCurve = true;



	// Curve Accel ///////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Curve Accel")
	float CurveAcceleration;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Curve Accel")
	float CurveDeceleration;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Curve Accel")
	float MaxSpeedOffset;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Curve Accel")
	float SpeedOffset;	// This value varies with the amount of tilt.

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Curve Accel")
	bool bCanAccelOnCurve = true;

private:
	void AccelSpeed(float TargetSpeed, float Acceleration, float DeltaTime);



	// Grounded //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Grounded")
	bool bIsGrounded = false;

	// NotifyHitで値が変化するbool変数用
	// OnCollisionExitが無いため、boolをfalseにするタイミングはフレームの最後になる
	// 毎フレームの最後に常にfalseにならないようにするためのバッファ変数が必要
	bool bIsGroundedBuffer = false;

public:
	bool IsGrounded() const { return bIsGrounded; };



	// Collision ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Collision")
	float CollisionImpulse;

	UPROPERTY(VisibleAnywhere, Category="Rider Properties|Collision")
	bool bCanBounce = true;	// Used to prevent getting multiple impulse on collision.



	// Properties ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Jump")
	float JumpImpulse;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	float SlideDuration;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	float SlideMaxSpeed;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	class UCurveFloat* SlideCurve;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Action|Slide")
	float SlideTilt;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	float SparkTilt;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	int MaxSparkRate;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|Spark")
	int MinSparkRate;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	FLinearColor AfterImageColor;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageMetallic;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageRoughness;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageOpacity;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageLifetime;

	UPROPERTY(EditAnywhere, Category="Rider Properties|VFX|AfterImage")
	float AfterImageInterval;



	// States ////////////////////////////////////////////////////////////////////////////////
private:
	UPsmComponent::TStateFunc SlideState;
	void SlideStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc JumpState;
	void JumpStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc BoostState;
	void BoostStateFunc(const FPsmInfo& Info);

	UPsmComponent::TStateFunc DriftState;
	void DriftStateFunc(const FPsmInfo& Info);



	// Drift //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Drift")
	float DriftImpulse;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Drift")
	float DriftMidTilt;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Drift")
	float DriftTiltRange;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Drift")
	float DriftInertiaSpeed;



	// Boost //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostSpeed;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostPitch;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostEnterEnergy;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Boost")
	float BoostStayEnergyPerSec;

public:
	bool IsBoosting() { return Psm->IsStateOn(BoostState); }



	// Lock On ////////////////////////////////////////////////////////////////////////////////
private:
	TArray<AActor*> TargetActors;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Lock On")
	float LockOnRadius;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Lock On")
	float LockOnAngle;

	UPROPERTY(EditAnywhere, Category="Rider Properties|Lock On")
	float LockOnAssistStrength;

private:
	TArray<AActor*> SearchTargetActor(float Radius, float Angle);
	void LookAtActor(AActor* TargetActor, float RotationSpeed, float DeltaTime);



	// Input Events ///////////////////////////////////////////////////////////////////////////
private:
	void OnSwipeUp();
	void OnSwipeDown();
	void OnSwipeLeft();
	void OnSwipeRight();

	void OnPressedBoost();
	void OnReleasedBoost();

	void OnPressedBandit();
	void OnRepeatBandit();
	void OnReleasedBandit();

	void OnJoyStick(float AxisValue);

	float StickValue;



	// Constants ////////////////////////////////////////////////////////////////////////////////
public:
	const static float BIKE_RADIUS;

private:
	const static FName SPARK_SPAWN_RATE;
};
