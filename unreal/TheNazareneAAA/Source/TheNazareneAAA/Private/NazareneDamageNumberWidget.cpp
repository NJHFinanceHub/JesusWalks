#include "NazareneDamageNumberWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"

void UNazareneDamageNumberWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (WidgetTree == nullptr)
    {
        return;
    }

    ValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DamageValueText"));
    if (ValueText != nullptr)
    {
        FSlateFontInfo FontInfo = ValueText->GetFont();
        FontInfo.Size = 20;
        ValueText->SetFont(FontInfo);
        ValueText->SetJustification(ETextJustify::Center);
        WidgetTree->RootWidget = ValueText;
    }
}

void UNazareneDamageNumberWidget::InitializeDamageNumber(APlayerController* InOwningController, const FVector& InWorldLocation, float InAmount, ENazareneDamageNumberType InType)
{
    OwningController = InOwningController;
    WorldLocation = InWorldLocation;
    Amount = InAmount;
    Type = InType;
    ElapsedTime = 0.0f;
    bExpired = false;

    if (ValueText == nullptr)
    {
        return;
    }

    FString Prefix;
    switch (Type)
    {
    case ENazareneDamageNumberType::Critical:
        Prefix = TEXT("CRIT ");
        break;
    case ENazareneDamageNumberType::Heal:
        Prefix = TEXT("+");
        break;
    case ENazareneDamageNumberType::PoiseBreak:
        Prefix = TEXT("POISE ");
        break;
    case ENazareneDamageNumberType::Blocked:
        Prefix = TEXT("BLOCK ");
        break;
    default:
        break;
    }

    const int32 DisplayAmount = FMath::RoundToInt(FMath::Abs(Amount));
    ValueText->SetText(FText::FromString(FString::Printf(TEXT("%s%d"), *Prefix, DisplayAmount)));
    UpdateVisualState(1.0f);
}

void UNazareneDamageNumberWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bExpired)
    {
        return;
    }

    ElapsedTime += InDeltaTime;
    const float LifeRatio = Lifetime > KINDA_SMALL_NUMBER ? FMath::Clamp(ElapsedTime / Lifetime, 0.0f, 1.0f) : 1.0f;
    const float Alpha = 1.0f - LifeRatio;

    WorldLocation.Z += DriftSpeed * InDeltaTime;

    if (OwningController != nullptr)
    {
        FVector2D ScreenPosition;
        if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(OwningController, WorldLocation, ScreenPosition, false))
        {
            SetRenderTranslation(ScreenPosition);
        }
    }

    UpdateVisualState(Alpha);

    if (LifeRatio >= 1.0f)
    {
        bExpired = true;
        RemoveFromParent();
    }
}

void UNazareneDamageNumberWidget::UpdateVisualState(float Alpha)
{
    if (ValueText == nullptr)
    {
        return;
    }

    FLinearColor Color(0.95f, 0.90f, 0.82f, Alpha);
    switch (Type)
    {
    case ENazareneDamageNumberType::Critical:
        Color = FLinearColor(1.0f, 0.30f, 0.22f, Alpha);
        break;
    case ENazareneDamageNumberType::Heal:
        Color = FLinearColor(0.30f, 0.92f, 0.52f, Alpha);
        break;
    case ENazareneDamageNumberType::PoiseBreak:
        Color = FLinearColor(0.95f, 0.70f, 0.20f, Alpha);
        break;
    case ENazareneDamageNumberType::Blocked:
        Color = FLinearColor(0.72f, 0.80f, 0.95f, Alpha);
        break;
    default:
        break;
    }

    ValueText->SetColorAndOpacity(FSlateColor(Color));
}
