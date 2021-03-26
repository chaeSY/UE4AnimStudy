// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ActionEnd.h"
#include "SYCharacter.h"

void UAnimNotify_ActionEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ASYCharacter* Character = Cast<ASYCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->CurrentAction = nullptr;

		//Character->CanNextAction = true;
	}
}
