#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

UCLASS()
class NETTPS_API ANetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ANetActor();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void Tick(float DeltaTime) override;


private:
	void PrintLog();
	
	// Owner 설정
	void FindOwner();

	void Rotate();

	void ChangeColor();

	void ChangeScale();

	UFUNCTION()
	void OnRep_Rotate();

	UFUNCTION()	// MColor 변경시 호출
	void OnRep_Color();

	// Request to Server
	UFUNCTION(Server, Reliable)
	void ServerRPC_Scale();

	UFUNCTION(Client, Reliable)
	void ClientRPC_Scale(float Scale);

	// All Client Req
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Scale(float Scale);
	

public:

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	// Owner 검색범위
	float FindDistance = 300.f;

	// 회전값 동기화용
	UPROPERTY(ReplicatedUsing=OnRep_Rotate)
	float RotYaw;

	UPROPERTY()
	class UMaterialInstanceDynamic* Material;

	// Material Color Sync Var
	UPROPERTY(ReplicatedUsing=OnRep_Color)
	FLinearColor MColor;

};
