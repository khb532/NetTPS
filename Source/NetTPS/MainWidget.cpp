#include "MainWidget.h"

#include "NetTPSCharacter.h"
#include "NetTPSPlayerController.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// UPROPERTY 에서 meta = (BindWidget) 을 생성자에서
	BtnRetry = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Retry")));

	// Btn_Retry On Clicked Callback
	BtnRetry->OnClicked.AddDynamic(this/*현재클래스위치의*/, &UMainWidget::OnRetry);
}

void UMainWidget::AddBullet(int32 cnt)
{
	// 전체 총알 UI 제거
	PopAllBullet();
	
	// bullet count만큼 총알 ui생성
	for (int32 i = 0; i < cnt; i++)
	{
		UUserWidget* BulletUI = CreateWidget<UUserWidget>(GetWorld(), BulletWidget);
		// MagazineBox에 자식으로 설정
		MagazineBox->AddChildToHorizontalBox(BulletUI);
	}
	
}

void UMainWidget::PopBullet()
{
	// first idx 총알 위젯 삭제
	MagazineBox->RemoveChildAt(0);
}

void UMainWidget::PopAllBullet()
{
	// MagazineBox의 모든 총알 삭제
	MagazineBox->ClearChildren();

	/*int32 count = MagazineBox->GetChildrenCount();
	for (int32 i = 0; i < count; i++)
	{
		MagazineBox->RemoveChildAt(0);
	}*/
}

void UMainWidget::ShowCrosshair(bool bshow)
{
	Crosshair->SetVisibility(bshow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	
}

void UMainWidget::ShowDamageUI()
{
	DamageOpacity = 1;
}

void UMainWidget::OnRetry()
{
	// Observer Mode
	ANetTPSPlayerController* pc = Cast<ANetTPSPlayerController>(GetWorld()->GetFirstPlayerController());

	// To SERVER : Change Spectator Req
	pc->ServerRPC_ChangeToSpectator();

	pc->SetShowMouseCursor(false);

	// MainUI Remove
	RemoveFromParent();
}

void UMainWidget::ShowBtnRetry()
{
	// Show Button
	BtnRetry->SetVisibility(ESlateVisibility::Visible);
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DamageOpacity > 0)
	{
		DamageOpacity -= InDeltaTime * 3.f;

		// 0 Gaurd
		DamageOpacity = FMath::Max(DamageOpacity, 0.f);

		// DamageUI의 RenderOpacity Set
		DamageUI->SetRenderOpacity(DamageOpacity);
	}
}



