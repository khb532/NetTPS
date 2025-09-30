#pragma once

#include "CoreMinimal.h"
#include "NetTPSCharacter.h"
#include "NetPlayer.generated.h"


UCLASS()
class NETTPS_API ANetPlayer : public ANetTPSCharacter
{
	GENERATED_BODY()

public:
	// 생성자
	ANetPlayer();
	
	// 총 집기&놓기 함수
	void TakeGun();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:


	
	/*////////////////////*/
	
public:
	// 총 집기&놓기 InputAction
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* TakeGunAction;

	// 총을 붙일 Component
	UPROPERTY(EditAnywhere)
	class USceneComponent* GunComp;

	// Level에 있는 모든 총을 담을 변수
	UPROPERTY(EditAnywhere)
	TArray<class AActor*> AllGun;

	// 총을 잡을수 있는 허용범위
	UPROPERTY(EditAnywhere)
	float CanTakeGunRange = 200.f;

	
protected:
	
private:
	
};
