// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

private:

/*//////////////////////*/
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
	
	
protected:

private:
	
};
