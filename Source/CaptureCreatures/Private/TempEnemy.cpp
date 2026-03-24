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
}

void ATempEnemy::ApplyTunableSettings()
{
	if (!CollisionComponent)
	{
		return;
	}

	CollisionComponent->SetSphereRadius(CollisionRadius);
	CollisionComponent->SetCollisionProfileName(CollisionProfileName);
}

/// <summary>/ Projectileから呼び出される関数。捕獲モードかどうかと、ダメージ量を受け取る。</summary>
void ATempEnemy::NotifyHitByProjectile(bool bCaptureMode, float DamageAmount)
{
	if (bCaptured)
	{
		return;
	}

	if (bCaptureMode)
	{
		Capture();
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
