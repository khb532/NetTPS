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

	UFUNCTION(BlueprintCallable)
	bool GetHasGun(){ return hasGun;}

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:
	// 총 탈부착
	void AttachGun();
	void DettachGun(class AGun* ptr);
	
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

	void ChangeCameraBoomSetting();
	
protected:
	
private:
	// 가까운 총의 인덱스
	int32 closeidx = -1;

	UPROPERTY()
	class AGun* OwnGun;

	// 총을 갖고있는지 여부
	bool hasGun = false;

	// 무장, CameraBoom Loc, Target Arm Length
	FVector CameraBoomLocationWithGun = FVector(0, 70, 110);
	float TargetArmLengthWithGun = 160;

	// 비무장
	FVector CameraBoomLocationWithoutGun = FVector(0, 0, 110);
	float TargetArmLengthWithoutGun = 400;
};
