// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API USessionInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
/* Method */
public:
	// Begin Play
	virtual void NativeConstruct() override;

	void SetSessionInfo(int32 Idx, FString SessionName);

	UFUNCTION()
	void OnClickJoin();
protected:



private:


	

/* Field */
public:
	// Session Name
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Txt_SessionName;

	//	Join Button
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Join;


protected:



private:
	// Session List IDX
	int32 SessionIdx;


	
};
