#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"


UCLASS()
class NETTPS_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

/* Method */
public:
	// 총알 UI를 현재 총알수 만큼 추가
	void AddBullet(int32 cnt);

	// 총알UI하나씩 제거
	void PopBullet();

	// 총알UI 전체 제거
	void PopAllBullet();

	// crosshair on/off
	void ShowCrosshair(bool bshow);
protected:

private:


/* Field */
public:
	UPROPERTY(EditAnywhere)		// WBP_Bullet을 저장
	TSubclassOf<class UUserWidget> BulletWidget;

	UPROPERTY(meta=(BindWidget))	// WBP의 UI항목 이름과 일치
	class UHorizontalBox* MagazineBox;

	UPROPERTY(meta=(BindWidget))	// crosshair 담을 변수
	class UImage* Crosshair;

protected:

private:
	
};
