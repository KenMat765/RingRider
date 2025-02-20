// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


namespace FActorUtility
{
    TArray<AActor*> GetActorsInFolder(UWorld* World, const FName& FolderName);
}
