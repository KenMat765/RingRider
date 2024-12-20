// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PsmComponent.h"
#include "GameInfo.h"
#include "Rider.generated.h"


class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;
class UNiagaraComponent;
class UAfterImageComponent;


DECLARE_MULTICAST_DELEGATE_TwoParams(FSpeedChangeDelegate, float, float)
DECLARE_MULTICAST_DELEGATE_TwoParams(FEnergyChangeDelegate, float, float)


UCLASS()
class RINGRIDER_API ARider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called on collision
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
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	UPsmComponent* Psm;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SparkComp;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SpinComp;

	UPROPERTY(VisibleAnywhere)
	UAfterImageComponent* ImageComp;



	// Team ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Team")
	ETeam Team;

public:
	ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam) { Team = NewTeam; }



	// Speed ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Speed")
	float Speed;

	// DefaultSpeed + MaxSpeedOffset (カーブによる加速分) + BoostMaxDeltaSpeed (ブーストによる加速分)
	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Speed")
	float MaxSpeed;	
	
	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Speed")
	float DefaultSpeed;

	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Speed")
	bool bCanMoveForward;

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
	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Energy")
	float Energy;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Energy")
	float MaxEnergy;

public:
	float GetEnergy() const { return Energy; }
	void SetEnergy(float NewEnergy) { Energy = NewEnergy; TriggerOnEnergyChangeActions(Energy, MaxEnergy); }
	void AddEnergy(float DeltaEnergy) { Energy += DeltaEnergy; TriggerOnEnergyChangeActions(Energy, MaxEnergy); }

	float GetMaxEnergy() const { return MaxEnergy; }

private:
	FEnergyChangeDelegate OnEnergyChangeActions;
	void TriggerOnEnergyChangeActions(float _NewEnergy, float _MaxEnergy) const;

public:
	FDelegateHandle AddOnEnergyChangeAction(TFunction<void(float, float)> NewFunc);
	void RemoveOnEnergyChangeAction(FDelegateHandle DelegateHandle);



	// Tilt & Rotation ///////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Rotation")
	float MaxRotationSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Rotation")
	float MaxTilt;	// 通常走行時の最大の傾き

	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Rotation")
	bool bCanTilt;

	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Rotation")
	bool bCanCurve;



	// Curve Accel ///////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Curve Accel")
	float CurveAcceleration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Curve Accel")
	float CurveDeceleration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Curve Accel")
	float MaxSpeedOffset;

	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Curve Accel")
	float SpeedOffset;	// This value varies with the amount of tilt.

	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Curve Accel")
	bool bCanAccelOnCurve;



	// Grounded //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Grounded")
	bool bIsGrounded;

	// NotifyHitで値が変化するbool変数用
	// OnCollisionExitが無いため、boolをfalseにするタイミングはフレームの最後になる
	// 毎フレームの最後に常にfalseにならないようにするためのバッファ変数が必要
	bool bIsGroundedBuffer;

public:
	bool IsGrounded() const { return bIsGrounded; };



	// Collision ////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Collision")
	float CollisionImpulse;

	UPROPERTY(VisibleInstanceOnly, Category="Rider Properties|Collision")
	bool bCanBounce;	// Used to prevent getting multiple impulse on collision.



	// Properties ////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Jump")
	float JumpImpulse;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	float SlideDuration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	float SlideMaxSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	class UCurveFloat* SlideCurve;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Slide")
	float SlideTilt;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	float BoostDuration;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	float BoostMaxDeltaSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	class UCurveFloat* BoostCurve;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Boost")
	float BoostMaxPitch;	// ブースト時の最大ピッチ角

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftImpulse;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftMidTilt;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftTiltRange;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|Action|Drift")
	float DriftInertiaSpeed;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|Spark")
	float SparkTilt;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|Spark")
	int MaxSparkRate;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|Spark")
	int MinSparkRate;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	FLinearColor AfterImageColor;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageMetallic;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageRoughness;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageOpacity;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
	float AfterImageLifetime;

	UPROPERTY(EditInstanceOnly, Category="Rider Properties|VFX|AfterImage")
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



	// Input Events ///////////////////////////////////////////////////////////////////////////
private:
	void OnSwipeUp();
	void OnSwipeDown();
	void OnSwipeLeft();
	void OnSwipeRight();

	void OnJoyStick(float AxisValue);

	float StickValue;



	// Constants ////////////////////////////////////////////////////////////////////////////////
public:
	const static float BIKE_RADIUS;
	const static FName RIDER_TAG;

private:
	const static FName SPARK_SPAWN_RATE;
};
