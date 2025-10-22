#include "NetGameInstance.h"
#include "OnlineSubsystem.h"		// IOnlineSubsystem*
#include "OnlineSubsystemUtils.h"	// Online::
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void UNetGameInstance::Init()
{
	Super::Init();

	// 현재 사용중인 서브시스템 로드
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());

	if (Subsystem)
	{
		// Subsystem의 Interface 로드
		SessionInterface = Subsystem->GetSessionInterface();

		// 세션 생성 성공시 호출 함수 등록
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);

		// 세션 조회 성공시 호출 함수 등록
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionComplete);

		// 세션 참여 성공시 호출 함수 등록
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnJoinSessionComplete);
	}
}

void UNetGameInstance::CreateMySession(FString DisplayName, int32 PlayerCount)
{
	// 세션 생성 위한 옵션
	FOnlineSessionSettings SessionSettings;

	// 현재 사용중인 서브시스템 이름
	FName SubsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();

	// 서브시스템 이름이 Null이면 Lan이용
	SessionSettings.bIsLANMatch = SubsysName.IsEqual(FName(TEXT("NULL")));

	// Steam 에서 필수
	// Lobby 사용 여부
	SessionSettings.bUseLobbiesIfAvailable = true;

	// 친구 접속여부 확인
	SessionSettings.bUsesPresence = true;	// bUseLobbiesIfAvailable 값과 일치해야함

	// 세션 검색 허용 여부
	SessionSettings.bShouldAdvertise = true;

	// 세션 최대 인원
	SessionSettings.NumPublicConnections = PlayerCount;

	// 커스텀 정보
	SessionSettings.Set(FName("DP_NAME"), DisplayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// Settings 로 세션 생성
	FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	
	SessionInterface->CreateSession(*NetID, FName(DisplayName), SessionSettings);
	
}

void UNetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 성공"), *SessionName.ToString());
		GetWorld()->ServerTravel(TEXT("/Game/ThirdPerson/Lvl_ThirdPerson?Listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 실패"), *SessionName.ToString());
	}
}

void UNetGameInstance::FindOtherSession()
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 시작"))

	// session search
	SessionSearch = MakeShared<FOnlineSessionSearch>();

	// 현재 사용중인 서브시스템 이름
	FName SubsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();

	// 서브시스템 이름이 Null이면 Lan이용
	SessionSearch->bIsLanQuery = SubsysName.IsEqual(FName(TEXT("NULL")));

	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	// SessionSearch->QuerySettings.Set(FName("DP_NAME"), FName("Wanted"), EOnlineComparisonOp::Equals);

	// 검색 횟수
	SessionSearch->MaxSearchResults = 100;

	// 세션 검색
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	
}

void UNetGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 끝"))

	if (bWasSuccessful)
	{
		// 검색된 세션 결과들
		TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;

		for (int32 i = 0; i < Results.Num(); i++)
		{
			// 세션 이름
			FString DisplayName;

			Results[i].Session.SessionSettings.Get(FName("DP_NAME"), DisplayName);
			UE_LOG(LogTemp, Warning, TEXT("Session - %i, Name : %s"), i, *DisplayName)

			OnFindComplete.ExecuteIfBound(i, DisplayName/*Delegate parameters*/);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Search FAIL"))
	}
	OnFindComplete.ExecuteIfBound(-1/*Complete*/, FString());
	
}

void UNetGameInstance::JoinOtherSession(int32 SessionIdx)
{
	// 검색된 세션 목록
	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;

	// 5.5 이후 변경됨 : 반드시 세팅 명시해야함
	Results[SessionIdx].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	Results[SessionIdx].Session.SessionSettings.bUsesPresence = true;

	// 세션 이름 로드
	FString DisplayName;
	Results[SessionIdx].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), DisplayName);
	
	// 세션 참여
	SessionInterface->JoinSession(0, FName(DisplayName), Results[SessionIdx]);
	
}

void UNetGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if ( Result == EOnJoinSessionCompleteResult::Success )
	{
		// 서버가 만든 세션 URL 로드
		FString URL;
		SessionInterface->GetResolvedConnectString(SessionName, URL);
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *URL)

		// 서버가 있는 맵으로 이동
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		PC->ClientTravel(URL, TRAVEL_Absolute);
	}
}
