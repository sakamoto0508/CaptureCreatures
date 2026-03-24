#pragma once

#include "CoreMinimal.h"
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

	/// <summary>/ BPで調整する最大HP。捕獲条件やダメージ処理に使用する予定。</summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	float MaxHP = 30.0f;

	/// <summary>/ 現在のHP。Projectileからのダメージ処理で減少させる予定。HPが0以下になったら捕獲成功とする予定。</summary>
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	float CurrentHP = 30.0f;

	/// <summary>/ 捕獲状態フラグ。Projectileからの処理で捕獲成功したらtrueにする予定。捕獲後はダメージ無効化などの処理もここで制御する予定。</summary>
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|State")
	bool bCaptured = false;

	void ApplyTunableSettings();
	void Capture();
};
