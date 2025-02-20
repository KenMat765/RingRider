// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/ActorUtility.h"

namespace FActorUtility
{
    TArray<AActor*> GetActorsInFolder(UWorld* World, const FName& FolderName)
    {
        TArray<AActor*> FoundActors;

        if (!World) return FoundActors;

        // ���x�����̂��ׂẴA�N�^�[���擾
        for (AActor* Actor : World->GetCurrentLevel()->Actors)
        {
            if (Actor && Actor->GetFolderPath() == FolderName)
            {
                FoundActors.Add(Actor);
            }
        }

        return FoundActors;
    }
}
