#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "CaptureProjectile.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	// カメラアーム
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// カメラ
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Inputのマッピングコンテキストを追加。
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// 入力バインディング。
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInput->BindAction(ShootAction, ETriggerEvent::Started, this, &APlayerCharacter::Shoot);
		EnhancedInput->BindAction(CaptureAction, ETriggerEvent::Started, this, &APlayerCharacter::Capture);
	}
}

// 移動とカメラ操作の関数。
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Forward, MovementVector.Y);
		AddMovementInput(Right, MovementVector.X);
	}
}

//Look関数は、コントローラーの入力に基づいてキャラクターの視点を回転させます。
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxis.X);
		AddControllerPitchInput(LookAxis.Y);
	}
}

void APlayerCharacter::Shoot(const struct FInputActionValue& Value)
{
	SpawnProjectile();
}

/// <summary>
/// Captureアクションの処理。押すたびに捕獲モードを切り替える。
/// </summary>
void APlayerCharacter::Capture(const struct FInputActionValue& Value)
{
	//押した瞬間にトグル（true/false反転）。
	bCaptureMode = !bCaptureMode;

	// 画面表示で現在モードを確認。
	if (GEngine)
	{
		const FString ModeText = bCaptureMode ? TEXT("CAPTURE MODE: ON") : TEXT("CAPTURE MODE: OFF");

		GEngine->AddOnScreenDebugMessage(
			1, // 固定ID: 同じ行を更新。
			1.5f, // 表示時間。
			bCaptureMode ? FColor::Cyan : FColor::White,
			ModeText
		);
	}
}

/// <summary>/ Projectileをスポーンして、プレイヤー正面方向へ発射する関数。Shootアクションから呼び出される。</summary>
void APlayerCharacter::SpawnProjectile()
{
	// ProjectileClassが設定されているか確認
	if (!ProjectileClass)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Red,
				TEXT("ERROR: ProjectileClass not set!")
			);
		}
		return;
	}

	// スポーン位置と発射方向はプレイヤー正面基準にする
	const FVector LaunchDirection = GetActorForwardVector();
	const FVector SpawnLocation = GetActorLocation() + LaunchDirection * 100.0f + FVector(0.0f, 0.0f, 50.0f);

	// アクターをスポーン
	ACaptureProjectile* Projectile = GetWorld()->SpawnActor<ACaptureProjectile>(
		ProjectileClass, //どのクラス
		SpawnLocation, //どこに
		LaunchDirection.Rotation() //どう向いて
	);

	if (Projectile)
	{
		// モード情報を設定
		Projectile->bIsCaptureMode = bCaptureMode;

		// 発射処理
		Projectile->Launch(LaunchDirection);
	}
}
