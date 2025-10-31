#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReadyPlayer.generated.h"

UCLASS()
class NETTPS_API AReadyPlayer : public AActor
{
	GENERATED_BODY()

public:
	AReadyPlayer();


	virtual void Tick(float DeltaTime) override;

	void SetMesh(int32 idx);
	
protected:
	virtual void BeginPlay() override;



/* Field*/
public:


private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USkeletalMeshComponent> m_Mesh;

	UPROPERTY(EditAnywhere)
	TArray<class USkeletalMesh*> m_arr_PlayerMesh;
};
