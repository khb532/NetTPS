#pragma once

#include "CoreMinimal.h"
#include "NetTPSCharacter.h"
#include "NetPlayer.generated.h"


UCLASS()
class NETTPS_API ANetPlayer : public ANetTPSCharacter
{
	GENERATED_BODY()

/* Method */
public:
	ANetPlayer();
	
	// 총 집기&놓기 함수
	void TakeGun();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_TakeGun();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DetachGun(class AGun* gun);

	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FiringAction(bool bHit, FHitResult hitInfo, int32 combo);

	UFUNCTION(Server, Reliable)
	void ServerRPC_Reload();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Reload();

	UFUNCTION(BlueprintCallable)
	bool GetHasGun(){ return hasGun;}

	void ChangeCameraBoomSetting();

	void OnReloadComplete();

	void FiringAction();
	
	// 콤보가 진행되는지 함수
	void OnCombo();

	// 공격 마무리 (콤보끊김 & 마지막공격)
	void OnFireComplete();

	// 총 탈부착
	UFUNCTION()
	void AttachGun();
	void DettachGun(class AGun* ptr);

	void DieProcess();

	UFUNCTION(Client, Reliable)
	void ClientRPC_OnPossess();

	void MakeCube();

	UFUNCTION(Server, Reliable)
	void ServerRPC_MakeCube();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	
private:
	// Fire & Reload
	void Fire();
	void Reload();
	void DamageProcess(float damage);
	void BillboardHpbar();
	void PrintNetLog();


/*Field*/
public:
	// 총 집기&놓기 InputAction
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* TakeGunAction;

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* ReloadAction;
	
	// 총을 붙일 Component
	UPROPERTY(EditAnywhere)
	class USceneComponent* GunComp;

	// Level에 있는 모든 총을 담을 변수
	UPROPERTY(EditAnywhere)
	TArray<class AActor*> AllGun;

	// 총을 잡을수 있는 허용범위
	UPROPERTY(EditAnywhere)
	float CanTakeGunRange = 200.f;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* PlayerMontage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> MainWidget;

	UPROPERTY()
	class UMainWidget* MainUI;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* HitEffect;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* CompHp;

	UPROPERTY(Replicated)
	bool bCanMakeCube;
	
	// 사망
	bool isDead;
	
	// 공격 시작 여부
	bool isFire;
	// 콤보 동작 여부
	bool isCombo;
	// 콤보 카운트
	int32 ComboCount;
	
	bool bIsDead = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CubeFactory;

protected:
	
private:
	// 가까운 총의 인덱스
	int32 closeidx = -1;

	UPROPERTY(ReplicatedUsing=AttachGun)
	class AGun* OwnGun;

	// 총을 갖고있는지 여부
	bool hasGun = false;

	// 무장, CameraBoom Loc, Target Arm Length
	FVector CameraBoomLocationWithGun = FVector(0, 70, 110);
	float TargetArmLengthWithGun = 160;

	// 비무장
	FVector CameraBoomLocationWithoutGun = FVector(0, 0, 110);
	float TargetArmLengthWithoutGun = 400;

	// Anim재생중 다른Anim Interpt 방지
	bool bReloading = false;


};
