#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar.generated.h"


UCLASS()
class NETTPS_API UHPBar : public UUserWidget
{
	GENERATED_BODY()

public:
	float UpdateHP(float damage);

private:
	
public:
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* hpbar;

private:
	float MaxHp = 100.f;
	float CurHp = MaxHp;

	
};
