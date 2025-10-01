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
