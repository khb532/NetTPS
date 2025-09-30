// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnim.h"
#include "NetPlayer.h"
#include "GameFramework/PawnMovementComponent.h"

void UNetPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 소유 pawn을 가져오자
	PawnOwner = Cast<ANetPlayer>(TryGetPawnOwner());
}

void UNetPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 이동속도 설정
	if (PawnOwner != nullptr)
		Velocity = PawnOwner->GetVelocity();
	// 수평이속 설정
	GroundSpeed = Velocity.Size2D();
	// 공중여부 설정
	if (PawnOwner != nullptr)
		isAir = PawnOwner->GetMovementComponent()->IsFalling();
}
