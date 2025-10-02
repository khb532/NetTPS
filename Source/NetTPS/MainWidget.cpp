#include "MainWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"

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


