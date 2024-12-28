// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ProjectableUserWidget.h"
#include "Kismet/GameplayStatics.h"
// #include "Kismet/KismetMathLibrary.h"


UProjectableUserWidget::UProjectableUserWidget(const FObjectInitializer& ObjectInitializer):
	UUserWidget(ObjectInitializer)
{
}


void UProjectableUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}


void UProjectableUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get PlayerController"));
		return;
	}
}


void UProjectableUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
}



bool UProjectableUserWidget::GetScreenPos(const FVector& WorldPos, FVector2D& ScreenPos)
{
	bool bProjected = UGameplayStatics::ProjectWorldToScreen(PlayerController, WorldPos, ScreenPos);
	return bProjected;
}
