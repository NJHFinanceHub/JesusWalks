#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NazareneCursorWidget.generated.h"

UCLASS()
class THENAZARENEAAA_API UNazareneCursorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual int32 NativePaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;

private:
    UPROPERTY(EditAnywhere, Category = "Cursor")
    float CursorSize = 40.0f;

    UPROPERTY(EditAnywhere, Category = "Cursor")
    float ArmLength = 11.0f;

    UPROPERTY(EditAnywhere, Category = "Cursor")
    float ArmThickness = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Cursor")
    float GapSize = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Cursor")
    float CenterSize = 4.0f;

    UPROPERTY(EditAnywhere, Category = "Cursor")
    float ShadowOffset = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Cursor")
    FLinearColor CrossColor = FLinearColor(0.96f, 0.88f, 0.62f, 0.95f);

    UPROPERTY(EditAnywhere, Category = "Cursor")
    FLinearColor CrossAccentColor = FLinearColor(1.0f, 0.97f, 0.86f, 1.0f);

    UPROPERTY(EditAnywhere, Category = "Cursor")
    FLinearColor ShadowColor = FLinearColor(0.03f, 0.02f, 0.01f, 0.75f);
};
