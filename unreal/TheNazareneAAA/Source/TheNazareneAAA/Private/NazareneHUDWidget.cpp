#include "NazareneHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NazareneCampaignGameMode.h"
#include "NazareneDamageNumberWidget.h"
#include "NazareneEnemyCharacter.h"
#include "NazareneEnemyHealthBarWidget.h"
#include "NazareneGameInstance.h"
#include "NazareneHUD.h"
#include "NazarenePlayerCharacter.h"
#include "NazareneSaveSubsystem.h"
#include "NazareneSettingsSubsystem.h"
#include "NazareneSkillTreeWidget.h"
#include "Components/SizeBox.h"
#include "Styling/SlateTypes.h"

namespace
{
    static void ConfigureText(UTextBlock* Text, const FString& Value, const FLinearColor& Color, int32 Size)
    {
        if (Text == nullptr)
        {
            return;
        }

        Text->SetText(FText::FromString(Value));
        Text->SetColorAndOpacity(FSlateColor(Color));

        FSlateFontInfo FontInfo = Text->GetFont();
        FontInfo.Size = Size;
        Text->SetFont(FontInfo);
    }

    static void AddVerticalChild(UVerticalBox* Parent, UWidget* Child, const FMargin& Padding)
    {
        if (Parent == nullptr || Child == nullptr)
        {
            return;
        }

        UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Child);
        if (Slot != nullptr)
        {
            Slot->SetPadding(Padding);
        }
    }

    static FButtonStyle CreateThemedButtonStyle()
    {
        FButtonStyle Style;

        // Normal state: dark warm brown with subtle gold border
        FSlateBrush NormalBrush;
        NormalBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
        NormalBrush.TintColor = FSlateColor(FLinearColor(0.10f, 0.08f, 0.05f, 1.0f));
        NormalBrush.OutlineSettings.CornerRadii = FVector4(4.0f, 4.0f, 4.0f, 4.0f);
        NormalBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
        NormalBrush.OutlineSettings.Color = FSlateColor(FLinearColor(0.55f, 0.45f, 0.22f, 0.70f));
        NormalBrush.OutlineSettings.Width = 1.5f;
        Style.SetNormal(NormalBrush);

        // Hovered state: gold-tinted brightening with glowing border
        FSlateBrush HoveredBrush = NormalBrush;
        HoveredBrush.TintColor = FSlateColor(FLinearColor(0.22f, 0.18f, 0.08f, 1.0f));
        HoveredBrush.OutlineSettings.Color = FSlateColor(FLinearColor(0.78f, 0.68f, 0.38f, 0.95f));
        HoveredBrush.OutlineSettings.Width = 2.0f;
        Style.SetHovered(HoveredBrush);

        // Pressed state: darkened inset with dimmer border
        FSlateBrush PressedBrush = NormalBrush;
        PressedBrush.TintColor = FSlateColor(FLinearColor(0.06f, 0.05f, 0.03f, 1.0f));
        PressedBrush.OutlineSettings.Color = FSlateColor(FLinearColor(0.40f, 0.34f, 0.18f, 0.80f));
        PressedBrush.OutlineSettings.Width = 1.0f;
        Style.SetPressed(PressedBrush);

        // Padding with 1px downshift on press for physical push feel
        Style.SetNormalPadding(FMargin(16.0f, 10.0f));
        Style.SetPressedPadding(FMargin(16.0f, 11.0f, 16.0f, 9.0f));

        return Style;
    }

    static UButton* CreateMenuButton(
        UWidgetTree* WidgetTree,
        UVerticalBox* Parent,
        const TCHAR* Name,
        const FString& Label,
        UTextBlock*& OutLabel
    )
    {
        if (WidgetTree == nullptr || Parent == nullptr)
        {
            return nullptr;
        }

        UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
        if (Button == nullptr)
        {
            return nullptr;
        }

        // Apply themed biblical gold style
        Button->SetStyle(CreateThemedButtonStyle());

        const FName LabelName(*FString::Printf(TEXT("%sLabel"), Name));
        OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), LabelName);
        ConfigureText(OutLabel, Label, FLinearColor(0.95f, 0.90f, 0.82f), 20);
        OutLabel->SetJustification(ETextJustify::Center);
        Button->AddChild(OutLabel);

        // Wrap in SizeBox for 48px minimum click target
        const FName SizeBoxName(*FString::Printf(TEXT("%sSizeBox"), Name));
        USizeBox* SizeWrapper = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), SizeBoxName);
        if (SizeWrapper != nullptr)
        {
            SizeWrapper->SetMinDesiredHeight(48.0f);
            SizeWrapper->AddChild(Button);
            AddVerticalChild(Parent, SizeWrapper, FMargin(12.0f, 4.0f, 12.0f, 4.0f));
        }
        else
        {
            AddVerticalChild(Parent, Button, FMargin(12.0f, 4.0f, 12.0f, 4.0f));
        }

        return Button;
    }

    // Create a compact menu button with tighter padding for dense menus
    static UButton* CreateCompactMenuButton(
        UWidgetTree* WidgetTree,
        UVerticalBox* Parent,
        const TCHAR* Name,
        const FString& Label,
        UTextBlock*& OutLabel,
        int32 FontSize = 17
    )
    {
        if (WidgetTree == nullptr || Parent == nullptr)
        {
            return nullptr;
        }

        UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
        if (Button == nullptr)
        {
            return nullptr;
        }

        FButtonStyle Style = CreateThemedButtonStyle();
        Style.SetNormalPadding(FMargin(12.0f, 6.0f));
        Style.SetPressedPadding(FMargin(12.0f, 7.0f, 12.0f, 5.0f));
        Button->SetStyle(Style);

        const FName LabelName(*FString::Printf(TEXT("%sLabel"), Name));
        OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), LabelName);
        ConfigureText(OutLabel, Label, FLinearColor(0.95f, 0.90f, 0.82f), FontSize);
        OutLabel->SetJustification(ETextJustify::Center);
        Button->AddChild(OutLabel);

        const FName SizeBoxName(*FString::Printf(TEXT("%sSizeBox"), Name));
        USizeBox* SizeWrapper = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), SizeBoxName);
        if (SizeWrapper != nullptr)
        {
            SizeWrapper->SetMinDesiredHeight(40.0f);
            SizeWrapper->AddChild(Button);
            AddVerticalChild(Parent, SizeWrapper, FMargin(10.0f, 2.0f, 10.0f, 2.0f));
        }
        else
        {
            AddVerticalChild(Parent, Button, FMargin(10.0f, 2.0f, 10.0f, 2.0f));
        }

        return Button;
    }

    // Styled progress bar with dark backing and height constraint
    static UProgressBar* CreateStyledBar(
        UWidgetTree* WidgetTree,
        UVerticalBox* Parent,
        const TCHAR* Name,
        float BarHeight,
        const FLinearColor& FillColor,
        const FMargin& Padding
    )
    {
        if (WidgetTree == nullptr || Parent == nullptr)
        {
            return nullptr;
        }

        UProgressBar* Bar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), Name);
        if (Bar == nullptr)
        {
            return nullptr;
        }

        Bar->SetPercent(1.0f);
        Bar->SetFillColorAndOpacity(FillColor);

        // Style the bar with a dark backing
        FProgressBarStyle BarStyle = Bar->GetWidgetStyle();
        FSlateBrush BackgroundBrush;
        BackgroundBrush.TintColor = FSlateColor(FLinearColor(0.03f, 0.03f, 0.02f, 0.92f));
        BarStyle.BackgroundImage = BackgroundBrush;
        Bar->SetWidgetStyle(BarStyle);

        // Constrain height with SizeBox
        const FName SizeBoxName(*FString::Printf(TEXT("%sSizeBox"), Name));
        USizeBox* SizeWrapper = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), SizeBoxName);
        if (SizeWrapper != nullptr)
        {
            SizeWrapper->SetMaxDesiredHeight(BarHeight);
            SizeWrapper->SetMinDesiredHeight(BarHeight);
            SizeWrapper->AddChild(Bar);
            AddVerticalChild(Parent, SizeWrapper, Padding);
        }
        else
        {
            AddVerticalChild(Parent, Bar, Padding);
        }

        return Bar;
    }

    // Gold ornamental divider line
    static void AddGoldSeparator(UWidgetTree* WidgetTree, UVerticalBox* Parent, const TCHAR* Name, const FMargin& Padding)
    {
        if (WidgetTree == nullptr || Parent == nullptr)
        {
            return;
        }

        UBorder* Separator = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
        if (Separator != nullptr)
        {
            Separator->SetBrushColor(FLinearColor(0.55f, 0.45f, 0.22f, 0.50f));
            USizeBox* SepSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
            if (SepSizeBox != nullptr)
            {
                SepSizeBox->SetMaxDesiredHeight(1.0f);
                SepSizeBox->SetMinDesiredHeight(1.0f);
                SepSizeBox->AddChild(Separator);
                AddVerticalChild(Parent, SepSizeBox, Padding);
            }
        }
    }
}

void UNazareneHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (WidgetTree == nullptr)
    {
        return;
    }

    UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootPanel"));
    if (RootPanel == nullptr)
    {
        return;
    }
    WidgetTree->RootWidget = RootPanel;

    // ======================================================================
    // PLAYER VITALS PANEL - Compact Dark Souls style (top-left)
    // ======================================================================
    UBorder* PlayerPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PlayerPanel"));
    PlayerPanel->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.03f, 0.80f));
    UCanvasPanelSlot* PlayerPanelSlot = RootPanel->AddChildToCanvas(PlayerPanel);
    if (PlayerPanelSlot != nullptr)
    {
        PlayerPanelSlot->SetSize(FVector2D(320.0f, 170.0f));
        PlayerPanelSlot->SetPosition(FVector2D(24.0f, 18.0f));
    }

    UVerticalBox* PlayerPanelContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PlayerPanelContent"));
    PlayerPanel->SetContent(PlayerPanelContent);

    // Health bar - red, 14px tall, dark backing
    HealthText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HealthText"));
    ConfigureText(HealthText, TEXT(""), FLinearColor(0.80f, 0.80f, 0.75f), 11);
    AddVerticalChild(PlayerPanelContent, HealthText, FMargin(10.0f, 8.0f, 10.0f, 1.0f));

    HealthBar = CreateStyledBar(WidgetTree, PlayerPanelContent, TEXT("HealthBar"), 14.0f,
        FLinearColor(0.78f, 0.18f, 0.14f, 1.0f), FMargin(10.0f, 0.0f, 10.0f, 4.0f));

    // Stamina bar - muted green, 12px tall
    StaminaText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StaminaText"));
    ConfigureText(StaminaText, TEXT(""), FLinearColor(0.80f, 0.80f, 0.75f), 11);
    AddVerticalChild(PlayerPanelContent, StaminaText, FMargin(10.0f, 2.0f, 10.0f, 1.0f));

    StaminaBar = CreateStyledBar(WidgetTree, PlayerPanelContent, TEXT("StaminaBar"), 12.0f,
        FLinearColor(0.22f, 0.62f, 0.20f, 1.0f), FMargin(10.0f, 0.0f, 10.0f, 4.0f));

    // Faith bar - gold, 10px tall
    FaithText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FaithText"));
    ConfigureText(FaithText, TEXT(""), FLinearColor(0.90f, 0.82f, 0.50f), 11);
    AddVerticalChild(PlayerPanelContent, FaithText, FMargin(10.0f, 2.0f, 10.0f, 1.0f));

    FaithBar = CreateStyledBar(WidgetTree, PlayerPanelContent, TEXT("FaithBar"), 10.0f,
        FLinearColor(0.88f, 0.78f, 0.38f, 1.0f), FMargin(10.0f, 0.0f, 10.0f, 4.0f));

    // Compact miracle cooldown strip - 3 thin bars
    HealCooldownBar = CreateStyledBar(WidgetTree, PlayerPanelContent, TEXT("HealCooldownBar"), 5.0f,
        FLinearColor(0.2f, 0.72f, 0.3f, 0.85f), FMargin(10.0f, 2.0f, 10.0f, 1.0f));
    BlessingCooldownBar = CreateStyledBar(WidgetTree, PlayerPanelContent, TEXT("BlessingCooldownBar"), 5.0f,
        FLinearColor(0.85f, 0.72f, 0.25f, 0.85f), FMargin(10.0f, 1.0f, 10.0f, 1.0f));
    RadianceCooldownBar = CreateStyledBar(WidgetTree, PlayerPanelContent, TEXT("RadianceCooldownBar"), 5.0f,
        FLinearColor(0.85f, 0.52f, 0.18f, 0.85f), FMargin(10.0f, 1.0f, 10.0f, 6.0f));

    // Compact level/combat state line (hidden until player data available)
    CombatStateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CombatStateText"));
    ConfigureText(CombatStateText, TEXT(""), FLinearColor(0.72f, 0.70f, 0.62f), 11);
    CombatStateText->SetAutoWrapText(true);
    AddVerticalChild(PlayerPanelContent, CombatStateText, FMargin(10.0f, 0.0f, 10.0f, 6.0f));

    // Lock target and context hint text - positioned separately below player panel
    LockTargetText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LockTargetText"));
    ConfigureText(LockTargetText, TEXT(""), FLinearColor(0.80f, 0.76f, 0.68f), 13);
    LockTargetText->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* LockTargetSlot = RootPanel->AddChildToCanvas(LockTargetText);
    if (LockTargetSlot != nullptr)
    {
        LockTargetSlot->SetSize(FVector2D(320.0f, 22.0f));
        LockTargetSlot->SetPosition(FVector2D(24.0f, 194.0f));
    }

    ContextHintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContextHintText"));
    ConfigureText(ContextHintText, TEXT(""), FLinearColor(0.88f, 0.80f, 0.58f), 14);
    ContextHintText->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* ContextHintSlot = RootPanel->AddChildToCanvas(ContextHintText);
    if (ContextHintSlot != nullptr)
    {
        ContextHintSlot->SetSize(FVector2D(400.0f, 28.0f));
        ContextHintSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
        ContextHintSlot->SetAlignment(FVector2D(0.5f, 1.0f));
        ContextHintSlot->SetPosition(FVector2D(0.0f, -24.0f));
    }

    // Cooldown labels (hidden; kept for compatibility)
    HealCooldownLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HealCooldownLabel"));
    HealCooldownLabel->SetVisibility(ESlateVisibility::Collapsed);
    BlessingCooldownLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BlessingCooldownLabel"));
    BlessingCooldownLabel->SetVisibility(ESlateVisibility::Collapsed);
    RadianceCooldownLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RadianceCooldownLabel"));
    RadianceCooldownLabel->SetVisibility(ESlateVisibility::Collapsed);

    // ======================================================================
    // OBJECTIVE PANEL (top-right)
    // ======================================================================
    UBorder* ObjectivePanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ObjectivePanel"));
    ObjectivePanel->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.03f, 0.72f));
    UCanvasPanelSlot* ObjectivePanelSlot = RootPanel->AddChildToCanvas(ObjectivePanel);
    if (ObjectivePanelSlot != nullptr)
    {
        ObjectivePanelSlot->SetSize(FVector2D(420.0f, 120.0f));
        ObjectivePanelSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
        ObjectivePanelSlot->SetAlignment(FVector2D(1.0f, 0.0f));
        ObjectivePanelSlot->SetPosition(FVector2D(-24.0f, 18.0f));
    }

    UVerticalBox* ObjectiveContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ObjectiveContent"));
    ObjectivePanel->SetContent(ObjectiveContent);

    RegionNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RegionNameText"));
    ConfigureText(RegionNameText, CachedRegionName, FLinearColor(0.90f, 0.84f, 0.68f), 15);
    AddVerticalChild(ObjectiveContent, RegionNameText, FMargin(12.0f, 10.0f, 12.0f, 3.0f));

    AddGoldSeparator(WidgetTree, ObjectiveContent, TEXT("ObjSeparator"), FMargin(12.0f, 0.0f, 12.0f, 3.0f));

    ObjectiveText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ObjectiveText"));
    ConfigureText(ObjectiveText, CachedObjective, FLinearColor(0.78f, 0.75f, 0.65f), 13);
    ObjectiveText->SetAutoWrapText(true);
    AddVerticalChild(ObjectiveContent, ObjectiveText, FMargin(12.0f, 0.0f, 12.0f, 8.0f));

    // ======================================================================
    // MESSAGE TEXT (top-center, shows contextual messages)
    // ======================================================================
    MessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MessageText"));
    ConfigureText(MessageText, TEXT(""), FLinearColor(0.95f, 0.90f, 0.78f), 18);
    MessageText->SetJustification(ETextJustify::Center);
    MessageText->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* MessageSlot = RootPanel->AddChildToCanvas(MessageText);
    if (MessageSlot != nullptr)
    {
        MessageSlot->SetSize(FVector2D(760.0f, 36.0f));
        MessageSlot->SetAnchors(FAnchors(0.5f, 0.0f, 0.5f, 0.0f));
        MessageSlot->SetAlignment(FVector2D(0.5f, 0.0f));
        MessageSlot->SetPosition(FVector2D(0.0f, 18.0f));
    }

    // ======================================================================
    // CRITICAL STATE TEXT (upper-center, danger warnings)
    // ======================================================================
    CriticalStateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CriticalStateText"));
    ConfigureText(CriticalStateText, TEXT(""), FLinearColor(0.95f, 0.40f, 0.28f), 17);
    CriticalStateText->SetJustification(ETextJustify::Center);
    CriticalStateText->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* CriticalStateSlot = RootPanel->AddChildToCanvas(CriticalStateText);
    if (CriticalStateSlot != nullptr)
    {
        CriticalStateSlot->SetSize(FVector2D(600.0f, 36.0f));
        CriticalStateSlot->SetAnchors(FAnchors(0.5f, 0.0f, 0.5f, 0.0f));
        CriticalStateSlot->SetAlignment(FVector2D(0.5f, 0.0f));
        CriticalStateSlot->SetPosition(FVector2D(0.0f, 62.0f));
    }

    // ======================================================================
    // PAUSE MENU OVERLAY
    // ======================================================================
    PauseOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PauseOverlay"));
    PauseOverlay->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.78f));
    PauseOverlay->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* PauseOverlaySlot = RootPanel->AddChildToCanvas(PauseOverlay);
    if (PauseOverlaySlot != nullptr)
    {
        PauseOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        PauseOverlaySlot->SetOffsets(FMargin(0.0f));
    }

    UCanvasPanel* PauseCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("PauseCanvas"));
    PauseOverlay->SetContent(PauseCanvas);

    UBorder* PauseMenuPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PauseMenuPanel"));
    PauseMenuPanel->SetBrushColor(FLinearColor(0.06f, 0.06f, 0.04f, 0.96f));
    UCanvasPanelSlot* PauseMenuSlot = PauseCanvas->AddChildToCanvas(PauseMenuPanel);
    if (PauseMenuSlot != nullptr)
    {
        PauseMenuSlot->SetSize(FVector2D(440.0f, 560.0f));
        PauseMenuSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        PauseMenuSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        PauseMenuSlot->SetPosition(FVector2D::ZeroVector);
    }

    UVerticalBox* PauseMenuContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PauseMenuContent"));
    PauseMenuPanel->SetContent(PauseMenuContent);

    UTextBlock* PauseTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PauseTitle"));
    ConfigureText(PauseTitle, TEXT("PILGRIMAGE"), FLinearColor(0.90f, 0.84f, 0.68f), 24);
    PauseTitle->SetJustification(ETextJustify::Center);
    AddVerticalChild(PauseMenuContent, PauseTitle, FMargin(12.0f, 16.0f, 12.0f, 4.0f));

    AddGoldSeparator(WidgetTree, PauseMenuContent, TEXT("PauseTitleSep"), FMargin(40.0f, 0.0f, 40.0f, 8.0f));

    UTextBlock* ButtonLabel = nullptr;
    PauseResumeButton = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("ResumeButton"), TEXT("Resume"), ButtonLabel);
    if (PauseResumeButton != nullptr)
    {
        PauseResumeButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleResumePressed);
    }

    AddGoldSeparator(WidgetTree, PauseMenuContent, TEXT("SaveSep"), FMargin(20.0f, 6.0f, 20.0f, 4.0f));

    PauseSaveSlot1Button = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("SaveSlot1Button"), TEXT("Save Slot 1"), ButtonLabel);
    if (PauseSaveSlot1Button != nullptr)
    {
        PauseSaveSlot1Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSaveSlot1Pressed);
    }
    PauseSaveSlot2Button = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("SaveSlot2Button"), TEXT("Save Slot 2"), ButtonLabel);
    if (PauseSaveSlot2Button != nullptr)
    {
        PauseSaveSlot2Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSaveSlot2Pressed);
    }
    PauseSaveSlot3Button = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("SaveSlot3Button"), TEXT("Save Slot 3"), ButtonLabel);
    if (PauseSaveSlot3Button != nullptr)
    {
        PauseSaveSlot3Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSaveSlot3Pressed);
    }

    AddGoldSeparator(WidgetTree, PauseMenuContent, TEXT("LoadSep"), FMargin(20.0f, 6.0f, 20.0f, 4.0f));

    PauseLoadSlot1Button = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("LoadSlot1Button"), TEXT("Load Slot 1"), ButtonLabel);
    if (PauseLoadSlot1Button != nullptr)
    {
        PauseLoadSlot1Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleLoadSlot1Pressed);
    }
    PauseLoadSlot2Button = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("LoadSlot2Button"), TEXT("Load Slot 2"), ButtonLabel);
    if (PauseLoadSlot2Button != nullptr)
    {
        PauseLoadSlot2Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleLoadSlot2Pressed);
    }
    PauseLoadSlot3Button = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("LoadSlot3Button"), TEXT("Load Slot 3"), ButtonLabel);
    if (PauseLoadSlot3Button != nullptr)
    {
        PauseLoadSlot3Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleLoadSlot3Pressed);
    }

    // Compact slot summaries
    SlotSummary1Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotSummary1Text"));
    ConfigureText(SlotSummary1Text, TEXT("Slot 1: Empty"), FLinearColor(0.62f, 0.60f, 0.55f), 11);
    AddVerticalChild(PauseMenuContent, SlotSummary1Text, FMargin(14.0f, 4.0f, 12.0f, 0.0f));

    SlotSummary2Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotSummary2Text"));
    ConfigureText(SlotSummary2Text, TEXT("Slot 2: Empty"), FLinearColor(0.62f, 0.60f, 0.55f), 11);
    AddVerticalChild(PauseMenuContent, SlotSummary2Text, FMargin(14.0f, 1.0f, 12.0f, 0.0f));

    SlotSummary3Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotSummary3Text"));
    ConfigureText(SlotSummary3Text, TEXT("Slot 3: Empty"), FLinearColor(0.62f, 0.60f, 0.55f), 11);
    AddVerticalChild(PauseMenuContent, SlotSummary3Text, FMargin(14.0f, 1.0f, 12.0f, 4.0f));

    AddGoldSeparator(WidgetTree, PauseMenuContent, TEXT("BottomSep"), FMargin(20.0f, 4.0f, 20.0f, 4.0f));

    PauseNewPilgrimageButton = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("NewPilgrimageButton"), TEXT("New Pilgrimage"), ButtonLabel);
    if (PauseNewPilgrimageButton != nullptr)
    {
        PauseNewPilgrimageButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleNewPilgrimagePressed);
    }

    PauseOptionsButton = CreateCompactMenuButton(WidgetTree, PauseMenuContent, TEXT("PauseOptionsButton"), TEXT("Options"), ButtonLabel);
    if (PauseOptionsButton != nullptr)
    {
        PauseOptionsButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleOptionsPressed);
    }

    // Pause menu gamepad navigation chain
    {
        UButton* PauseButtons[] = {
            PauseResumeButton, PauseSaveSlot1Button, PauseSaveSlot2Button, PauseSaveSlot3Button,
            PauseLoadSlot1Button, PauseLoadSlot2Button, PauseLoadSlot3Button,
            PauseNewPilgrimageButton, PauseOptionsButton
        };
        const int32 PauseCount = UE_ARRAY_COUNT(PauseButtons);
        for (int32 i = 0; i < PauseCount; ++i)
        {
            if (PauseButtons[i] == nullptr) { continue; }
            int32 NextIndex = (i + 1) % PauseCount;
            int32 PrevIndex = (i - 1 + PauseCount) % PauseCount;
            if (PauseButtons[NextIndex] != nullptr)
            {
                PauseButtons[i]->SetNavigationRuleExplicit(EUINavigation::Down, PauseButtons[NextIndex]);
            }
            if (PauseButtons[PrevIndex] != nullptr)
            {
                PauseButtons[i]->SetNavigationRuleExplicit(EUINavigation::Up, PauseButtons[PrevIndex]);
            }
        }
    }

    // ======================================================================
    // START MENU OVERLAY
    // ======================================================================
    StartMenuOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StartMenuOverlay"));
    StartMenuOverlay->SetBrushColor(FLinearColor(0.02f, 0.02f, 0.01f, 0.42f));
    StartMenuOverlay->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* StartOverlaySlot = RootPanel->AddChildToCanvas(StartMenuOverlay);
    if (StartOverlaySlot != nullptr)
    {
        StartOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        StartOverlaySlot->SetOffsets(FMargin(0.0f));
    }

    UCanvasPanel* StartCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("StartCanvas"));
    StartMenuOverlay->SetContent(StartCanvas);

    UBorder* StartMenuPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StartMenuPanel"));
    StartMenuPanel->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.03f, 0.92f));
    UCanvasPanelSlot* StartPanelSlot = StartCanvas->AddChildToCanvas(StartMenuPanel);
    if (StartPanelSlot != nullptr)
    {
        StartPanelSlot->SetSize(FVector2D(520.0f, 420.0f));
        StartPanelSlot->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
        StartPanelSlot->SetAlignment(FVector2D(0.0f, 1.0f));
        StartPanelSlot->SetPosition(FVector2D(54.0f, -42.0f));
    }

    UVerticalBox* StartMenuContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StartMenuContent"));
    StartMenuPanel->SetContent(StartMenuContent);

    UTextBlock* StartTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StartTitle"));
    ConfigureText(StartTitle, TEXT("THE NAZARENE"), FLinearColor(0.92f, 0.86f, 0.68f), 32);
    StartTitle->SetJustification(ETextJustify::Left);
    AddVerticalChild(StartMenuContent, StartTitle, FMargin(20.0f, 26.0f, 18.0f, 2.0f));

    UTextBlock* StartSubtitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StartSubtitle"));
    ConfigureText(StartSubtitle, TEXT("The stone is rolled away."), FLinearColor(0.82f, 0.76f, 0.58f), 16);
    StartSubtitle->SetJustification(ETextJustify::Left);
    AddVerticalChild(StartMenuContent, StartSubtitle, FMargin(20.0f, 0.0f, 18.0f, 2.0f));

    AddGoldSeparator(WidgetTree, StartMenuContent, TEXT("StartSep"), FMargin(20.0f, 8.0f, 20.0f, 12.0f));

    UTextBlock* StartButtonLabel = nullptr;
    StartNewGameButton = CreateMenuButton(WidgetTree, StartMenuContent, TEXT("StartPilgrimageButton"), TEXT("Begin Pilgrimage"), StartButtonLabel);
    if (StartNewGameButton != nullptr)
    {
        StartNewGameButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleStartPilgrimagePressed);
    }

    StartContinueButton = CreateMenuButton(WidgetTree, StartMenuContent, TEXT("ContinuePilgrimageButton"), TEXT("Continue Journey"), StartButtonLabel);
    if (StartContinueButton != nullptr)
    {
        StartContinueButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleContinuePilgrimagePressed);
    }

    StartOptionsButton = CreateMenuButton(WidgetTree, StartMenuContent, TEXT("OptionsButton"), TEXT("Options"), StartButtonLabel);
    if (StartOptionsButton != nullptr)
    {
        StartOptionsButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleOptionsPressed);
    }

    StartQuitButton = CreateMenuButton(WidgetTree, StartMenuContent, TEXT("QuitButton"), TEXT("Quit"), StartButtonLabel);
    if (StartQuitButton != nullptr)
    {
        StartQuitButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleQuitPressed);
    }

    // Start menu gamepad navigation chain (wrapping)
    {
        UButton* StartButtons[] = { StartNewGameButton, StartContinueButton, StartOptionsButton, StartQuitButton };
        const int32 StartCount = UE_ARRAY_COUNT(StartButtons);
        for (int32 i = 0; i < StartCount; ++i)
        {
            if (StartButtons[i] == nullptr) { continue; }
            int32 NextIndex = (i + 1) % StartCount;
            int32 PrevIndex = (i - 1 + StartCount) % StartCount;
            if (StartButtons[NextIndex] != nullptr)
            {
                StartButtons[i]->SetNavigationRuleExplicit(EUINavigation::Down, StartButtons[NextIndex]);
            }
            if (StartButtons[PrevIndex] != nullptr)
            {
                StartButtons[i]->SetNavigationRuleExplicit(EUINavigation::Up, StartButtons[PrevIndex]);
            }
        }
    }

    // ======================================================================
    // OPTIONS OVERLAY
    // ======================================================================
    OptionsOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("OptionsOverlay"));
    OptionsOverlay->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.94f));
    OptionsOverlay->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* OptionsOverlaySlot = RootPanel->AddChildToCanvas(OptionsOverlay);
    if (OptionsOverlaySlot != nullptr)
    {
        OptionsOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        OptionsOverlaySlot->SetOffsets(FMargin(0.0f));
    }

    UCanvasPanel* OptionsCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("OptionsCanvas"));
    OptionsOverlay->SetContent(OptionsCanvas);

    UBorder* OptionsPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("OptionsPanel"));
    OptionsPanel->SetBrushColor(FLinearColor(0.06f, 0.06f, 0.04f, 0.98f));
    UCanvasPanelSlot* OptionsPanelSlot = OptionsCanvas->AddChildToCanvas(OptionsPanel);
    if (OptionsPanelSlot != nullptr)
    {
        FVector2D ViewportSize(1920.0f, 1080.0f);
        if (APlayerController* PC = GetOwningPlayer())
        {
            int32 ViewportX = 0;
            int32 ViewportY = 0;
            PC->GetViewportSize(ViewportX, ViewportY);
            if (ViewportX > 0 && ViewportY > 0)
            {
                ViewportSize = FVector2D(float(ViewportX), float(ViewportY));
            }
        }

        const float ResponsiveWidth = FMath::Clamp(ViewportSize.X * 0.38f, 580.0f, 800.0f);
        const float ResponsiveHeight = FMath::Clamp(ViewportSize.Y * 0.82f, 700.0f, 980.0f);
        OptionsPanelSlot->SetSize(FVector2D(ResponsiveWidth, ResponsiveHeight));
        OptionsPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        OptionsPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        OptionsPanelSlot->SetPosition(FVector2D::ZeroVector);
    }

    UVerticalBox* OptionsContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("OptionsContent"));
    OptionsPanel->SetContent(OptionsContent);

    UTextBlock* OptionsTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OptionsTitle"));
    ConfigureText(OptionsTitle, TEXT("OPTIONS"), FLinearColor(0.90f, 0.84f, 0.68f), 24);
    OptionsTitle->SetJustification(ETextJustify::Center);
    AddVerticalChild(OptionsContent, OptionsTitle, FMargin(16.0f, 16.0f, 16.0f, 4.0f));

    AddGoldSeparator(WidgetTree, OptionsContent, TEXT("OptionsTitleSep"), FMargin(40.0f, 0.0f, 40.0f, 6.0f));

    OptionsSummaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OptionsSummaryText"));
    ConfigureText(OptionsSummaryText, TEXT("Loading settings..."), FLinearColor(0.78f, 0.74f, 0.66f), 13);
    OptionsSummaryText->SetAutoWrapText(true);
    AddVerticalChild(OptionsContent, OptionsSummaryText, FMargin(18.0f, 4.0f, 18.0f, 8.0f));

    UTextBlock* OptionsButtonLabel = nullptr;
    TArray<UButton*> OptionsButtons;

    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("SensitivityDownButton"), TEXT("Sensitivity -"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSensitivityDownPressed);
        FirstOptionsButton = Button;
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("SensitivityUpButton"), TEXT("Sensitivity +"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSensitivityUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("InvertLookYButton"), TEXT("Toggle Invert Look Y"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleInvertLookYPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("FovDownButton"), TEXT("Field Of View -"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFovDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("FovUpButton"), TEXT("Field Of View +"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFovUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("GammaDownButton"), TEXT("Gamma -"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleGammaDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("GammaUpButton"), TEXT("Gamma +"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleGammaUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("VolumeDownButton"), TEXT("Master Volume -"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleVolumeDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("VolumeUpButton"), TEXT("Master Volume +"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleVolumeUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("FrameLimitDownButton"), TEXT("Frame Limit -"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFrameLimitDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("FrameLimitUpButton"), TEXT("Frame Limit +"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFrameLimitUpPressed);
        OptionsButtons.Add(Button);
    }

    AddGoldSeparator(WidgetTree, OptionsContent, TEXT("AccessSep"), FMargin(20.0f, 4.0f, 20.0f, 4.0f));

    UTextBlock* AccessibilityHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AccessibilityHeader"));
    ConfigureText(AccessibilityHeader, TEXT("Accessibility"), FLinearColor(0.90f, 0.84f, 0.68f), 15);
    AccessibilityHeader->SetJustification(ETextJustify::Center);
    AddVerticalChild(OptionsContent, AccessibilityHeader, FMargin(12.0f, 2.0f, 12.0f, 4.0f));

    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("SubtitleScaleDownButton"), TEXT("Subtitle Size -"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSubtitleScaleDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("SubtitleScaleUpButton"), TEXT("Subtitle Size +"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSubtitleScaleUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("ColorblindToggleButton"), TEXT("Colorblind Mode"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleColorblindTogglePressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("HighContrastToggleButton"), TEXT("High Contrast HUD"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleHighContrastTogglePressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("ScreenShakeToggleButton"), TEXT("Screen Shake Reduction"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleScreenShakeTogglePressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("HUDScaleDownButton"), TEXT("HUD Scale -"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleHUDScaleDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("HUDScaleUpButton"), TEXT("HUD Scale +"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleHUDScaleUpPressed);
        OptionsButtons.Add(Button);
    }

    AddGoldSeparator(WidgetTree, OptionsContent, TEXT("OptionsBottomSep"), FMargin(20.0f, 4.0f, 20.0f, 4.0f));

    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("OptionsApplyButton"), TEXT("Apply And Save"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleOptionsApplyPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateCompactMenuButton(WidgetTree, OptionsContent, TEXT("OptionsBackButton"), TEXT("Back"), OptionsButtonLabel, 15))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleOptionsBackPressed);
        OptionsButtons.Add(Button);
    }

    // Options menu gamepad navigation chain (wrapping)
    {
        const int32 OptCount = OptionsButtons.Num();
        for (int32 i = 0; i < OptCount; ++i)
        {
            if (OptionsButtons[i] == nullptr) { continue; }
            int32 NextIndex = (i + 1) % OptCount;
            int32 PrevIndex = (i - 1 + OptCount) % OptCount;
            if (OptionsButtons[NextIndex] != nullptr)
            {
                OptionsButtons[i]->SetNavigationRuleExplicit(EUINavigation::Down, OptionsButtons[NextIndex]);
            }
            if (OptionsButtons[PrevIndex] != nullptr)
            {
                OptionsButtons[i]->SetNavigationRuleExplicit(EUINavigation::Up, OptionsButtons[PrevIndex]);
            }
        }
    }

    // ======================================================================
    // DEATH OVERLAY
    // ======================================================================
    DeathOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DeathOverlay"));
    DeathOverlay->SetBrushColor(FLinearColor(0.02f, 0.01f, 0.01f, 0.90f));
    DeathOverlay->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* DeathOverlaySlot = RootPanel->AddChildToCanvas(DeathOverlay);
    if (DeathOverlaySlot != nullptr)
    {
        DeathOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        DeathOverlaySlot->SetOffsets(FMargin(0.0f));
    }

    UVerticalBox* DeathContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DeathContent"));
    DeathOverlay->SetContent(DeathContent);

    UTextBlock* DeathTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DeathTitleText"));
    ConfigureText(DeathTitleText, TEXT("YOU WERE STRUCK DOWN"), FLinearColor(0.85f, 0.22f, 0.16f), 32);
    DeathTitleText->SetJustification(ETextJustify::Center);
    AddVerticalChild(DeathContent, DeathTitleText, FMargin(16.0f, 260.0f, 16.0f, 12.0f));

    DeathRetryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DeathRetryText"));
    ConfigureText(DeathRetryText, TEXT("Attempt 1 begins."), FLinearColor(0.88f, 0.82f, 0.70f), 16);
    DeathRetryText->SetJustification(ETextJustify::Center);
    AddVerticalChild(DeathContent, DeathRetryText, FMargin(16.0f, 0.0f, 16.0f, 24.0f));

    UTextBlock* RiseButtonLabel = nullptr;
    RiseAgainButton = CreateMenuButton(WidgetTree, DeathContent, TEXT("RiseAgainButton"), TEXT("Rise Again"), RiseButtonLabel);
    if (RiseAgainButton != nullptr)
    {
        RiseAgainButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleRiseAgainPressed);
    }

    // ======================================================================
    // LOADING OVERLAY
    // ======================================================================
    LoadingOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("LoadingOverlay"));
    LoadingOverlay->SetBrushColor(FLinearColor(0.01f, 0.01f, 0.01f, 0.92f));
    LoadingOverlay->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* LoadingOverlaySlot = RootPanel->AddChildToCanvas(LoadingOverlay);
    if (LoadingOverlaySlot != nullptr)
    {
        LoadingOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        LoadingOverlaySlot->SetOffsets(FMargin(0.0f));
    }

    UVerticalBox* LoadingContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("LoadingContent"));
    LoadingOverlay->SetContent(LoadingContent);

    UTextBlock* LoadingTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LoadingTitle"));
    ConfigureText(LoadingTitle, TEXT("Loading Pilgrimage..."), FLinearColor(0.90f, 0.84f, 0.72f), 28);
    LoadingTitle->SetJustification(ETextJustify::Center);
    AddVerticalChild(LoadingContent, LoadingTitle, FMargin(20.0f, 260.0f, 20.0f, 18.0f));

    LoadingTipText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LoadingTipText"));
    ConfigureText(LoadingTipText, TEXT("Lore Tip"), FLinearColor(0.78f, 0.74f, 0.62f), 16);
    LoadingTipText->SetJustification(ETextJustify::Center);
    LoadingTipText->SetAutoWrapText(true);
    AddVerticalChild(LoadingContent, LoadingTipText, FMargin(120.0f, 0.0f, 120.0f, 12.0f));

    // ======================================================================
    // SKILL TREE WIDGET
    // ======================================================================
    APlayerController* SkillTreePC = GetOwningPlayer();
    if (SkillTreePC != nullptr)
    {
        SkillTreeWidget = CreateWidget<UNazareneSkillTreeWidget>(SkillTreePC, UNazareneSkillTreeWidget::StaticClass());
        if (SkillTreeWidget != nullptr)
        {
            SkillTreeWidget->AddToViewport(50);
            SkillTreeWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    RefreshSlotSummaries();
    RefreshOptionsSummary();
}

void UNazareneHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    CachedDeltaTime = InDeltaTime;

    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
    RefreshVitals(Player);
    SyncEnemyHealthBars(Player);

    if (MessageTimer > 0.0f)
    {
        MessageTimer = FMath::Max(0.0f, MessageTimer - InDeltaTime);
        if (MessageText != nullptr)
        {
            MessageText->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else if (MessageText != nullptr)
    {
        MessageText->SetVisibility(ESlateVisibility::Collapsed);
    }

    for (int32 Index = DamageNumberWidgets.Num() - 1; Index >= 0; --Index)
    {
        UNazareneDamageNumberWidget* Widget = DamageNumberWidgets[Index];
        if (Widget == nullptr || Widget->IsExpired())
        {
            DamageNumberWidgets.RemoveAtSwap(Index);
        }
    }
}

void UNazareneHUDWidget::SetRegionName(const FString& InRegionName)
{
    CachedRegionName = InRegionName;
    if (RegionNameText != nullptr)
    {
        RegionNameText->SetText(FText::FromString(CachedRegionName));
    }
}

void UNazareneHUDWidget::SetObjective(const FString& InObjective)
{
    CachedObjective = InObjective;
    if (ObjectiveText != nullptr)
    {
        ObjectiveText->SetText(FText::FromString(CachedObjective));
    }
}

void UNazareneHUDWidget::ShowMessage(const FString& InMessage, float Duration)
{
    if (MessageText != nullptr)
    {
        MessageText->SetText(FText::FromString(InMessage));
        MessageText->SetVisibility(ESlateVisibility::Visible);
    }
    MessageTimer = FMath::Max(0.0f, Duration);
}

void UNazareneHUDWidget::ShowDamageNumber(const FVector& WorldLocation, float Amount, ENazareneDamageNumberType Type)
{
    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController == nullptr)
    {
        return;
    }

    UNazareneDamageNumberWidget* DamageWidget = CreateWidget<UNazareneDamageNumberWidget>(PlayerController, UNazareneDamageNumberWidget::StaticClass());
    if (DamageWidget == nullptr)
    {
        return;
    }

    DamageWidget->AddToViewport(70);
    DamageWidget->InitializeDamageNumber(PlayerController, WorldLocation, Amount, Type);
    DamageNumberWidgets.Add(DamageWidget);
}

void UNazareneHUDWidget::ShowDeathOverlay(int32 RetryCount)
{
    if (DeathRetryText != nullptr)
    {
        DeathRetryText->SetText(FText::FromString(FString::Printf(TEXT("Attempt %d begins at the nearest prayer site."), FMath::Max(1, RetryCount))));
    }

    if (DeathOverlay != nullptr)
    {
        DeathOverlay->SetVisibility(ESlateVisibility::Visible);
    }

    if (RiseAgainButton != nullptr)
    {
        RiseAgainButton->SetKeyboardFocus();
    }
}

void UNazareneHUDWidget::SetLoadingOverlayVisible(bool bVisible, const FString& LoreTip)
{
    if (LoadingTipText != nullptr && !LoreTip.IsEmpty())
    {
        LoadingTipText->SetText(FText::FromString(LoreTip));
    }

    if (LoadingOverlay != nullptr)
    {
        LoadingOverlay->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UNazareneHUDWidget::SetStartMenuVisible(bool bVisible)
{
    if (StartMenuOverlay != nullptr)
    {
        StartMenuOverlay->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (bVisible && PauseOverlay != nullptr)
    {
        PauseOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (!bVisible && OptionsOverlay != nullptr && bOptionsOpenedFromStartMenu)
    {
        OptionsOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (bVisible && StartNewGameButton != nullptr)
    {
        StartNewGameButton->SetKeyboardFocus();
    }
}

bool UNazareneHUDWidget::IsStartMenuVisible() const
{
    return StartMenuOverlay != nullptr && StartMenuOverlay->GetVisibility() == ESlateVisibility::Visible;
}

void UNazareneHUDWidget::SetPauseMenuVisible(bool bVisible)
{
    if (IsStartMenuVisible() && bVisible)
    {
        return;
    }

    if (PauseOverlay != nullptr)
    {
        PauseOverlay->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (bVisible)
    {
        RefreshSlotSummaries();

        if (PauseResumeButton != nullptr)
        {
            PauseResumeButton->SetKeyboardFocus();
        }
    }
    else if (OptionsOverlay != nullptr && !bOptionsOpenedFromStartMenu)
    {
        OptionsOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }
}

bool UNazareneHUDWidget::IsPauseMenuVisible() const
{
    return PauseOverlay != nullptr && PauseOverlay->GetVisibility() == ESlateVisibility::Visible;
}

void UNazareneHUDWidget::RefreshSlotSummaries()
{
    UNazareneSaveSubsystem* SaveSubsystem = nullptr;
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        SaveSubsystem = GameInstance->GetSubsystem<UNazareneSaveSubsystem>();
    }

    if (SaveSubsystem == nullptr)
    {
        if (SlotSummary1Text != nullptr)
        {
            SlotSummary1Text->SetText(FText::FromString(TEXT("Slot 1: unavailable")));
        }
        if (SlotSummary2Text != nullptr)
        {
            SlotSummary2Text->SetText(FText::FromString(TEXT("Slot 2: unavailable")));
        }
        if (SlotSummary3Text != nullptr)
        {
            SlotSummary3Text->SetText(FText::FromString(TEXT("Slot 3: unavailable")));
        }
        return;
    }

    if (SlotSummary1Text != nullptr)
    {
        SlotSummary1Text->SetText(FText::FromString(SaveSubsystem->GetSlotSummary(1)));
    }
    if (SlotSummary2Text != nullptr)
    {
        SlotSummary2Text->SetText(FText::FromString(SaveSubsystem->GetSlotSummary(2)));
    }
    if (SlotSummary3Text != nullptr)
    {
        SlotSummary3Text->SetText(FText::FromString(SaveSubsystem->GetSlotSummary(3)));
    }
}

void UNazareneHUDWidget::RefreshOptionsSummary()
{
    if (OptionsSummaryText == nullptr)
    {
        return;
    }

    UNazareneSettingsSubsystem* SettingsSubsystem = nullptr;
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        SettingsSubsystem = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>();
    }

    if (SettingsSubsystem == nullptr)
    {
        OptionsSummaryText->SetText(FText::FromString(TEXT("Settings subsystem unavailable.")));
        return;
    }

    const FNazarenePlayerSettings& Settings = SettingsSubsystem->GetSettings();

    const TCHAR* ColorblindModeNames[] = { TEXT("None"), TEXT("Deuteranopia"), TEXT("Protanopia"), TEXT("Tritanopia") };
    const int32 ColorblindIndex = FMath::Clamp(static_cast<int32>(Settings.ColorblindMode), 0, 3);

    const FString Summary = FString::Printf(
        TEXT("Sensitivity: %.2f | Invert Y: %s | FOV: %.0f\nGamma: %.2f | Volume: %.0f%% | FPS Cap: %.0f\nSubtitles: %.2f | Colorblind: %s\nContrast: %s | Shake: %s | HUD: %.2f"),
        Settings.MouseSensitivity,
        Settings.bInvertLookY ? TEXT("On") : TEXT("Off"),
        Settings.FieldOfView,
        Settings.DisplayGamma,
        Settings.MasterVolume * 100.0f,
        Settings.FrameRateLimit,
        Settings.SubtitleTextScale,
        ColorblindModeNames[ColorblindIndex],
        Settings.bHighContrastHUD ? TEXT("On") : TEXT("Off"),
        Settings.bScreenShakeReduction ? TEXT("On") : TEXT("Off"),
        Settings.HUDScale
    );
    OptionsSummaryText->SetText(FText::FromString(Summary));
}

void UNazareneHUDWidget::RefreshVitals(const ANazarenePlayerCharacter* Player)
{
    if (Player == nullptr)
    {
        return;
    }

    const float MaxHealth = Player->GetMaxHealth();
    const float MaxStamina = Player->GetMaxStamina();
    const float Health = Player->GetHealth();
    const float Stamina = Player->GetStamina();

    const float HealthRatio = MaxHealth > 0.0f ? Health / MaxHealth : 0.0f;
    const float StaminaRatio = MaxStamina > 0.0f ? Stamina / MaxStamina : 0.0f;

    // Compact bar labels
    if (HealthText != nullptr)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("HP  %.0f / %.0f"), Health, MaxHealth)));
    }
    if (StaminaText != nullptr)
    {
        StaminaText->SetText(FText::FromString(FString::Printf(TEXT("STA  %.0f / %.0f"), Stamina, MaxStamina)));
    }
    if (FaithText != nullptr)
    {
        FaithText->SetText(FText::FromString(FString::Printf(TEXT("FTH  %.0f"), Player->GetFaith())));
    }

    // Smooth bar interpolation
    const float TargetHealthPercent = FMath::Clamp(HealthRatio, 0.0f, 1.0f);
    DisplayedHealthPercent = FMath::FInterpTo(DisplayedHealthPercent, TargetHealthPercent, CachedDeltaTime, BarLerpSpeed);
    if (HealthBar)
    {
        HealthBar->SetPercent(DisplayedHealthPercent);
    }

    const float TargetStaminaPercent = FMath::Clamp(StaminaRatio, 0.0f, 1.0f);
    DisplayedStaminaPercent = FMath::FInterpTo(DisplayedStaminaPercent, TargetStaminaPercent, CachedDeltaTime, BarLerpSpeed);
    if (StaminaBar)
    {
        StaminaBar->SetPercent(DisplayedStaminaPercent);
    }

    // Low-resource pulse effects
    bHealthCritical = TargetHealthPercent <= 0.25f;
    if (bHealthCritical)
    {
        HealthPulseTimer += CachedDeltaTime;
        const float PulseAlpha = 0.5f + 0.5f * FMath::Sin(HealthPulseTimer * 6.0f);
        if (HealthBar)
        {
            HealthBar->SetFillColorAndOpacity(FLinearColor(0.92f, 0.18f * PulseAlpha, 0.12f * PulseAlpha, 1.0f));
        }
    }
    else
    {
        HealthPulseTimer = 0.0f;
        if (HealthBar)
        {
            HealthBar->SetFillColorAndOpacity(FLinearColor(0.78f, 0.18f, 0.14f, 1.0f));
        }
    }

    bStaminaCritical = TargetStaminaPercent <= 0.15f;
    if (bStaminaCritical)
    {
        StaminaPulseTimer += CachedDeltaTime;
        const float StaminaPulse = 0.5f + 0.5f * FMath::Sin(StaminaPulseTimer * 6.0f);
        if (StaminaBar)
        {
            StaminaBar->SetFillColorAndOpacity(FLinearColor(0.18f, 0.58f * StaminaPulse, 0.16f * StaminaPulse, 1.0f));
        }
    }
    else
    {
        StaminaPulseTimer = 0.0f;
        if (StaminaBar)
        {
            StaminaBar->SetFillColorAndOpacity(FLinearColor(0.22f, 0.62f, 0.20f, 1.0f));
        }
    }

    if (FaithBar != nullptr && Player != nullptr)
    {
        const float MaxFaith = Player->StartingFaith * 2.0f;
        const float FaithPercent = MaxFaith > 0.0f ? FMath::Clamp(Player->GetFaith() / MaxFaith, 0.0f, 1.0f) : 0.0f;
        FaithBar->SetPercent(FaithPercent);
    }

    if (Player != nullptr)
    {
        if (HealCooldownBar != nullptr)
        {
            const float HealRemaining = Player->GetHealCooldownRemaining();
            const float HealMax = Player->HealCooldown;
            HealCooldownBar->SetPercent(HealMax > 0.0f ? FMath::Clamp(1.0f - (HealRemaining / HealMax), 0.0f, 1.0f) : 1.0f);
        }
        if (BlessingCooldownBar != nullptr)
        {
            const float BlessingRemaining = Player->GetBlessingCooldownRemaining();
            const float BlessingMax = Player->BlessingCooldown;
            BlessingCooldownBar->SetPercent(BlessingMax > 0.0f ? FMath::Clamp(1.0f - (BlessingRemaining / BlessingMax), 0.0f, 1.0f) : 1.0f);
        }
        if (RadianceCooldownBar != nullptr)
        {
            const float RadianceRemaining = Player->GetRadianceCooldownRemaining();
            const float RadianceMax = Player->RadianceCooldown;
            RadianceCooldownBar->SetPercent(RadianceMax > 0.0f ? FMath::Clamp(1.0f - (RadianceRemaining / RadianceMax), 0.0f, 1.0f) : 1.0f);
        }
    }

    // Lock-on target display
    if (LockTargetText != nullptr)
    {
        const FString TargetName = Player->GetLockTargetName();
        if (TargetName.IsEmpty())
        {
            LockTargetText->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            LockTargetText->SetText(FText::FromString(FString::Printf(TEXT("Target: %s"), *TargetName)));
            LockTargetText->SetVisibility(ESlateVisibility::Visible);
        }
    }

    // Context hint (only show when there's a hint)
    if (ContextHintText != nullptr)
    {
        const FString Hint = Player->GetContextHint();
        if (Hint.IsEmpty())
        {
            ContextHintText->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            ContextHintText->SetText(FText::FromString(Hint));
            ContextHintText->SetVisibility(ESlateVisibility::Visible);
        }
    }

    // Critical state warning
    if (CriticalStateText != nullptr)
    {
        FString CriticalState;
        FLinearColor CriticalColor(0.95f, 0.40f, 0.28f, 1.0f);

        if (HealthRatio <= 0.25f)
        {
            CriticalState = TEXT("CRITICAL HEALTH");
            CriticalColor = FLinearColor(0.92f, 0.20f, 0.14f, 1.0f);
        }
        else if (StaminaRatio <= 0.15f)
        {
            CriticalState = TEXT("EXHAUSTED");
            CriticalColor = FLinearColor(0.90f, 0.65f, 0.18f, 1.0f);
        }

        if (CriticalState.IsEmpty())
        {
            CriticalStateText->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            CriticalStateText->SetText(FText::FromString(CriticalState));
            CriticalStateText->SetColorAndOpacity(FSlateColor(CriticalColor));
            CriticalStateText->SetVisibility(ESlateVisibility::Visible);
        }
    }

    // Compact combat state (level + miracles)
    if (CombatStateText != nullptr)
    {
        const FString BlessingState = Player->IsMiracleUnlocked(FName(TEXT("blessing")))
            ? FString::Printf(TEXT("Bless %.0fs"), Player->GetBlessingCooldownRemaining())
            : TEXT("Bless --");
        const FString RadianceState = Player->IsMiracleUnlocked(FName(TEXT("radiance")))
            ? FString::Printf(TEXT("Rad %.0fs"), Player->GetRadianceCooldownRemaining())
            : TEXT("Rad --");

        CombatStateText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Lv%d  XP %d/%d  SP %d  |  Heal %.0fs  %s  %s"),
                    Player->GetPlayerLevel(),
                    Player->GetTotalXP(),
                    Player->GetXPToNextLevel(),
                    Player->GetSkillPoints(),
                    Player->GetHealCooldownRemaining(),
                    *BlessingState,
                    *RadianceState
                )
            )
        );
    }
}

void UNazareneHUDWidget::SyncEnemyHealthBars(ANazarenePlayerCharacter* Player)
{
    if (Player == nullptr || GetOwningPlayer() == nullptr || GetWorld() == nullptr)
    {
        return;
    }

    TArray<ANazareneEnemyCharacter*> Enemies;
    for (TActorIterator<ANazareneEnemyCharacter> It(GetWorld()); It; ++It)
    {
        ANazareneEnemyCharacter* Enemy = *It;
        if (Enemy != nullptr && !Enemy->IsRedeemed())
        {
            Enemies.Add(Enemy);
        }
    }

    for (ANazareneEnemyCharacter* Enemy : Enemies)
    {
        bool bHasWidget = false;
        for (UNazareneEnemyHealthBarWidget* HealthWidget : EnemyHealthBarWidgets)
        {
            if (HealthWidget != nullptr && HealthWidget->GetBoundEnemy() == Enemy)
            {
                bHasWidget = true;
                break;
            }
        }

        if (bHasWidget)
        {
            continue;
        }

        UNazareneEnemyHealthBarWidget* NewWidget = CreateWidget<UNazareneEnemyHealthBarWidget>(GetOwningPlayer(), UNazareneEnemyHealthBarWidget::StaticClass());
        if (NewWidget == nullptr)
        {
            continue;
        }

        NewWidget->AddToViewport(30);
        NewWidget->BindToEnemy(Enemy, Player);
        EnemyHealthBarWidgets.Add(NewWidget);
    }

    for (int32 Index = EnemyHealthBarWidgets.Num() - 1; Index >= 0; --Index)
    {
        UNazareneEnemyHealthBarWidget* Widget = EnemyHealthBarWidgets[Index];
        ANazareneEnemyCharacter* BoundEnemy = Widget != nullptr ? Widget->GetBoundEnemy() : nullptr;
        if (Widget == nullptr || BoundEnemy == nullptr || BoundEnemy->IsRedeemed() || !Enemies.Contains(BoundEnemy))
        {
            if (Widget != nullptr)
            {
                Widget->RemoveFromParent();
            }
            EnemyHealthBarWidgets.RemoveAtSwap(Index);
        }
    }
}

void UNazareneHUDWidget::HandleResumePressed()
{
    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PlayerController->GetHUD()))
        {
            HUD->SetPauseMenuVisible(false);
        }
    }
}

void UNazareneHUDWidget::HandleStartPilgrimagePressed()
{
    if (UWorld* World = GetWorld())
    {
        if (UNazareneGameInstance* Session = World->GetGameInstance<UNazareneGameInstance>())
        {
            Session->StartNewGame();
        }
    }

    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PlayerController->GetHUD()))
        {
            HUD->SetStartMenuVisible(false);
        }
    }

    if (UWorld* World = GetWorld())
    {
        if (ANazareneCampaignGameMode* GM = Cast<ANazareneCampaignGameMode>(World->GetAuthGameMode()))
        {
            GM->OnMenuDismissed();
        }
    }
}

