#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"


// 세션 검색 완료시 호출될 함수 등록하는 델리게이트
DECLARE_DELEGATE_TwoParams(/*Delegate 자료형*/FFindComplete, int32, FString);
/*
 *	int32 == -1, Session Search Complete
 *
 */



UCLASS()
class NETTPS_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//	Begin Play
	virtual void Init() override;

	//	세션 조회 완료시 세션 갯수만큼 호출하는 Delegate
	FFindComplete OnFindComplete;
	

	//	세션의 모든 처리를 진행하는 객체
	IOnlineSessionPtr SessionInterface;

	//	세션 생성 관련
	//	현재 세션 이름
	FName CurrentSessionName;

	//	세션 생성 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString DisplayName, int32 PlayerCount);

	//	세션 생성 완료 함수
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	//	세션 조회
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UFUNCTION(BlueprintCallable)
	void FindOtherSession();

	void OnFindSessionComplete(bool bWasSuccessful);

	//	세션 참여 관련
	//	세션 참여 함수
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 SessionIdx);
	
	//	세션 참여 완료 함수
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
};
