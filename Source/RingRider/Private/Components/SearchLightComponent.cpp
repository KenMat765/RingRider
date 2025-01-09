// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SearchLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/TransformUtility.h"


USearchLightComponent::USearchLightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USearchLightComponent::BeginPlay()
{
	Super::BeginPlay();
}


TArray<AActor*> USearchLightComponent::SearchActors(
	float Radius,
	float Angle,
    TArray<FName> TargetTags,
    TArray<ECollisionChannel> CollisionChannels,
	UClass* ClassFilter,
	TArray<AActor*> ActorsToIgnore
)
{
    // ===== Distance ===== //
    TArray<AActor*> ActorsInDistance;

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    for(ECollisionChannel CollisionChannel : CollisionChannels)
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(CollisionChannel));

    ActorsToIgnore.Add(GetOwner());

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetComponentLocation(),
        Radius,
        ObjectTypes,
        ClassFilter,
        ActorsToIgnore,
        ActorsInDistance
    );


    // ===== Angle ===== //
    TArray<AActor*> ActorsInAngle;
    for (AActor* ActorInDistance : ActorsInDistance)
    {
        FVector DirectionToActor = ActorInDistance->GetActorLocation() - GetComponentLocation();
        float RadianBetweenActor = FVectorUtility::RadianBetweenVectors(DirectionToActor, GetForwardVector());
        if (RadianBetweenActor <= FMath::DegreesToRadians(Angle))
        {
            ActorsInAngle.Add(ActorInDistance);
        }
    }


    // ===== Tags ===== //
    if (TargetTags.Num() <= 0)
        return ActorsInAngle;

    TArray<AActor*> ActorsWithTag;
    for (AActor* ActorInAngle : ActorsInAngle)
    {
        for (FName Tag : ActorInAngle->Tags)
        {
            if (TargetTags.Contains(Tag))
            {
                ActorsWithTag.Add(ActorInAngle);
                break;
            }
        }
    }

    return ActorsWithTag;
}

