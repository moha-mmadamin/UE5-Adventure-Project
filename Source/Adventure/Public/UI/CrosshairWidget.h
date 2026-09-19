#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

class UImage;

UCLASS()
class ADVENTURE_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* Top;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* Bottom;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* Left;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* Right;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
	float ExpandAmount = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
	float DefaultOffset = 10.f;

	UFUNCTION(BlueprintCallable, Category = "Crosshair")
	void OnFire();

	UFUNCTION(BlueprintCallable, Category = "Crosshair")
	void ResetCrosshair();

protected:
    virtual void NativeConstruct() override;

private:
    void SetLineTranslation(UImage* Image, float X, float Y);
	
};
