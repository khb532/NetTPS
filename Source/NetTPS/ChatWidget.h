#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"


UCLASS()
class NETTPS_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

/* Method */
public:
	void SetContent(const FString& m_content);

	
private:



/* Field */
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> txt_Content;


private:



	
};
