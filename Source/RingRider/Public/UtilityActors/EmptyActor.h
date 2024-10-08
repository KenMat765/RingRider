// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmptyActor.generated.h"

UCLASS()
class RINGRIDER_API AEmptyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEmptyActor();

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComp;
};
