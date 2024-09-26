// Fill out your copyright notice in the Description page of Project Settings.


#include "VFX/AfterImageComponent.h"
#include "VFX/AfterImageActor.h"


UAfterImageComponent::UAfterImageComponent():
	ImageBaseColor(FLinearColor::White),
	ImageMetallic(1.f),
	ImageRoughness(0.f),
	ImageOpacity(0.5f),
	bIsPlaying(false),
	Lifetime(1.f),
	Interval(0.5f),
	SpawnTimer(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAfterImageComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UAfterImageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// �c���A�N�^�[���C���^�[�o�����ɐ���
	if (bIsPlaying)
	{
		SpawnTimer -= DeltaTime;
		if (SpawnTimer <= 0)
		{
			SpawnTimer = Interval;
			AAfterImageActor* NewActor = SpawnAfterImageActor();
			NewActor->Timer = Lifetime;
			ImagesInScene.Add(NewActor);
		}
	}

	// �V�[�����̎c���̃��[�v����
	if (ImagesInScene.Num() > 0)
	{
		// �C�e���[�^����ImagesInScene����Remove�o���Ȃ����߁A��x�L���b�V�����Č�ō폜����
		TArray<AAfterImageActor*> ActorsToDelete;

		for (auto& Image : ImagesInScene)
		{
			// TimerRatio: 1 -> 0
			float TimeRatio = FMath::Clamp(Image->Timer / Lifetime, 0.f, 1.f);
			float Opacity = ImageOpacity * TimeRatio;
			Image->SetMaterialOpacity(Opacity);

			Image->Timer -= DeltaTime;
			if (Image->Timer <= 0)
			{
				ActorsToDelete.Add(Image);
			}
		}

		// ����̃t���[���ō폜����c��������΍폜
		if (ActorsToDelete.Num() > 0)
		{
			for (auto& DeleteActor : ActorsToDelete)
			{
				ImagesInScene.Remove(DeleteActor);
				DeleteActor->Destroy();
			}
		}
	}
}



// Mesh ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UAfterImageComponent::AddMesh(UStaticMesh* NewMesh)
{
	if (NewMesh)
	{
		Meshes.Add(NewMesh);
	}
}

void UAfterImageComponent::RemoveMesh(UStaticMesh* TargetMesh)
{
	if (TargetMesh)
	{
		if (Meshes.Contains(TargetMesh))
		{
			Meshes.Remove(TargetMesh);
		}
	}
}



// Material ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UAfterImageComponent::SetMaterialParams(FLinearColor BaseColor, float Metallic, float Roughness, float Opacity)
{
	ImageBaseColor = BaseColor;
	ImageMetallic = Metallic;
	ImageRoughness = Roughness;
	ImageOpacity = Opacity;
}



// Effect //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UAfterImageComponent::SetLifetime(float NewLifetime)
{
	if (NewLifetime <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("�c���̃��C�t�^�C����0���傫�Ȓl�łȂ���΂Ȃ�܂���B1.0�ɐݒ肵�܂����B"));
		Lifetime = 1.f;
	}
	else
	{
		Lifetime = NewLifetime;
	}
}

void UAfterImageComponent::SetInterval(float NewInterval)
{
	if (NewInterval <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("�c���̃C���^�[�o����0���傫�Ȓl�łȂ���΂Ȃ�܂���B0.5�ɐݒ肵�܂����B"));
		Interval = 0.5f;
	}
	else
	{
		Interval = NewInterval;
	}
}

void UAfterImageComponent::PlayEffect()
{
	bIsPlaying = true;
	SpawnTimer = Interval;
}

void UAfterImageComponent::StopEffect()
{
	bIsPlaying = false;
}



// Utilities //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AAfterImageActor* UAfterImageComponent::SpawnAfterImageActor()
{
	AAfterImageActor* NewActor = GetWorld()->SpawnActor<AAfterImageActor>();

	// �g�����X�t�H�[��
	NewActor->SetActorTransform(GetComponentTransform());

	// ���b�V��
	for (UStaticMesh* Mesh : Meshes)
	{
		NewActor->AddMesh(Mesh);
	}

	// �}�e���A��
	NewActor->SetMaterialColor(ImageBaseColor);
	NewActor->SetMaterialMetallic(ImageMetallic);
	NewActor->SetMaterialRoughness(ImageRoughness);
	NewActor->SetMaterialOpacity(ImageOpacity);

	return NewActor;
}

