// Fill out your copyright notice in the Description page of Project Settings.


#include "SYAnimInstance.h"
#include "SYCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USYAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ASYCharacter* Character = Cast<ASYCharacter>(TryGetPawnOwner());
	if (Character && Character->IsValidLowLevel())
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh())
		{
			if (SkeletalMeshComponent->SkeletalMesh)
			{
				int HeadBoneIndex = SkeletalMeshComponent->GetBoneIndex(TEXT("Head"));
				FMatrix HeadBoneMatrix = SkeletalMeshComponent->SkeletalMesh->GetComposedRefPoseMatrix(HeadBoneIndex);
				WorldHeadRotation = HeadBoneMatrix.Rotator() + SkeletalMeshComponent->GetRelativeRotation();
			}
		}
	}
}

void USYAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ASYCharacter* Character = Cast<ASYCharacter>(TryGetPawnOwner());

	if (Character && Character->IsValidLowLevel())
	{
		// speed / direction
		{
			FVector MovementVector = Character->GetCharacterMovement()->Velocity;
			FRotator CharacterRotator = Character->GetActorRotation();

			Speed = MovementVector.Size();
			Direction = CalculateDirection(MovementVector, CharacterRotator);
		}

		// look at camera
		{
			bLookAtCam = Character->IsLookAtCam();
			if (bLookAtCam)
			{
				float InterpolationSpeed = Character->GetLookAtCamSpeed();
				WorldHeadRotation = FMath::RInterpTo(WorldHeadRotation, Character->GetWorldHeadRotationToCamera(), DeltaSeconds, InterpolationSpeed);
			}
		}

		/*
		// 1. rotator to matrix
		FMatrix RotMatrix = FRotationMatrix(CharacterRotator);
		
		// 2. Matrix to Axis Vector
		// Q. Rotator로 만들어진 Matrix는 normalized가 되어있는가?
		FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
		FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);

		// 3. 
		FVector NormalizedMovementVector = MovementVector.GetSafeNormal();
		//UE_LOG(LogClass, Warning, TEXT("Movement: %s"), *NormalizedMovementVector.ToString());
	
		float deg = 0.f;
		if (!NormalizedMovementVector.IsNearlyZero())
		{
			float cos = FVector::DotProduct(ForwardVector, NormalizedMovementVector);
			deg = FMath::RadiansToDegrees(FMath::Acos(cos));

			float rightCos = FVector::DotProduct(RightVector, NormalizedMovementVector);
			if (rightCos < 0.f)
			{
				deg *= -1.f;
			}

		}
		//UE_LOG(LogClass, Warning, TEXT("Degree: %f"), deg);
		*/
	}
}