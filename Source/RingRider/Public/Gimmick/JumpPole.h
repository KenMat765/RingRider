// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/BanditStickable.h"
#include "JumpPole.generated.h"


UCLASS()
class RINGRIDER_API AJumpPole : public AActor, public IBanditStickable
{
	GENERATED_BODY()
	
public:	
	AJumpPole();


public:
	virtual bool IsStickable() const { return bIsStickable; }
	virtual void SetStickable(bool _bStickable) { bIsStickable = _bStickable; }

	virtual void OnBanditSticked(UBanditBand* _OtherBanditBand) override;
	virtual void OnBanditPulledEnter(UBanditBand* _OtherBanditBand) override;


private:
	class IPhysicsMoveable* OtherPhysicsMoveable;

	UPROPERTY(EditAnywhere, Category = "Pole Properties")
	bool bIsStickable = true;

	UPROPERTY(EditAnywhere, Category = "Pole Properties", meta = (
		ToolTip="Impulse added on Pull-Jump"))
	float ImpulseOnPullJump = 2500000.f;
};
