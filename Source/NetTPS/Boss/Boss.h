#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

// 보스 상태 정의
UENUM(BlueprintType)
enum class EBossState : uint8
{
	IDLE,
	ATTACK_JUMP
};

UCLASS()
class NETTPS_API ABoss : public ACharacter
{
	GENERATED_BODY()
	
/* Method */
public:
	ABoss();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;





private:
	
	
	



/* Field */
public:
	EBossState e_CurrState;
	
	
	
};
