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
	void MoveRight(float Value);
	void MoveForward(float Value);

	void InputMouseX(float Value);
	void InputMouseY(float Value);

	void OnMouseLButtonDown();
	void OnMouseLButtonUp();
	void OnMouseWheelUp();
	void OnMouseWheelDown();

	// cam rotate
	bool bMouseLButtonPressed = false;
	FVector2D MousePosBeforePressed = { 0.f, 0.f };

	// springarm zoom in / zoom out
	enum {
		SpringArmLengthMin = 100,
		SpringArmLengthMax = 400
	};

	float DesiredSpringArmLength = 0.f;

	// look at cam


	// test key 
	void InputXButtonDown();

public:
	bool IsLookAtCam();
	float GetLookAtCamSpeed();
	//FRotator WorldHeadRotationToCamera;
	FRotator GetWorldHeadRotationToCamera();

private:
	UPROPERTY(Category = "LookAtCamera", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bLookAtCam;

	UPROPERTY(Category = "LookAtCamera", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LookAtCamSpeed;

	UPROPERTY(Category = "LookAtCamera", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LookAtCamLimitYawDegree;

	UPROPERTY(Category = "LookAtCamera", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LookAtCamLimitPitchDegree;
	
	float LookAtCamLimitYawCos;
	float LookAtCamLimitPitchCos;
	FRotator RefPoseHeadBoneRotation;
};
