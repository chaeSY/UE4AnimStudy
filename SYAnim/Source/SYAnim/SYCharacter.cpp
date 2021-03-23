// Fill out your copyright notice in the Description page of Project Settings.


#include "SYCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

// Sets default values
ASYCharacter::ASYCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// location and rotation
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -86.f), FRotator(0, -90.f, 0));

	// spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	check(SpringArm);
	SpringArm->SetupAttachment(GetCapsuleComponent());
	
	// camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	check(Camera);
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ASYCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetController<APlayerController>();
	check(PlayerController); // todo: check가 어떤 역할을 하는지 분석

	CameraManager = PlayerController->PlayerCameraManager;
	check(CameraManager);

	PlayerController->bShowMouseCursor = true;

	// look at camera
	{
		LookAtCamLimitYawCos = FMath::Cos(FMath::DegreesToRadians(LookAtCamLimitYawDegree));
		LookAtCamLimitPitchCos = FMath::Cos(FMath::DegreesToRadians(LookAtCamLimitPitchDegree));

		// cache refernce pose head rotation
		if (GetMesh() && GetMesh()->SkeletalMesh)
		{
			int HeadBoneIndex = GetMesh()->GetBoneIndex(TEXT("Head"));
			FMatrix HeadBoneMatrix = GetMesh()->SkeletalMesh->GetComposedRefPoseMatrix(HeadBoneIndex);
			RefPoseHeadBoneRotation = HeadBoneMatrix.Rotator() + GetMesh()->GetRelativeRotation();

			UE_LOG(LogClass, Warning, TEXT("head bone rotation: %s"), *RefPoseHeadBoneRotation.ToString());
		}
	}

	// spring arm
	if (SpringArm)
	{
		DesiredSpringArmLength = SpringArm->TargetArmLength;
	}
}

float ASYCharacter::GetLookAtCamSpeed()
{
	return LookAtCamSpeed;
}

bool ASYCharacter::IsLookAtCam()
{
	if (!bLookAtCam)
		return false;

	if (!CameraManager)
		return false;

	FVector CameraPos = CameraManager->GetCameraLocation();
	FVector HeadPos = GetMesh()->GetBoneLocation(TEXT("head"));
	FVector HeadToCamera = CameraPos - HeadPos;

	// get yaw cos
	FVector ForwardVectorExceptZ = GetActorForwardVector();
	ForwardVectorExceptZ.Z = 0.f;
	ForwardVectorExceptZ.Normalize();

	FVector HeadToCameraExceptZ = HeadToCamera.GetSafeNormal2D();
	float YawCos = FVector::DotProduct(ForwardVectorExceptZ, HeadToCameraExceptZ);
	
	if (YawCos < LookAtCamLimitYawCos)
		return false;

	// get pitch cos
	FVector ForwardVectorExceptY = GetActorForwardVector();
	ForwardVectorExceptY.Y = 0.f;
	ForwardVectorExceptY.Normalize();

	FVector HeadToCameraExceptY = HeadToCamera;
	HeadToCameraExceptY.Y = 0.f;
	HeadToCameraExceptY.Normalize();
	float PitchCos = FVector::DotProduct(ForwardVectorExceptY, HeadToCameraExceptY);

	if (PitchCos < LookAtCamLimitPitchCos)
		return false;

	return true;
}

// Called every frame
void ASYCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SpringArm zoom in/ zoom out
	if (SpringArm)
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, DesiredSpringArmLength, GetWorld()->GetDeltaSeconds(), 5.f);
	}

	if (GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Actor Rotation: %s"), *GetActorRotation().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Controller Rotation: %s"), *GetControlRotation().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SpringArm Rotation: %s"), *SpringArm->GetRelativeRotation().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("HeadRotation: %s"), *GetMesh()->GetBoneQuaternion(TEXT("Head"), EBoneSpaces::WorldSpace).Rotator().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("lookat: %d"), IsLookAtCam()), false);
		//LookAtCam = GetMesh()->GetBoneQuaternion(TEXT("Head"), EBoneSpaces::WorldSpace).Rotator();
	}
}

