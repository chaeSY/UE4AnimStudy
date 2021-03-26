// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_DelayEnd.h"
#include "SYCharacter.h"

void UAnimNotify_DelayEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ASYCharacter* Character = Cast<ASYCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->CanNextAction = true;
	}
}
