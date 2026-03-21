#include "CaptureProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ACaptureProjectile::ACaptureProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

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
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// デフォルト設定。
	bIsCaptureMode = false;
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

	// ライフスパン後に自動削除。
	SetLifeSpan(LifeSpan);
}

// Called every frame
void ACaptureProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACaptureProjectile::Launch(FVector Direction, float Speed)
{
	if (ProjectileMovement)
	{
		// 方向を正規化
		Direction.Normalize();

		// 速度を設定
		ProjectileMovement->Velocity = Direction * Speed;

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

void ACaptureProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 自分自身との衝突は無視。
	if (OtherActor && OtherActor != this && OtherComp)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Green,
				FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName())
			);
		}

		// TODO: ここに敵の捕獲処理を実装
		// とりあえず衝突確認用にデバッグ出力

		// 弾は衝突後、少ししてから消える（または即削除）
		Destroy();
	}
}

