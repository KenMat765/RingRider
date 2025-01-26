// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BanditSnapArea.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UBanditSnapArea : public USphereComponent
{
	GENERATED_BODY()

public:	
	UBanditSnapArea();

protected:
	virtual void BeginPlay() override;


public:
	bool IsSnappable() const { return bIsSnappable; }
	void EnableSnap(bool _bEnable)
	{
		bIsSnappable = _bEnable;
		ECollisionChannel CollisionChannel = _bEnable ? SnapChannel : NoSnapChannel;
		SetCollisionObjectType(CollisionChannel);
	}

private:
	UPROPERTY(EditAnywhere)
	bool bIsSnappable = true;

	static const ECollisionChannel NoSnapChannel = ECollisionChannel::ECC_WorldStatic;
	static const ECollisionChannel SnapChannel = ECollisionChannel::ECC_GameTraceChannel2;
};
