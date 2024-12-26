// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ring.generated.h"


class UBoxComponent;
class ARider;
class UNiagaraComponent;


UCLASS()
class RINGRIDER_API ARing : public AActor
{
	GENERATED_BODY()
	
public:	
	ARing();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;



private:
	static constexpr int VertNum = 6;

	// Passed Rider
	ARider* PassedRider;

	UPROPERTY(VisibleAnywhere, Category="Ring Properties")
	bool bIsPassed = false;

	// Animation
	float AnimTimer = 0.f;

	float StartScale;

	UPROPERTY(EditAnywhere, Category="Ring Properties|Animation")
	float AnimDuration = 1.f;

	UPROPERTY(EditAnywhere, Category="Ring Properties|Animation")
	float RotateSpeed = 1080.f;

	UPROPERTY(EditAnywhere, Category="Ring Properties|Animation")
	UCurveFloat* AnimCurve;



	// Energy //////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditAnywhere)
	float EnergyAmount = 100;

public:
	float GetEnergyAmount() const { return EnergyAmount; }
	void SetEnergyAmount(float NewEneygyAmount) { EnergyAmount = NewEneygyAmount; }



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



	// Rider Pass Events /////////////////////////////////////////////////////////////////////////
private:
	void OnRiderPassed(ARider* PassedRider);



	// VFX /////////////////////////////////////////////////////////////////////////////////////////
	static const FString TARGET_POSITION;
};
