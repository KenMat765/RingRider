// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/ComponentUtility.h"
#include "Components/ChildActorComponent.h"


namespace FComponentUtility
{
	FTimerHandle IgnoreCollisionTemporary(
		FTimerManager& _TimerManager,
		UPrimitiveComponent* _Primitive,
		ECollisionChannel _IgnoreChannel,
		float _IgnoreSeconds)
	{
		if (_IgnoreSeconds <= 0.f)
		{
			UE_LOG(LogTemp, Error, TEXT("IgnoreCollisionTemporary: 0 or minus value of _IgnoreSeconds is not allowed"));
			return FTimerHandle();
		}

		// 指定したチャンネルとのコリジョンを無効化
		ECollisionResponse DefaultResponse = _Primitive->GetCollisionResponseToChannel(_IgnoreChannel);
		_Primitive->SetCollisionResponseToChannel(_IgnoreChannel, ECR_Ignore);

		// 一定時間後にコリジョン設定を戻す
		FTimerHandle TimerHandle;
		FTimerDelegate RestoreCollisionDelegate = FTimerDelegate::CreateLambda([_Primitive, _IgnoreChannel, DefaultResponse](){
			_Primitive->SetCollisionResponseToChannel(_IgnoreChannel, DefaultResponse);
			});
		_TimerManager.SetTimer(TimerHandle, RestoreCollisionDelegate, _IgnoreSeconds, false);
		return TimerHandle;
	}
}
