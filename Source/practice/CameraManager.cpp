// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/StaticMeshActor.h"

// Sets default values for this component's properties
ACameraManager::ACameraManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ACameraManager::BeginPlay()
{
	Super::BeginPlay();
	RotationFlag = false;

	if (ChangeCameraMeshClass != nullptr)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        ChangeCameraMeshInstance = GetWorld()->SpawnActor<AStaticMeshActor>(ChangeCameraMeshClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

         if (ChangeCameraMeshInstance)
        {
            // 처음에는 보이지 않도록 숨김
            ChangeCameraMeshInstance->SetActorHiddenInGame(true);
        } 
    }

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    SubSystem->AddMappingContext(DefaultContext, 0);
    
	CurrentCameraIndex = 0;
	if (Cameras.Num() > 0)
    {
		CameraActor = Cameras[CurrentCameraIndex].Get();
        PlayerController->SetViewTarget(CameraActor);
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Cameras array is empty!"));
    }

}


// Called every frame
void ACameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(RotationFlag)
	{
		SmoothRotateCamera(TargetRotationValue, DeltaTime, 45.0f);
	}
}

void ACameraManager::SmoothRotateCamera(float _RotationValue, float DeltaTime, float RotationSpeed)
{
	if (CameraActor)
    {
        // 현재 카메라 액터의 회전값을 가져옴
        FRotator CurrentRotation = CameraActor->GetActorRotation();

		// 목표 회전 벡터 값을 설정
		FRotator Target = CurrentRotation;
		Target.Yaw = _RotationValue;

		// 카메라의 움직일 방향 결정 (왼쪽, 오른쪽)
		FRotator RotateWithSpeed = CurrentRotation;
		if((CurrentRotation.Yaw - _RotationValue) > 0)
			RotateWithSpeed.Yaw -= RotationSpeed * DeltaTime;
		else
			RotateWithSpeed.Yaw += RotationSpeed * DeltaTime;

        // 새로 계산된 회전값을 카메라 액터에 적용
        CameraActor->SetActorRotation(RotateWithSpeed);

        // 목표 회전에 도달하면 회전 종료
         if (CurrentRotation.Equals(Target, 1.0f))
        {
			RotateWithSpeed.Yaw = Target.Yaw;
			CameraActor->SetActorRotation(RotateWithSpeed);
			RotationFlag = false;
        } 
    }
}


void ACameraManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ACameraManager::Equip);
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ACameraManager::Rotate);
	}
}

void ACameraManager::Equip(const FInputActionValue& Value)
{
	if(RotationFlag)
		return;

	float Result = Value.Get<float>();

	if(Result > 0)
	{
		if(CurrentCameraIndex >= Cameras.Num()-1)
		{
			CurrentCameraIndex = 0;
		}
		else
		{
			CurrentCameraIndex++;
		}
	}
	else if(Result < 0)
	{
		if(CurrentCameraIndex == 0)
		{
			CurrentCameraIndex = Cameras.Num()-1;
		}
		else
		{
			CurrentCameraIndex--;
		}
	}
	CameraActor = Cameras[CurrentCameraIndex].Get();
	PlayerController->SetViewTarget(CameraActor);

	DisplayMeshForCameraChange();

}

void ACameraManager::Rotate(const FInputActionValue& Value)
{
	if(RotationFlag)
		return;

	FVector2D Input = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Display, TEXT("%f %f"), Input.X, Input.Y);

	if(Input.X > 0)
	{
		TargetRotationValue = RotationValue;
	}
	else if(Input.X < 0)
	{
		TargetRotationValue = -RotationValue;
	}
	else if(Input.Y < 0)
	{
		TargetRotationValue = 0;
	}

	RotationFlag = true;
}

void ACameraManager::DisplayMeshForCameraChange()
{
    if (ChangeCameraMeshInstance)
    {
        // 카메라 앞에 배치 (카메라의 방향과 일치시키도록 설정)
        FVector CameraLocation = CameraActor->GetActorLocation();
        FRotator CameraRotation = CameraActor->GetActorRotation();

        ChangeCameraMeshInstance->SetActorLocation(CameraLocation + CameraRotation.Vector() * 100.0f); // 카메라 앞에 배치
        ChangeCameraMeshInstance->SetActorRotation(CameraRotation); // 카메라와 동일한 방향
        ChangeCameraMeshInstance->SetActorScale3D(FVector(1.0f, 10.0f, 10.0f)); // 화면을 덮도록 크기 설정

        // Mesh를 보이게 설정
        ChangeCameraMeshInstance->SetActorHiddenInGame(false);

        // 1초 후에 Mesh를 다시 숨김
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_HideMesh, this, &ACameraManager::HideCameraChangeMesh, 0.3f, false);
    }
}

void ACameraManager::HideCameraChangeMesh()
{
    if (ChangeCameraMeshInstance)
    {
        // Mesh를 숨김
        ChangeCameraMeshInstance->SetActorHiddenInGame(true);
    }
}