void UNazareneHUDWidget::HandleContinuePilgrimagePressed()
{
    bool bLoaded = false;
    FString LoadedSource;

    if (UWorld* World = GetWorld())
    {
        if (ANazareneCampaignGameMode* CampaignMode = Cast<ANazareneCampaignGameMode>(World->GetAuthGameMode()))
        {
            if (CampaignMode->LoadCheckpoint())
            {
                bLoaded = true;
                LoadedSource = TEXT("checkpoint");
            }
            else
            {
                for (int32 SlotId = 1; SlotId <= 3; ++SlotId)
                {
                    if (CampaignMode->LoadFromSlot(SlotId))
                    {
                        bLoaded = true;
                        LoadedSource = FString::Printf(TEXT("slot %d"), SlotId);
                        break;
                    }
                }
            }
        }
    }

    if (!bLoaded)
    {
        ShowMessage(TEXT("No checkpoint or save slots found. Start a new game."), 4.5f);
        return;
    }

    ShowMessage(FString::Printf(TEXT("Continuing from %s."), *LoadedSource), 3.0f);
    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PlayerController->GetHUD()))
        {
            HUD->SetStartMenuVisible(false);
        }
    }

    if (UWorld* World = GetWorld())
    {
        if (ANazareneCampaignGameMode* GM = Cast<ANazareneCampaignGameMode>(World->GetAuthGameMode()))
        {
            GM->OnMenuDismissed();
        }
    }
}

