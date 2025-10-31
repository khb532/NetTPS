// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossAnim.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UBossAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;



public:
	//	Own Pawn
	UPROPERTY()
	class ABoss* p_PawnOwner;

	//	Boss State
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBossState e_CurrState;

	//	Boss in Air flag
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b_IsAir;
};
