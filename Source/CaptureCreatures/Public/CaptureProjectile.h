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
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 発射処理（速度は弾側の設定値を使用）
	void Launch(FVector Direction);

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

	// BPで調整する弾の半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Tuning")
	float CollisionRadius = 5.0f;

	// BPで調整する初速
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Tuning")
	float ProjectileInitialSpeed = 2000.0f;

	// BPで調整する最高速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Tuning")
	float ProjectileMaxSpeed = 2000.0f;

	// BPで調整するライフスパン（秒）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Tuning")
	float ProjectileLifeSpan = 10.0f;

	// BPで調整するダメージ量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Combat")
	float DamageAmount = 10.0f;

	void ApplyTunableSettings();

	// 衝突時の処理
	//渡された引数は、衝突したコンポーネント、衝突したアクター、衝突したコンポーネント、衝突の法線インパルス、および衝突の詳細を含むHitResult構造体です。
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

public:
	// Called every frame
};
