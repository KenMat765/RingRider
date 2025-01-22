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

		// �w�肵���`�����l���Ƃ̃R���W�����𖳌���
		ECollisionResponse DefaultResponse = _Primitive->GetCollisionResponseToChannel(_IgnoreChannel);
		_Primitive->SetCollisionResponseToChannel(_IgnoreChannel, ECR_Ignore);

		// ��莞�Ԍ�ɃR���W�����ݒ��߂�
		FTimerHandle TimerHandle;
		FTimerDelegate RestoreCollisionDelegate = FTimerDelegate::CreateLambda([_Primitive, _IgnoreChannel, DefaultResponse](){
			_Primitive->SetCollisionResponseToChannel(_IgnoreChannel, DefaultResponse);
			});
		_TimerManager.SetTimer(TimerHandle, RestoreCollisionDelegate, _IgnoreSeconds, false);
		return TimerHandle;
	}
}
