#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ACaptureProjectile;

UCLASS()
class CAPTURECREATURES_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// カメラ
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;

	// 入力
	UPROPERTY(EditAnywhere)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere)
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere)
	UInputAction* CaptureAction;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ACaptureProjectile> ProjectileClass;

	// キャプチャモード（true = 網、false = 通常弾）
	UPROPERTY(BlueprintReadWrite, Category = "Weapon", meta=(AllowPrivateAccess="true"))
	bool bCaptureMode = false;

	// 関数
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void Shoot(const struct FInputActionValue& Value);
	void Capture(const struct FInputActionValue& Value);

	//ヘルパー関数。
	void SpawnProjectile();
};
