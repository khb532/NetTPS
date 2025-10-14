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

public:

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	// Owner 검색범위
	float FindDistance = 300.f;

	// 회전값 동기화용
	UPROPERTY(Replicated)
	float RotYaw;

};
