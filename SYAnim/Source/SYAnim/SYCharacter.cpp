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
#include "Animation/AnimMontage.h"

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

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	check(WeaponMesh);
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("Weapon"));
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

	// action
	InitAction();
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


	// screen debug message
	if (GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();
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

void ASYCharacter::InitAction()
{
	FAction Action1;
	Action1.ActionID = 1;
	Action1.SectionName = TEXT("Attack1");
	Action1.NextActionID = 2;
	

	FAction Action2;
	Action2.ActionID = 2;
	Action2.SectionName = TEXT("Attack2");
	Action2.NextActionID = 3;

	FAction Action3;
	Action3.ActionID = 3;
	Action3.SectionName = TEXT("Attack3");
	Action3.NextActionID = 0;

	ActionMap.Add(Action1.ActionID, Action1);
	ActionMap.Add(Action2.ActionID, Action2);
	ActionMap.Add(Action3.ActionID, Action3);
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
	PlayerInputComponent->BindAction(TEXT("MouseWheelUp"), IE_Released, this, &ASYCharacter::OnMouseWheelUp);
	PlayerInputComponent->BindAction(TEXT("MouseWheelDown"), IE_Released, this, &ASYCharacter::OnMouseWheelDown);
	PlayerInputComponent->BindAction(TEXT("X"), IE_Pressed, this, &ASYCharacter::InputXButtonDown);
}

void ASYCharacter::InputXButtonDown()
{
	PlayAction();
}

void ASYCharacter::MoveRight(float Value)
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance())
		return;

	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
	{
		float speed = 1.f; //temp
		AddMovementInput(GetActorRightVector(), Value * speed);
	}
}

void ASYCharacter::MoveForward(float Value)
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance())
		return;

	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
	{
		float speed = 1.f; //temp
		AddMovementInput(GetActorForwardVector(), Value * speed);
	}
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

void ASYCharacter::PlayAction()
{
	if (!CanNextAction)
		return;

	if (CurrentAction)
	{
		CurrentAction = ActionMap.Find(CurrentAction->NextActionID);
		if (CurrentAction)
		{
			CanNextAction = false;
			PlayAnimMontage(AttackMontage, 1.0f, CurrentAction->SectionName);
		}
	}
	else //root action
	{
		CurrentAction = ActionMap.Find(1);
		if (CurrentAction)
		{
			CanNextAction = false;
			PlayAnimMontage(AttackMontage, 1.0f, CurrentAction->SectionName);
		}
	}
}