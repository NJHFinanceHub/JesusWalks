#include "NazareneCursorWidget.h"

#include "GameFramework/PlayerController.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

void UNazareneCursorWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetDesiredSizeInViewport(FVector2D(CursorSize, CursorSize));
    SetAlignmentInViewport(FVector2D::ZeroVector);
    SetVisibility(ESlateVisibility::Collapsed);
}

void UNazareneCursorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    APlayerController* PC = GetOwningPlayer();
    if (PC == nullptr || !PC->bShowMouseCursor)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    float MouseX = 0.0f;
    float MouseY = 0.0f;
    if (!PC->GetMousePosition(MouseX, MouseY))
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    SetVisibility(ESlateVisibility::HitTestInvisible);
    SetPositionInViewport(FVector2D(MouseX - CursorSize * 0.5f, MouseY - CursorSize * 0.5f), false);
}

int32 UNazareneCursorWidget::NativePaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    const int32 BaseLayer = Super::NativePaint(
        Args,
        AllottedGeometry,
        MyCullingRect,
        OutDrawElements,
        LayerId,
        InWidgetStyle,
        bParentEnabled);

    const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("WhiteBrush");
    if (WhiteBrush == nullptr)
    {
        return BaseLayer;
    }

    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const FVector2D Center = LocalSize * 0.5f;
    const float HalfThickness = ArmThickness * 0.5f;

    const FVector2D LeftPos(Center.X - GapSize - ArmLength, Center.Y - HalfThickness);
    const FVector2D RightPos(Center.X + GapSize, Center.Y - HalfThickness);
    const FVector2D TopPos(Center.X - HalfThickness, Center.Y - GapSize - ArmLength);
    const FVector2D BottomPos(Center.X - HalfThickness, Center.Y + GapSize);
    const FVector2D CenterPos(Center.X - CenterSize * 0.5f, Center.Y - CenterSize * 0.5f);

    auto DrawBox = [&](const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color, int32 DrawLayer)
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            DrawLayer,
            AllottedGeometry.ToPaintGeometry(FVector2f(Size), FSlateLayoutTransform(FVector2f(Position))),
            WhiteBrush,
            ESlateDrawEffect::None,
            Color);
    };

    const FVector2D HorizontalSize(ArmLength, ArmThickness);
    const FVector2D VerticalSize(ArmThickness, ArmLength);
    const FVector2D CenterDotSize(CenterSize, CenterSize);
    const FVector2D ShadowDelta(ShadowOffset, ShadowOffset);

    const int32 ShadowLayer = BaseLayer + 1;
    const int32 CursorLayer = BaseLayer + 2;

    DrawBox(LeftPos + ShadowDelta, HorizontalSize, ShadowColor, ShadowLayer);
    DrawBox(RightPos + ShadowDelta, HorizontalSize, ShadowColor, ShadowLayer);
    DrawBox(TopPos + ShadowDelta, VerticalSize, ShadowColor, ShadowLayer);
    DrawBox(BottomPos + ShadowDelta, VerticalSize, ShadowColor, ShadowLayer);
    DrawBox(CenterPos + ShadowDelta, CenterDotSize, ShadowColor, ShadowLayer);

    DrawBox(LeftPos, HorizontalSize, CrossColor, CursorLayer);
    DrawBox(RightPos, HorizontalSize, CrossColor, CursorLayer);
    DrawBox(TopPos, VerticalSize, CrossColor, CursorLayer);
    DrawBox(BottomPos, VerticalSize, CrossColor, CursorLayer);
    DrawBox(CenterPos, CenterDotSize, CrossAccentColor, CursorLayer);

    return CursorLayer;
}
