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
};
