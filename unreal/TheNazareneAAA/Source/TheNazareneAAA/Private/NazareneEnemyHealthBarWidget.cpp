#include "NazareneEnemyHealthBarWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "NazareneEnemyCharacter.h"
#include "NazarenePlayerCharacter.h"

namespace
{
    static void AddBoxChild(UVerticalBox* Box, UWidget* Child, const FMargin& Padding)
    {
        if (Box == nullptr || Child == nullptr)
        {
            return;
        }

        if (UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Child))
        {
            Slot->SetPadding(Padding);
        }
    }
}

void UNazareneEnemyHealthBarWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (WidgetTree == nullptr)
    {
        return;
    }

    RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("HealthBarRoot"));
    RootBorder->SetBrushColor(FLinearColor(0.03f, 0.03f, 0.03f, 0.78f));
    WidgetTree->RootWidget = RootBorder;

    UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("HealthBarLayout"));
    RootBorder->SetContent(Layout);

    EnemyNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EnemyNameText"));
    if (EnemyNameText != nullptr)
    {
        EnemyNameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.90f, 0.80f, 1.0f)));
        FSlateFontInfo Font = EnemyNameText->GetFont();
        Font.Size = 12;
        EnemyNameText->SetFont(Font);
        EnemyNameText->SetJustification(ETextJustify::Center);
        AddBoxChild(Layout, EnemyNameText, FMargin(8.0f, 4.0f, 8.0f, 2.0f));
    }

    HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("EnemyHealthBar"));
    if (HealthBar != nullptr)
    {
        HealthBar->SetFillColorAndOpacity(FLinearColor(0.82f, 0.25f, 0.23f, 1.0f));
        HealthBar->SetPercent(1.0f);
        AddBoxChild(Layout, HealthBar, FMargin(8.0f, 0.0f, 8.0f, 2.0f));
    }

    PoiseBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("EnemyPoiseBar"));
    if (PoiseBar != nullptr)
    {
        PoiseBar->SetFillColorAndOpacity(FLinearColor(0.76f, 0.68f, 0.24f, 1.0f));
        PoiseBar->SetPercent(1.0f);
        AddBoxChild(Layout, PoiseBar, FMargin(8.0f, 0.0f, 8.0f, 6.0f));
    }

    SetVisibility(ESlateVisibility::Collapsed);
}

void UNazareneEnemyHealthBarWidget::BindToEnemy(ANazareneEnemyCharacter* InEnemy, ANazarenePlayerCharacter* InPlayer)
{
    Enemy = InEnemy;
    Player = InPlayer;

    if (EnemyNameText != nullptr)
    {
        EnemyNameText->SetText(FText::FromString(InEnemy != nullptr ? InEnemy->EnemyName : TEXT("Enemy")));
    }
}

void UNazareneEnemyHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    ANazareneEnemyCharacter* BoundEnemy = Enemy.Get();
    ANazarenePlayerCharacter* BoundPlayer = Player.Get();
    if (BoundEnemy == nullptr || BoundPlayer == nullptr || BoundEnemy->IsRedeemed())
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    const float HealthRatio = BoundEnemy->MaxHealth > KINDA_SMALL_NUMBER ? FMath::Clamp(BoundEnemy->CurrentHealth / BoundEnemy->MaxHealth, 0.0f, 1.0f) : 0.0f;
    const float PoiseRatio = BoundEnemy->MaxPoise > KINDA_SMALL_NUMBER ? FMath::Clamp(BoundEnemy->CurrentPoise / BoundEnemy->MaxPoise, 0.0f, 1.0f) : 0.0f;

    if (HealthBar != nullptr)
    {
        HealthBar->SetPercent(HealthRatio);
    }
    if (PoiseBar != nullptr)
    {
        PoiseBar->SetPercent(PoiseRatio);
    }

    const bool bHealthChanged = !FMath::IsNearlyEqual(HealthRatio, LastHealthRatio, 0.001f);
    if (bHealthChanged)
    {
        AutoFadeTimer = 2.3f;
    }
    LastHealthRatio = HealthRatio;

    APlayerController* PC = GetOwningPlayer();
    if (PC == nullptr)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    FVector2D ScreenPosition;
    const FVector WidgetAnchor = BoundEnemy->GetActorLocation() + FVector(0.0f, 0.0f, 145.0f);
    const bool bOnScreen = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WidgetAnchor, ScreenPosition, false);
    if (!bOnScreen)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    const float DistanceToPlayer = FVector::Dist2D(BoundEnemy->GetActorLocation(), BoundPlayer->GetActorLocation());
    const bool bLockedOn = BoundPlayer->GetLockTargetActor() == BoundEnemy;
    UpdateVisibilityState(InDeltaTime, bLockedOn, HealthRatio, DistanceToPlayer);
    if (GetVisibility() == ESlateVisibility::Collapsed)
    {
        return;
    }

    SetRenderTranslation(ScreenPosition + FVector2D(-90.0f, -42.0f));
}

void UNazareneEnemyHealthBarWidget::UpdateVisibilityState(float DeltaSeconds, bool bLockedOn, float HealthRatio, float DistanceToPlayer)
{
    AutoFadeTimer = FMath::Max(0.0f, AutoFadeTimer - DeltaSeconds);

    const bool bShouldShow =
        bLockedOn ||
        AutoFadeTimer > 0.0f ||
        HealthRatio < 0.999f ||
        DistanceToPlayer <= 640.0f;

    if (!bShouldShow)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    SetVisibility(ESlateVisibility::Visible);

    const float Alpha = bLockedOn ? 1.0f : FMath::Clamp(AutoFadeTimer / 2.3f, 0.35f, 0.92f);
    SetRenderOpacity(Alpha);
}
