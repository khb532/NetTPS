#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class NETTPS_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	AGun();

	virtual void Tick(float DeltaTime) override;

	// 현재 잔탄 반환
	int32 GetBulletCount() { return CurBullet; }

	// 총알 소모
	void PopBullet();

	// 총알 충전
	void FillBullet() { CurBullet = MaxBullet; }

	// 총알이 최대치인지
	bool IsFillBullet() { return CurBullet == MaxBullet; }
	
protected:
	virtual void BeginPlay() override;

private:

	int32 MaxBullet = 10;

	int32 CurBullet;

	

};
