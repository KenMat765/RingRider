// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityActors/EmptyActor.h"

AEmptyActor::AEmptyActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComp;
}

