#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnim.generated.h"


UCLASS()
class NETTPS_API UNetPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

/* Method */
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 재장전 Anim 끝날때 호출될 이벤트함수
	// Notify수신 규칙 : 함수이름
	UFUNCTION()
	void AnimNotify_OnReloadComplete();	// [HEAD]AnimNotify + '_' + AM_PlayerMontage의 추가된 Notify의 이름과 일치해야함.

protected:

private:

/* Field */
public:
	// Animation 소유 pawn
	UPROPERTY()
	class ANetPlayer* PawnOwner;

	// 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;

	// 공중에 있는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool isAir;

	// 수평 이동 속력(스칼라)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GroundSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DirH;	// Horizon 좌우

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DirV;	// Vertical 상하
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool isArmed = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PitchAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float YawAngle;
	
protected:

	
private:

	
};