FRotator ASYCharacter::GetWorldHeadRotationToCamera()
{
	if (GetMesh() && CameraManager)
	{
		FVector CameraPos = CameraManager->GetCameraLocation();
		FVector HeadPos = GetMesh()->GetBoneLocation(TEXT("head"));
		FVector HeadToCamera = CameraPos - HeadPos;
		return RefPoseHeadBoneRotation + HeadToCamera.Rotation();
	}

	return FRotator();
}


// Called to bind functionality to input
void ASYCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ASYCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &ASYCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MouseX"), this, &ASYCharacter::InputMouseX);
	PlayerInputComponent->BindAxis(TEXT("MouseY"), this, &ASYCharacter::InputMouseY);

	PlayerInputComponent->BindAction(TEXT("MouseLButton"), IE_Pressed, this, &ASYCharacter::OnMouseLButtonDown);
	PlayerInputComponent->BindAction(TEXT("MouseLButton"), IE_Released, this, &ASYCharacter::OnMouseLButtonUp);
	PlayerInputComponent->BindAction(TEXT("X"), IE_Pressed, this, &ASYCharacter::InputXButtonDown);

	PlayerInputComponent->BindAction(TEXT("MouseWheelUp"), IE_Released, this, &ASYCharacter::OnMouseWheelUp);
	PlayerInputComponent->BindAction(TEXT("MouseWheelDown"), IE_Released, this, &ASYCharacter::OnMouseWheelDown);
}

void ASYCharacter::InputXButtonDown()
{
	bLookAtCam = !bLookAtCam;
}

void ASYCharacter::MoveRight(float Value)
{
	float speed = 1.f; //temp
	AddMovementInput(GetActorRightVector(), Value * speed);
}

void ASYCharacter::MoveForward(float Value)
{
	float speed = 1.f; //temp
	AddMovementInput(GetActorForwardVector(), Value * speed);
}

void ASYCharacter::InputMouseX(float Value)
{
	if (Value != 0.f)
	{
		if (bMouseLButtonPressed)
		{
			if (APlayerController* PC = GetController<APlayerController>())
			{
				PC->bShowMouseCursor = false;

				if (!MousePosBeforePressed.IsNearlyZero())
				{
					PC->SetMouseLocation(MousePosBeforePressed.X, MousePosBeforePressed.Y);
				}
			}

			if (SpringArm)
			{
				float XRate = 1.f;
				SpringArm->AddRelativeRotation(FRotator(0, Value * XRate, 0));
			}
		}
	}
}

void ASYCharacter::InputMouseY(float Value)
{
	if (Value != 0.f)
	{
		if (bMouseLButtonPressed)
		{
			if (APlayerController* PC = GetController<APlayerController>())
			{
				PC->bShowMouseCursor = false;

				if (!MousePosBeforePressed.IsNearlyZero())
				{
					PC->SetMouseLocation(MousePosBeforePressed.X, MousePosBeforePressed.Y);
				}
			}

			if (SpringArm)
			{
				float YRate = 1.f;
				SpringArm->AddRelativeRotation(FRotator(Value * YRate, 0, 0));
			}
		}
	}
}

void ASYCharacter::OnMouseLButtonDown()
{
	bMouseLButtonPressed = true;

	if (APlayerController* PC = GetController<APlayerController>())
	{
		PC->GetMousePosition(MousePosBeforePressed.X, MousePosBeforePressed.Y);
	}
}

void ASYCharacter::OnMouseLButtonUp()
{
	bMouseLButtonPressed = false;

	if (APlayerController* PC = GetController<APlayerController>())
	{
		MousePosBeforePressed = FVector2D::ZeroVector;
		PC->bShowMouseCursor = true;
	}
}

const static float lengthTick = 80.f; //temp
void ASYCharacter::OnMouseWheelUp()
{
	if (SpringArm)
	{
		float l = SpringArm->TargetArmLength - lengthTick;
		float r = SpringArmLengthMin;
		DesiredSpringArmLength = FMath::Max(l, r);
	}
}

void ASYCharacter::OnMouseWheelDown()
{
	if (SpringArm)
	{
		float l = SpringArm->TargetArmLength + lengthTick;
		float r = SpringArmLengthMax;
		DesiredSpringArmLength = FMath::Min(l, r);
	}
}