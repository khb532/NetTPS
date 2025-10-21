#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"


UCLASS()
class NETTPS_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Begin Play
	virtual void Init() override;

	// 세션의 모든 처리를 진행하는 객체
	IOnlineSessionPtr SessionInterface;

	// 세션 생성 관련
	// 현재 세션 이름
	FName CurrentSessionName;

	// 세션 생성 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString DisplayName, int32 PlayerCount);

	// 세션 생성 완료 함수
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// 세션 조회
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UFUNCTION(BlueprintCallable)
	void FindOtherSession();

	void OnFindSessionComplete(bool bWasSuccessful);
	
};
