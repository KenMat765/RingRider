// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BanditSnapArea.generated.h"


class USphereComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UBanditSnapArea : public USceneComponent
{
	GENERATED_BODY()

public:	
	UBanditSnapArea();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* SnapArea;
};
