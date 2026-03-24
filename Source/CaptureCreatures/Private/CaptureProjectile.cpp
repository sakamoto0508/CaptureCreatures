#include "CaptureProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TempEnemy.h"

// Sets default values
ACaptureProjectile::ACaptureProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	//コリジョンコンポーネントをルートに設定。
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	RootComponent = CollisionComponent;

	//移動コンポーネント。
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// デフォルト設定。
	bIsCaptureMode = false;

	ApplyTunableSettings();
}

void ACaptureProjectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyTunableSettings();
}

// Called when the game starts or when spawned
void ACaptureProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// 衝突コールバックをバインド。
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ACaptureProjectile::OnHit);
	}

	ApplyTunableSettings();

	// ライフスパン後に自動削除。
	SetLifeSpan(ProjectileLifeSpan);
}

// Called every frame
void ACaptureProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/// <summary>/ 発射処理。方向を受け取り、弾側の速度設定でProjectileMovementComponentに設定する。</summary>
void ACaptureProjectile::Launch(FVector Direction)
{
	if (ProjectileMovement)
	{
		// 方向を正規化
		Direction.Normalize();

		// 速度を設定
		ProjectileMovement->Velocity = Direction * ProjectileInitialSpeed;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Yellow,
				FString::Printf(TEXT("Projectile launched! Mode: %s"), bIsCaptureMode ? TEXT("CAPTURE") : TEXT("DAMAGE"))
			);
		}
	}
}

/// <summary>/ Tunableな設定をコンポーネントに適用する関数。ConstructionScriptとBeginPlayで呼び出される。</summary>
void ACaptureProjectile::ApplyTunableSettings()
{
	if (CollisionComponent)
	{
		CollisionComponent->InitSphereRadius(CollisionRadius);
	}

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = ProjectileInitialSpeed;
		ProjectileMovement->MaxSpeed = ProjectileMaxSpeed;
	}
}

/// <summary>/ 衝突処理。現在はデバッグ出力のみ。将来的には敵の捕獲処理をここに実装予定。</summary>
void ACaptureProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 自分自身との衝突は無視。
	if (OtherActor && OtherActor != this && OtherComp)
	{
		if (ATempEnemy* HitEnemy = Cast<ATempEnemy>(OtherActor))
		{
			HitEnemy->NotifyHitByProjectile(bIsCaptureMode, DamageAmount);
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Green,
				FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName())
			);
		}


		// 弾は衝突後、少ししてから消える（または即削除）
		Destroy();
	}
}

