#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class CAPTURECREATURES_API ACaptureProjectile : public AActor
{
	GENERATED_BODY()

public:
	ACaptureProjectile();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 発射処理
	void Launch(FVector Direction, float Speed);

	// キャプチャモードフラグ
	UPROPERTY(BlueprintReadWrite, Category = "Capture")
	bool bIsCaptureMode;

protected:
	// 衝突判定用
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* CollisionComponent;

	//移動コンポーネント
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;

	//ライフスパン（秒）
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	float LifeSpan = 10.0f;

	// 衝突時の処理
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

public:
	// Called every frame
};
