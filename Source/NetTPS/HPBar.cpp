// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBar.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

#include "Components/ProgressBar.h"

float UHPBar::UpdateHP(float damage)
{
	// CurHp 를 damage 만큼 감소
	CurHp -= damage;

	// CurHp를 0~1 사이 소수로 변환
	float per = CurHp / MaxHp;
	
	// hpbar 의 percent 갱신
	hpbar->SetPercent(per);

	return CurHp;
}
