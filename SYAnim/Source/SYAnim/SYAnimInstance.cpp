// Fill out your copyright notice in the Description page of Project Settings.


#include "SYAnimInstance.h"
#include "SYCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USYAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ASYCharacter* ch = Cast<ASYCharacter>(TryGetPawnOwner());

	if (ch && ch->IsValidLowLevel())
	{
		// speed / direction
		{
			FVector MovementVector = ch->GetCharacterMovement()->Velocity;
			FRotator CharacterRotator = ch->GetActorRotation();

			Speed = MovementVector.Size();
			Degree = CalculateDirection(MovementVector, CharacterRotator);
		}

		// look at camera
		{
			bLookAtCam = ch->bLookAtCam;
			if (bLookAtCam)
			{
				LookAtCam = FMath::RInterpTo(LookAtCam, ch->LookAtCam, DeltaSeconds, ch->LookAtCamSpeed);
			}
			else
			{
				LookAtCam = ch->LookAtCam;
			}
		}

		//log
		{
			/*
			if (GEngine)
			{
				GEngine->ClearOnScreenDebugMessages();
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("animinstance look: %s"), *LookAtCam.ToString()), false);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("character look: %s"), *ch->LookAtCam.ToString()), false);
			}
			*/
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