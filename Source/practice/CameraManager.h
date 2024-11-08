// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "CameraManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRACTICE_API ACameraManager : public APawn
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ACameraManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SmoothRotateCamera(float _RotationValue, float DeltaTime, float RotationSpeed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Camera)
	TArray<TSoftObjectPtr<AActor>> Cameras;

	UPROPERTY(EditAnywhere, Category=Camera)
	float RotationValue;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* RotateAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraChange")
	TSubclassOf<AStaticMeshActor> ChangeCameraMeshClass;

	AStaticMeshActor* ChangeCameraMeshInstance; // 기존에 스폰된 액터를 가리킴
	
	APlayerController* PlayerController;
	int32 CurrentCameraIndex;
	AActor* CameraActor;
    FTimerHandle TimerHandle_HideMesh; // 여기에서 선언
	float TargetRotationValue;
	bool RotationFlag;

protected:
	void Equip(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
	void DisplayMeshForCameraChange();
	void HideCameraChangeMesh();
};
