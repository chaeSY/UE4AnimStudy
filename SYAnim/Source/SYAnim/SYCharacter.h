// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SYCharacter.generated.h"


USTRUCT()
struct FAction
{
	GENERATED_USTRUCT_BODY()
	int ActionID;
	FName SectionName;
	int NextActionID;
};

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

private:
	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;

public:
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

	// test key 
	void InputXButtonDown();

public:
	bool IsLookAtCam();
	float GetLookAtCamSpeed();
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


// action
private:
	UPROPERTY(Category = "Animations", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;
	
	// cache
	TMap<int, FAction> ActionMap;
	void InitAction();
	void PlayAction();

public:
	FAction* CurrentAction = nullptr;
	bool CanNextAction = true;
};
