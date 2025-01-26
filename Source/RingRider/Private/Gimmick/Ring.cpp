// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Ring.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Interface/Moveable.h"


const FString ARing::TARGET_POSITION = FString("TargetPosition");


ARing::ARing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring Mesh"));
	RootComponent = MeshComp;

	PassCheckComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pass Check"));
	PassCheckComp->SetupAttachment(RootComponent);
	PassCheckComp->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnOverlapBegin);

	ObtainComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Energy Obtain Effect"));
	ObtainComp->SetupAttachment(RootComponent);


	// ===== Collision ===== //
	const float RingRadius = 90.f;
	const FVector BoxScale(0.55f, 0.45f, 1.8f);
	for (int k = 0; k < VertNum; k++)
	{
		FName CompName(*FString::Printf(TEXT("Collider%d"), k));
		UBoxComponent* BoxComp = CreateDefaultSubobject<UBoxComponent>(CompName);
		BoxComp->SetupAttachment(RootComponent);

		// Transform
		float Angle = (360.f / VertNum) * k;
		float Sin, Cos;
		FMath::SinCos(&Sin, &Cos, FMath::DegreesToRadians(Angle));
		float LocX = RingRadius * Cos;
		float LocY = RingRadius * Sin;
		BoxComp->SetRelativeLocation(FVector(0.f, LocX, LocY));
		BoxComp->SetRelativeRotation(FRotator(0.f, 0.f, -Angle));
		BoxComp->SetRelativeScale3D(BoxScale);

		ColComps.Add(BoxComp);
	}
}



void ARing::BeginPlay()
{
	Super::BeginPlay();

	ObtainComp->Deactivate();
}



void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPassed)
	{
		// 時間計測 & カーブのサンプル
		AnimTimer += DeltaTime;
		float TimeRatio = AnimTimer / AnimDuration;
		TimeRatio = FMath::Clamp(TimeRatio, 0.f, 1.f);
		float CurveVal = AnimCurve->GetFloatValue(TimeRatio);	// 0 -> 1

		// リングをスピン
		float DeltaAngle = RotateSpeed * DeltaTime;
		AddActorLocalRotation(FRotator(0.f, 0.f, DeltaAngle));

		// リングを縮小
		float S = StartScale * (1 - CurveVal);
		SetActorScale3D(FVector(S,S,S));

		// ライダーを追尾
		FVector RiderPos = PassedActor->GetActorLocation();
		FVector DiffPos = RiderPos - GetActorLocation();
		FVector DeltaPos = DiffPos * CurveVal;
		AddActorWorldOffset(DeltaPos);
		
		// パーティクルもライダーを追尾
		ObtainComp->SetNiagaraVariableVec3(TARGET_POSITION, RiderPos);

		if (TimeRatio >= 1.f)
		{
			Destroy();
		}
	}
}



// Overlap Events //////////////////////////////////////////////////////////////////////////////
void ARing::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherbodyIndex,
	bool bFromSweep,
	const FHitResult& Hit
)
{
	if (bIsPassed)
		return;

	OnActorPassed(OtherActor);
}



// Rider Pass Events /////////////////////////////////////////////////////////////////////////
void ARing::OnActorPassed(AActor* _PassedActor)
{
	if (!_PassedActor)
		return;
	
	IEnergy* PassedIEnergy = Cast<IEnergy>(_PassedActor);
	if (!PassedIEnergy)
		return;

	bIsPassed = true;
	PassedActor = _PassedActor;
	GiveEnergy(PassedIEnergy, Energy);

	// リング生成時にスケールが徐々に大きくなる演出があるので、BeginPlayでなく、ここでリングのスケールを取得
	StartScale = GetActorScale().X;

	// ライダーが吹っ飛んでしまうので、コライダーはOFF
	for (int k = 0; k < VertNum; k++)
	{
		ColComps[k]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// VFX
	ObtainComp->Activate(true);

	// IMoveableを実装していれば加速させる
	if (IMoveable* PassedIMoveable = Cast<IMoveable>(_PassedActor))
		PassedIMoveable->AddSpeed(SpeedBoostOnPassed);
}


