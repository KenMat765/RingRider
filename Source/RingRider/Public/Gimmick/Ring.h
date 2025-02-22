// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Energy.h"
#include "Ring.generated.h"


class UBoxComponent;
class UNiagaraComponent;


UCLASS()
class RINGRIDER_API ARing : public AActor, public IEnergy
{
	GENERATED_BODY()
	
public:	
	ARing();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;



	// Animation ///////////////////////////////////////////////////////////////////////////////////
private:
	static constexpr int VertNum = 6;

	float AnimTimer = 0.f;

	float StartScale;

	UPROPERTY(EditAnywhere, Category="Ring Properties|Animation")
	float AnimDuration = 1.f;

	UPROPERTY(EditAnywhere, Category="Ring Properties|Animation")
	float RotateSpeed = 1080.f;

	UPROPERTY(EditAnywhere, Category="Ring Properties|Animation")
	UCurveFloat* AnimCurve;



	// Energy //////////////////////////////////////////////////////////////////////////////////////
public:
	virtual bool CanModifyEnergy() const override { return bCanModifyEnergy; }
	virtual void SetEnergyModifiable(bool _bModifiable) override { bCanModifyEnergy = _bModifiable; }

	virtual float GetEnergy() const override { return Energy; }
	virtual void SetEnergy(float _NewEnergy) override { Energy = _NewEnergy; }

private:
	UPROPERTY(EditAnywhere, Category="Ring Properties")
	bool bCanModifyEnergy = true;

	UPROPERTY(EditAnywhere, Category="Ring Properties")
	float Energy = 100;



	// Components //////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	TArray<UBoxComponent*> ColComps;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PassCheckComp;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* ObtainComp;



	// Overlap Events //////////////////////////////////////////////////////////////////////////////
private:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherbodyIndex,
		bool bFromSweep,
		const FHitResult& Hit
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);



	// Actor Passed Events /////////////////////////////////////////////////////////////////////////
public:
	bool IsPassed() const { return bIsPassed; }

private:
	bool bIsPassed = false;
	AActor* PassingActor;
	float EnterSide;

	void OnActorPassed(AActor* _PassedActor);

	UPROPERTY(EditAnywhere, Category="Ring Properties")
	float SpeedBoostOnPassed = 1000.f;



	// VFX /////////////////////////////////////////////////////////////////////////////////////////
	static const FString TARGET_POSITION;
};
