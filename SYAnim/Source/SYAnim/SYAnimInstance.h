// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SYAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SYANIM_API USYAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Direction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator WorldHeadRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLookAtCam;
};
