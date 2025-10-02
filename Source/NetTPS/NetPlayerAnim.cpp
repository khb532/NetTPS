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


	if (PawnOwner != nullptr)
	{
		// 수평이속 설정
		GroundSpeed = Velocity.Size2D();

		// 이동속도 설정
		Velocity = PawnOwner->GetVelocity();
	
		// 공중여부 설정
		isAir = PawnOwner->GetMovementComponent()->IsFalling();
		
		isArmed = PawnOwner->GetHasGun();
		if (isArmed)
		{
			FVector tmpV = Velocity;
			tmpV.Z = 0.f;
			
			DirV = FVector::DotProduct(PawnOwner->GetActorForwardVector(), tmpV);
			DirH = FVector::DotProduct(PawnOwner->GetActorRightVector(), tmpV);
		}

		// 무장여부 pitch yaw angle 설정
		PitchAngle = isArmed ? -PawnOwner->GetBaseAimRotation().Pitch : 0;
		YawAngle = isArmed ? 20 : 0;
	}

}

void UNetPlayerAnim::AnimNotify_OnReloadComplete()
{
	if (IsValid(PawnOwner))
	{
		PawnOwner->OnReloadComplete();
	}
}
