#include "NetGameInstance.h"
#include "OnlineSubsystem.h"		// IOnlineSubsystem*
#include "OnlineSubsystemUtils.h"	// Online::
#include "OnlineSessionSettings.h"

void UNetGameInstance::Init()
{
	Super::Init();

	// 현재 사용중인 서브시스템 로드
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());

	if (Subsystem)
	{
		// Subsystem의 Interface 로드
		SessionInterface = Subsystem->GetSessionInterface();

		// 세션 생성 성공시 호출될 함수 등록
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 실패"), *SessionName.ToString());
	}
}