void UNazareneHUDWidget::HandleOptionsPressed()
{
    bOptionsOpenedFromStartMenu = IsStartMenuVisible();
    if (OptionsOverlay != nullptr)
    {
        OptionsOverlay->SetVisibility(ESlateVisibility::Visible);
    }
    RefreshOptionsSummary();

    if (FirstOptionsButton != nullptr)
    {
        FirstOptionsButton->SetKeyboardFocus();
    }
}

void UNazareneHUDWidget::HandleOptionsBackPressed()
{
    if (OptionsOverlay != nullptr)
    {
        OptionsOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UNazareneHUDWidget::HandleOptionsApplyPressed()
{
    UNazareneSettingsSubsystem* SettingsSubsystem = nullptr;
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        SettingsSubsystem = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>();
    }

    if (SettingsSubsystem == nullptr)
    {
        ShowMessage(TEXT("Settings subsystem unavailable."), 3.5f);
        return;
    }

    SettingsSubsystem->ApplySettings();
    const bool bSaved = SettingsSubsystem->SaveSettings();
    ShowMessage(bSaved ? TEXT("Settings saved.") : TEXT("Settings applied, but save failed."), 3.5f);
    RefreshOptionsSummary();
}

void UNazareneHUDWidget::HandleSensitivityDownPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetMouseSensitivity(Settings->GetSettings().MouseSensitivity - 0.1f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleSensitivityUpPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetMouseSensitivity(Settings->GetSettings().MouseSensitivity + 0.1f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleInvertLookYPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetInvertLookY(!Settings->GetSettings().bInvertLookY);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleFovDownPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetFieldOfView(Settings->GetSettings().FieldOfView - 2.0f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleFovUpPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetFieldOfView(Settings->GetSettings().FieldOfView + 2.0f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleGammaDownPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetDisplayGamma(Settings->GetSettings().DisplayGamma - 0.05f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleGammaUpPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetDisplayGamma(Settings->GetSettings().DisplayGamma + 0.05f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleVolumeDownPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetMasterVolume(Settings->GetSettings().MasterVolume - 0.05f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleVolumeUpPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetMasterVolume(Settings->GetSettings().MasterVolume + 0.05f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleFrameLimitDownPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetFrameRateLimit(Settings->GetSettings().FrameRateLimit - 10.0f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleFrameLimitUpPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetFrameRateLimit(Settings->GetSettings().FrameRateLimit + 10.0f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleSubtitleScaleDownPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetSubtitleTextScale(Settings->GetSettings().SubtitleTextScale - 0.25f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleSubtitleScaleUpPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetSubtitleTextScale(Settings->GetSettings().SubtitleTextScale + 0.25f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleColorblindTogglePressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            int32 Current = static_cast<int32>(Settings->GetSettings().ColorblindMode);
            int32 Next = (Current + 1) % 4;
            Settings->SetColorblindMode(static_cast<ENazareneColorblindMode>(Next));
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleHighContrastTogglePressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetHighContrastHUD(!Settings->GetSettings().bHighContrastHUD);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleScreenShakeTogglePressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetScreenShakeReduction(!Settings->GetSettings().bScreenShakeReduction);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleHUDScaleDownPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetHUDScale(Settings->GetSettings().HUDScale - 0.25f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleHUDScaleUpPressed()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UNazareneSettingsSubsystem* Settings = GameInstance->GetSubsystem<UNazareneSettingsSubsystem>())
        {
            Settings->SetHUDScale(Settings->GetSettings().HUDScale + 0.25f);
            RefreshOptionsSummary();
        }
    }
}

void UNazareneHUDWidget::HandleQuitPressed()
{
    APlayerController* PlayerController = GetOwningPlayer();
    UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}

void UNazareneHUDWidget::HandleSaveSlot1Pressed()
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
    const bool bSaved = Player != nullptr && Player->SaveToSlot(1);
    const FString Hint = (Player != nullptr) ? Player->GetContextHint() : TEXT("Player unavailable.");
    ShowMessage(bSaved ? TEXT("Saved to slot 1.") : (Hint.IsEmpty() ? TEXT("Save failed for slot 1.") : Hint), 3.5f);
    RefreshSlotSummaries();
}

void UNazareneHUDWidget::HandleSaveSlot2Pressed()
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
    const bool bSaved = Player != nullptr && Player->SaveToSlot(2);
    const FString Hint = (Player != nullptr) ? Player->GetContextHint() : TEXT("Player unavailable.");
    ShowMessage(bSaved ? TEXT("Saved to slot 2.") : (Hint.IsEmpty() ? TEXT("Save failed for slot 2.") : Hint), 3.5f);
    RefreshSlotSummaries();
}

void UNazareneHUDWidget::HandleSaveSlot3Pressed()
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
    const bool bSaved = Player != nullptr && Player->SaveToSlot(3);
    const FString Hint = (Player != nullptr) ? Player->GetContextHint() : TEXT("Player unavailable.");
    ShowMessage(bSaved ? TEXT("Saved to slot 3.") : (Hint.IsEmpty() ? TEXT("Save failed for slot 3.") : Hint), 3.5f);
    RefreshSlotSummaries();
}

void UNazareneHUDWidget::HandleLoadSlot1Pressed()
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
    const bool bLoaded = Player != nullptr && Player->LoadFromSlot(1);
    const FString Hint = (Player != nullptr) ? Player->GetContextHint() : TEXT("Player unavailable.");
    ShowMessage(bLoaded ? TEXT("Loaded slot 1.") : (Hint.IsEmpty() ? TEXT("Load failed for slot 1.") : Hint), 3.5f);
    RefreshSlotSummaries();

    if (bLoaded)
    {
        HandleResumePressed();
    }
}

void UNazareneHUDWidget::HandleLoadSlot2Pressed()
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
    const bool bLoaded = Player != nullptr && Player->LoadFromSlot(2);
    const FString Hint = (Player != nullptr) ? Player->GetContextHint() : TEXT("Player unavailable.");
    ShowMessage(bLoaded ? TEXT("Loaded slot 2.") : (Hint.IsEmpty() ? TEXT("Load failed for slot 2.") : Hint), 3.5f);
    RefreshSlotSummaries();

    if (bLoaded)
    {
        HandleResumePressed();
    }
}

void UNazareneHUDWidget::HandleLoadSlot3Pressed()
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
    const bool bLoaded = Player != nullptr && Player->LoadFromSlot(3);
    const FString Hint = (Player != nullptr) ? Player->GetContextHint() : TEXT("Player unavailable.");
    ShowMessage(bLoaded ? TEXT("Loaded slot 3.") : (Hint.IsEmpty() ? TEXT("Load failed for slot 3.") : Hint), 3.5f);
    RefreshSlotSummaries();

    if (bLoaded)
    {
        HandleResumePressed();
    }
}

void UNazareneHUDWidget::HandleNewPilgrimagePressed()
{
    if (UWorld* World = GetWorld())
    {
        if (UNazareneGameInstance* Session = World->GetGameInstance<UNazareneGameInstance>())
        {
            Session->StartNewGame();
        }

        if (APlayerController* PlayerController = GetOwningPlayer())
        {
            if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PlayerController->GetHUD()))
            {
                HUD->SetPauseMenuVisible(false);
            }
        }

        const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
        UGameplayStatics::OpenLevel(this, FName(*LevelName));
    }
}

void UNazareneHUDWidget::HandleRiseAgainPressed()
{
    if (DeathOverlay != nullptr)
    {
        DeathOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UNazareneHUDWidget::SetSkillTreeVisible(bool bVisible)
{
    if (SkillTreeWidget == nullptr)
    {
        return;
    }

    SkillTreeWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    if (bVisible)
    {
        ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(GetOwningPlayerPawn());
        SkillTreeWidget->SetPlayerCharacter(Player);
        SkillTreeWidget->RefreshSkillTree();
    }
}

bool UNazareneHUDWidget::IsSkillTreeVisible() const
{
    return SkillTreeWidget != nullptr && SkillTreeWidget->GetVisibility() == ESlateVisibility::Visible;
}

void UNazareneHUDWidget::HandleSkillTreeClosePressed()
{
    SetSkillTreeVisible(false);
}
