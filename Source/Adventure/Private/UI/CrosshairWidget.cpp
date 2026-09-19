#include "UI/CrosshairWidget.h"
#include "Components/Image.h"

void UCrosshairWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ResetCrosshair();
}
void UCrosshairWidget::OnFire()
{
    SetLineTranslation(Left,   -ExpandAmount,  0.f);
	SetLineTranslation(Top,     0.f,           -ExpandAmount);
	SetLineTranslation(Right,   ExpandAmount,   0.f);
	SetLineTranslation(Bottom,  0.f,            ExpandAmount);

	FTimerHandle ResetHandle;
	GetWorld()->GetTimerManager().SetTimer(
		ResetHandle,
		this,
		&UCrosshairWidget::ResetCrosshair,
		0.1f,
		false
	);
}
void UCrosshairWidget::ResetCrosshair()
{
    SetLineTranslation(Left,   -DefaultOffset,  0.f);
	SetLineTranslation(Top,     0.f,            -DefaultOffset);
	SetLineTranslation(Right,   DefaultOffset,   0.f);
	SetLineTranslation(Bottom,  0.f,             DefaultOffset);
}
void UCrosshairWidget::SetLineTranslation(UImage* Image, float X, float Y)
{
    if(!Image) return;

    FWidgetTransform Transform = Image->GetRenderTransform();
	Transform.Translation = FVector2D(X, Y);
	Image->SetRenderTransform(Transform);
}