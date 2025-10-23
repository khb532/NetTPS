#include "ChatWidget.h"

#include "Components/TextBlock.h"

void UChatWidget::SetContent(const FString& m_content)
{
	txt_Content->SetText(FText::FromString(m_content));
	
}
