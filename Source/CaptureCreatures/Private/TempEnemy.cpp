#include "TempEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

ATempEnemy::ATempEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	// 当たり判定（コンポーネント自体はC++で固定）
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComponent;

	// 見た目（詳細な見た目調整はBPで実施）
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ApplyTunableSettings();
}

void ATempEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyTunableSettings();
}

void ATempEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	bCaptured = false;
	ApplyTunableSettings();

	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ATempEnemy::OnCollisionHit);
	}
}

void ATempEnemy::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Cyan,
			FString::Printf(TEXT("Enemy Collision Hit: %s"), *OtherActor->GetName())
		);
	}
}

/// <summary>/ 捕獲成功率を計算する関数。現在のHPと最大HP、BPで調整する基本捕獲成功率と低HP捕獲ボーナスを使用して計算する。</summary>
float ATempEnemy::CalculateCaptureChance() const
{
	if (MaxHP <= KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}

	const float HpRatio = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
	const float MissingHpRatio = 1.0f - HpRatio;

	return FMath::Clamp(BaseCaptureChance + (MissingHpRatio * LowHpCaptureBonus), 0.0f, 1.0f);
}

bool ATempEnemy::TryCaptureByChance()
{
	if (MaxHP <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryCaptureByChance called but MaxHP is zero or negative."));
		return false;
	}

	const float HpRatio = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);

	if (HpRatio > CaptureAllowedHpRatio)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Red,
				FString::Printf(
					TEXT("Capture failed: HP ratio %.2f is above the allowed threshold %.2f."), HpRatio,
					CaptureAllowedHpRatio)
			);
		}

		UE_LOG(LogTemp, Log, TEXT("Capture failed by HP gate on %s. HpRatio=%.2f Gate=%.2f"),
		       *GetName(), HpRatio, CaptureAllowedHpRatio);
		return false;
	}

	const float CaptureChance = CalculateCaptureChance();
	const float Roll = FMath::FRandRange(0.0f, 1.0f);
	const bool bSuccess = Roll <= CaptureChance;


	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.8f,
			bSuccess ? FColor::Green : FColor::Red,
			FString::Printf(TEXT("Capture roll: %.2f / %.2f (%s)"),
			                Roll,
			                CaptureChance,
			                bSuccess ? TEXT("SUCCESS") : TEXT("FAIL"))
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Capture roll on %s. HpRatio=%.2f Chance=%.2f Roll=%.2f Result=%s"),
	       *GetName(), HpRatio, CaptureChance, Roll, bSuccess ? TEXT("SUCCESS") : TEXT("FAIL"));

	if (bSuccess)
	{
		Capture();
		return true;
	}

	return false;
}

void ATempEnemy::ApplyTunableSettings()
{
	if (!CollisionComponent)
	{
		return;
	}

	CollisionComponent->SetSphereRadius(CollisionRadius);
	CollisionComponent->SetCollisionProfileName(CollisionProfileName);
	CollisionComponent->SetCollisionObjectType(CollisionObjectType);
}

/// <summary>/ Projectileから呼び出される関数。捕獲モードかどうかと、ダメージ量を受け取る。</summary>
void ATempEnemy::NotifyHitByProjectile(bool bCaptureMode, float DamageAmount)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(
				TEXT("Enemy hit! CaptureMode: %s, EnemyHP: %.1f"), bCaptureMode ? TEXT("ON") : TEXT("OFF"), CurrentHP)
		);
	}

	if (bCaptured)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Blue,
				TEXT("Already captured, no effect.")
			);
		}
		return;
	}

	if (bCaptureMode)
	{
		TryCaptureByChance();
		return;
	}

	CurrentHP = FMath::Max(0.0f, CurrentHP - DamageAmount);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Enemy HP: %.1f / %.1f"), CurrentHP, MaxHP)
		);
	}
}

/// <summary>/ 捕獲処理。捕獲フラグを立てて、画面表示して、アクターを削除する。</summary>
void ATempEnemy::Capture()
{
	if (bCaptured)
	{
		return;
	}

	bCaptured = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Green,
			TEXT("Captured!")
		);
	}

	Destroy();
}
