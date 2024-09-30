// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ring.generated.h"


class UBoxComponent;


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

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ColComps[VertNum];
};
