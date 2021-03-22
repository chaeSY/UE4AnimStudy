// Fill out your copyright notice in the Description page of Project Settings.


#include "SYCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
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

	//temp
	LookAtCamLimitYaw = 60.f;
	LookAtCamLimitPitch = 60.f;

	LookAtCamLimitYawCos = FMath::Cos(FMath::DegreesToRadians(LookAtCamLimitYaw));
	LookAtCamLimitPitchCos = FMath::Cos(FMath::DegreesToRadians(LookAtCamLimitPitch));

	UE_LOG(LogClass, Warning, TEXT("YawCos: %f"), LookAtCamLimitYawCos);
	UE_LOG(LogClass, Warning, TEXT("PitchCos: %f"), LookAtCamLimitPitchCos);

}

// Called when the game starts or when spawned
void ASYCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetController<APlayerController>();
	check(PlayerController); // todo: check가 어떤 역할을 하는지  분석

	CameraManager = PlayerController->PlayerCameraManager;
	check(CameraManager);

	PlayerController->bShowMouseCursor = true;

	// 앞으로 향하고 있는 head의 Bone을 캐싱
	OriginHeadRot = GetMesh()->GetBoneQuaternion(TEXT("Head"), EBoneSpaces::WorldSpace).Rotator();

	// 
	if (SpringArm)
	{
		TargetSpringArmLength = SpringArm->TargetArmLength;
	}
}

bool ASYCharacter::IsLookAtCam()
{
	//FVector ForwardVector = 
	return false;
}

// Called every frame
void ASYCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//
	float YawCos = 0.f;
	float PitchCos = 0.f;

	FVector CameraPos = CameraManager->GetCameraLocation();
	FVector HeadPos = GetMesh()->GetBoneLocation(TEXT("head"));
	FVector HeadToCamera = CameraPos - HeadPos;

	// get yaw except z 
	{
		FVector ForwardVectorExceptZ = GetActorForwardVector();
		ForwardVectorExceptZ.Z = 0.f;
		ForwardVectorExceptZ.Normalize();

		FVector HeadToCameraExceptZ = HeadToCamera.GetSafeNormal2D();
		YawCos = FVector::DotProduct(ForwardVectorExceptZ, HeadToCameraExceptZ);
	}

	//get pitch except y
	{
		FVector ForwardVectorExceptY = GetActorForwardVector();
		ForwardVectorExceptY.Y = 0.f;
		ForwardVectorExceptY.Normalize();

		FVector HeadToCameraExceptY = HeadToCamera;
		HeadToCameraExceptY.Y = 0.f;
		HeadToCameraExceptY.Normalize();
		
		PitchCos = FVector::DotProduct(ForwardVectorExceptY, HeadToCameraExceptY);
	}

	if (YawCos >= LookAtCamLimitYawCos && PitchCos >= LookAtCamLimitPitchCos)
	{
		bLookAtCam = true;

		// Look At Camera
		HeadToCamera.Normalize();
		LookAtCam = HeadToCamera.Rotation() + OriginHeadRot;
	}
	else
	{
		bLookAtCam = false;

		LookAtCam = GetMesh()->GetBoneQuaternion(TEXT("Head"), EBoneSpaces::WorldSpace).Rotator();
	}

	//SpringArm zoom in/ zoom out
	if (SpringArm)
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetSpringArmLength, GetWorld()->GetDeltaSeconds(), 5.f);
	}

	if (GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Actor Rotation: %s"), *GetActorRotation().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Controller Rotation: %s"), *GetControlRotation().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SpringArm Rotation: %s"), *SpringArm->GetRelativeRotation().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("bLookAtCam: %d"), bLookAtCam), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("HeadRotation: %s"), *GetMesh()->GetBoneQuaternion(TEXT("Head"), EBoneSpaces::WorldSpace).Rotator().ToString()), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("HeadToCameraRotation: %s"), *HeadToCamera.Rotation().ToString()), false);
		//LookAtCam = GetMesh()->GetBoneQuaternion(TEXT("Head"), EBoneSpaces::WorldSpace).Rotator();

	}
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

const static float lengthTick = 80.f;
void ASYCharacter::OnMouseWheelUp()
{
	if (SpringArm)
	{
		float l = SpringArm->TargetArmLength - lengthTick;
		float r = SpringArmLengthMin;
		TargetSpringArmLength = FMath::Max(l, r);
	}
}

void ASYCharacter::OnMouseWheelDown()
{
	if (SpringArm)
	{
		float l = SpringArm->TargetArmLength + lengthTick;
		float r = SpringArmLengthMax;
		TargetSpringArmLength = FMath::Max(l, r);
	}
}