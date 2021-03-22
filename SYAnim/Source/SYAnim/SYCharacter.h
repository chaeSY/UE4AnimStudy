// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SYCharacter.generated.h"

UCLASS()
class SYANIM_API ASYCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASYCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveRight(float Value);
	void MoveForward(float Value);
	
	void InputMouseX(float Value);
	void InputMouseY(float Value);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerCameraManager* CameraManager;

private:
	void OnMouseLButtonDown();
	void OnMouseLButtonUp();

	// cam rotate
	bool bMouseLButtonPressed = false;
	FVector2D MousePosBeforePressed = { 0.f, 0.f };

	// springarm zoom in / zoom out
	void OnMouseWheelUp();
	void OnMouseWheelDown();

	enum {
		SpringArmLengthMin = 100,
		SpringArmLengthMax = 400
	};

	float TargetSpringArmLength = 0.f;

	// toggle lookat cam
	void InputXButtonDown();

public:
	bool bLookAtCam; // temp
	bool IsLookAtCam();
	FRotator LookAtCam;
	FRotator OriginHeadRot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookAtCamSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookAtCamLimitYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookAtCamLimitPitch;
	
	float LookAtCamLimitYawCos;
	float LookAtCamLimitPitchCos;
};
