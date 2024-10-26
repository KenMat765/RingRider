// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WaveInfoUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/RetainerBox.h"
#include "GameModes/GameModeBattle.h"


UWaveInfoUserWidget::UWaveInfoUserWidget(const FObjectInitializer& ObjectInitializer):
	UUserWidget(ObjectInitializer)
{
}



void UWaveInfoUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}


void UWaveInfoUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto GameModeBattle = Cast<AGameModeBattle>(GetWorld()->GetAuthGameMode());
	if (GameModeBattle != nullptr)
	{
		auto OnTimeUpdateDelegate = [this](float NewTime, float MaxTime) { OnTimeUpdate(NewTime, MaxTime); };
		OnTimeUpdateDelegateHandle = GameModeBattle->AddOnTimeUpdateAction(OnTimeUpdateDelegate);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not get GameModeBattle"));
}


void UWaveInfoUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	auto GameModeBattle = Cast<AGameModeBattle>(GetWorld()->GetAuthGameMode());
	if (GameModeBattle != nullptr)
	{
		GameModeBattle->RemoveOnTimeUpdateAction(OnTimeUpdateDelegateHandle);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not get GameModeBattle"));
}



void UWaveInfoUserWidget::ShowWaveText(int WaveNum)
{
	FString WaveStr = TEXT("WAVE ") + FString::FromInt(WaveNum);
	WaveText->SetText(FText::FromString(WaveStr));
}


void UWaveInfoUserWidget::ShowTimeText(float TimeInSec)
{
	int TimeInSec_Int = FMath::CeilToInt(TimeInSec);
	int Min = TimeInSec_Int / 60;
	int Sec = TimeInSec_Int % 60;
	FString WaveStr = FString::FromInt(Min) + TEXT(":") + FString::FromInt(Sec);
	TimeText->SetText(FText::FromString(WaveStr));
}


void UWaveInfoUserWidget::ShowTimeMeter(float TimeRatio)
{
	UMaterialInstanceDynamic* TimeMeter = TimeRetainerBox->GetEffectMaterial();
	TimeMeter->SetScalarParameterValue(FName("Value"), TimeRatio);
}



void UWaveInfoUserWidget::OnTimeUpdate(float NewTime, float MaxTime)
{
	ShowTimeText(NewTime);
	float TimeRatio =  NewTime / MaxTime;
	UE_LOG(LogTemp, Log, TEXT("MaxTime: %f"), MaxTime);
	UE_LOG(LogTemp, Log, TEXT("TimeRatio: %f"), TimeRatio);
	ShowTimeMeter(TimeRatio);
}
