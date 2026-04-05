#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "TempEnemy.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class CAPTURECREATURES_API ATempEnemy : public AActor
{
	GENERATED_BODY()

public:
	ATempEnemy();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	// Projectileから呼び出される関数。捕獲モードかどうかと、ダメージ量を受け取る。
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void NotifyHitByProjectile(bool bCaptureMode, float DamageAmount);

	/// <summary>/ 捕獲処理。現在はHPを0にして捕獲フラグを立てるだけ。将来的には捕獲アニメーションやエフェクトもここで実装予定。</summary>
	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsCaptured() const { return bCaptured; }

protected:
	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    FVector NormalImpulse, const FHitResult& Hit);

	// C++で所有する当たり判定。サイズや見た目合わせはBP子クラスで調整する。
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	USphereComponent* CollisionComponent;

	/// <summary>/ C++で所有する見た目。詳細な見た目調整はBP子クラスで実施する。</summary>
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	UStaticMeshComponent* MeshComponent;

	// BPで調整する当たり判定半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Collision")
	float CollisionRadius = 40.0f;

	// BPで調整するコリジョンプロファイル名（例: Pawn, BlockAllDynamic）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Collision")
	FName CollisionProfileName = TEXT("Pawn");

	// BPで調整するObject Type。Collision Preset適用後に最終的に上書きする。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Collision")
	TEnumAsByte<ECollisionChannel> CollisionObjectType = ECC_Pawn;

	/// <summary>/ BPで調整する最大HP。捕獲条件やダメージ処理に使用する予定。</summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	float MaxHP = 30.0f;

	/// <summary>/ 現在のHP。</summary>
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	float CurrentHP = 30.0f;

	/// <summary>/ BPで調整する捕獲可能HP割合。現在のHPがこの割合以下のときに捕獲可能になる予定。</summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ENemy|Capture", meta=(ClampMin="0.0", ClampMax="1.0"))
	float CaptureAllowedHpRatio = 0.5f;

	/// <summary>/ BPで調整する基本捕獲成功率。捕獲可能HP割合以下のときにこの確率で捕獲成功になる予定。</summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy|Capture", meta=(ClampMin="0.0", ClampMax="1.0"))
	float BaseCaptureChance = 0.35f;

	/// <summary>/ BPで調整する低HP捕獲ボーナス。現在のHPが捕獲可能HP割合の半分以下のときに、基本捕獲成功率にこの値を加算する予定。</summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy|Capture", meta=(ClampMin="0.0", ClampMax="1.0"))
	float LowHpCaptureBonus = 0.55f;

	/// <summary>/ 捕獲状態フラグ。</summary>
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|State")
	bool bCaptured = false;

	void ApplyTunableSettings();
	void Capture();
};
