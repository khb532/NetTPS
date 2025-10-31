#include "BossAnim.h"

void UBossAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (p_PawnOwner)
	{
		//	Set State
		e_CurrState = p_PawnOwner->CurrState;

		//	Set isair
		b_IsAir = p_PawnOwner->GetCharacterMovement()->IsFalling();
	}
}
