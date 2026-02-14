#include "NazareneHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
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

        const FName LabelName(*FString::Printf(TEXT("%sLabel"), Name));
        OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), LabelName);
        ConfigureText(OutLabel, Label, FLinearColor(0.95f, 0.90f, 0.82f), 16);
        OutLabel->SetJustification(ETextJustify::Center);
        Button->AddChild(OutLabel);

        AddVerticalChild(Parent, Button, FMargin(0.0f, 4.0f));
        return Button;
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

    UBorder* PlayerPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PlayerPanel"));
    PlayerPanel->SetBrushColor(FLinearColor(0.07f, 0.07f, 0.06f, 0.74f));
    UCanvasPanelSlot* PlayerPanelSlot = RootPanel->AddChildToCanvas(PlayerPanel);
    if (PlayerPanelSlot != nullptr)
    {
        PlayerPanelSlot->SetSize(FVector2D(470.0f, 380.0f));
        PlayerPanelSlot->SetPosition(FVector2D(24.0f, 18.0f));
    }

    UVerticalBox* PlayerPanelContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PlayerPanelContent"));
    PlayerPanel->SetContent(PlayerPanelContent);

    UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
    ConfigureText(TitleText, TEXT("The Nazarene - Unreal Campaign"), FLinearColor(0.95f, 0.90f, 0.78f), 17);
    AddVerticalChild(PlayerPanelContent, TitleText, FMargin(14.0f, 12.0f, 12.0f, 4.0f));

    HealthText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HealthText"));
    ConfigureText(HealthText, TEXT("Health 0 / 0"), FLinearColor::White, 15);
    AddVerticalChild(PlayerPanelContent, HealthText, FMargin(14.0f, 4.0f, 12.0f, 0.0f));

    HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
    HealthBar->SetPercent(1.0f);
    HealthBar->SetFillColorAndOpacity(FLinearColor(0.83f, 0.24f, 0.20f, 1.0f));
    AddVerticalChild(PlayerPanelContent, HealthBar, FMargin(14.0f, 2.0f, 14.0f, 6.0f));

    StaminaText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StaminaText"));
    ConfigureText(StaminaText, TEXT("Stamina 0 / 0"), FLinearColor::White, 15);
    AddVerticalChild(PlayerPanelContent, StaminaText, FMargin(14.0f, 4.0f, 12.0f, 0.0f));

    StaminaBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("StaminaBar"));
    StaminaBar->SetPercent(1.0f);
    StaminaBar->SetFillColorAndOpacity(FLinearColor(0.30f, 0.78f, 0.34f, 1.0f));
    AddVerticalChild(PlayerPanelContent, StaminaBar, FMargin(14.0f, 2.0f, 14.0f, 6.0f));

    FaithBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("FaithBar"));
    if (FaithBar != nullptr)
    {
        FaithBar->SetPercent(1.0f);
        FaithBar->SetFillColorAndOpacity(FLinearColor(0.93f, 0.86f, 0.54f, 1.0f));
        AddVerticalChild(PlayerPanelContent, FaithBar, FMargin(14.0f, 2.0f, 14.0f, 6.0f));
    }

    FaithText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FaithText"));
    ConfigureText(FaithText, TEXT("Faith 0"), FLinearColor(0.93f, 0.86f, 0.54f), 15);
    AddVerticalChild(PlayerPanelContent, FaithText, FMargin(14.0f, 3.0f, 12.0f, 0.0f));

    LockTargetText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LockTargetText"));
    ConfigureText(LockTargetText, TEXT("Lock-On None"), FLinearColor(0.88f, 0.88f, 0.88f), 15);
    AddVerticalChild(PlayerPanelContent, LockTargetText, FMargin(14.0f, 3.0f, 12.0f, 0.0f));

    ContextHintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContextHintText"));
    ConfigureText(ContextHintText, TEXT(""), FLinearColor(0.92f, 0.82f, 0.66f), 14);
    AddVerticalChild(PlayerPanelContent, ContextHintText, FMargin(14.0f, 6.0f, 12.0f, 10.0f));

    CombatStateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CombatStateText"));
    ConfigureText(CombatStateText, TEXT("Cooldowns: Heal Ready | Blessing Locked | Radiance Locked"), FLinearColor(0.84f, 0.86f, 0.92f), 13);
    CombatStateText->SetAutoWrapText(true);
    AddVerticalChild(PlayerPanelContent, CombatStateText, FMargin(14.0f, 0.0f, 12.0f, 8.0f));

    HealCooldownBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealCooldownBar"));
    if (HealCooldownBar != nullptr)
    {
        HealCooldownBar->SetPercent(1.0f);
        HealCooldownBar->SetFillColorAndOpacity(FLinearColor(0.2f, 0.8f, 0.3f, 0.9f));
        AddVerticalChild(PlayerPanelContent, HealCooldownBar, FMargin(14.0f, 2.0f, 14.0f, 2.0f));
    }

    BlessingCooldownBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("BlessingCooldownBar"));
    if (BlessingCooldownBar != nullptr)
    {
        BlessingCooldownBar->SetPercent(1.0f);
        BlessingCooldownBar->SetFillColorAndOpacity(FLinearColor(0.9f, 0.8f, 0.3f, 0.9f));
        AddVerticalChild(PlayerPanelContent, BlessingCooldownBar, FMargin(14.0f, 2.0f, 14.0f, 2.0f));
    }

    RadianceCooldownBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("RadianceCooldownBar"));
    if (RadianceCooldownBar != nullptr)
    {
        RadianceCooldownBar->SetPercent(1.0f);
        RadianceCooldownBar->SetFillColorAndOpacity(FLinearColor(0.9f, 0.6f, 0.2f, 0.9f));
        AddVerticalChild(PlayerPanelContent, RadianceCooldownBar, FMargin(14.0f, 2.0f, 14.0f, 6.0f));
    }

    UBorder* ObjectivePanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ObjectivePanel"));
    ObjectivePanel->SetBrushColor(FLinearColor(0.07f, 0.07f, 0.06f, 0.74f));
    UCanvasPanelSlot* ObjectivePanelSlot = RootPanel->AddChildToCanvas(ObjectivePanel);
    if (ObjectivePanelSlot != nullptr)
    {
        ObjectivePanelSlot->SetSize(FVector2D(500.0f, 190.0f));
        ObjectivePanelSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
        ObjectivePanelSlot->SetAlignment(FVector2D(1.0f, 0.0f));
        ObjectivePanelSlot->SetPosition(FVector2D(-24.0f, 18.0f));
    }

    UVerticalBox* ObjectiveContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ObjectiveContent"));
    ObjectivePanel->SetContent(ObjectiveContent);

    RegionNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RegionNameText"));
    ConfigureText(RegionNameText, CachedRegionName, FLinearColor(0.95f, 0.90f, 0.78f), 17);
    AddVerticalChild(ObjectiveContent, RegionNameText, FMargin(14.0f, 12.0f, 12.0f, 4.0f));

    ObjectiveText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ObjectiveText"));
    ConfigureText(ObjectiveText, CachedObjective, FLinearColor(0.87f, 0.83f, 0.72f), 15);
    ObjectiveText->SetAutoWrapText(true);
    AddVerticalChild(ObjectiveContent, ObjectiveText, FMargin(14.0f, 3.0f, 12.0f, 8.0f));

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


    CriticalStateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CriticalStateText"));
    ConfigureText(CriticalStateText, TEXT(""), FLinearColor(0.95f, 0.40f, 0.28f), 19);
    CriticalStateText->SetJustification(ETextJustify::Center);
    CriticalStateText->SetVisibility(ESlateVisibility::Collapsed);
    UCanvasPanelSlot* CriticalStateSlot = RootPanel->AddChildToCanvas(CriticalStateText);
    if (CriticalStateSlot != nullptr)
    {
        CriticalStateSlot->SetSize(FVector2D(700.0f, 46.0f));
        CriticalStateSlot->SetAnchors(FAnchors(0.5f, 0.0f, 0.5f, 0.0f));
        CriticalStateSlot->SetAlignment(FVector2D(0.5f, 0.0f));
        CriticalStateSlot->SetPosition(FVector2D(0.0f, 72.0f));
    }

    UTextBlock* ControlsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ControlsText"));
    ConfigureText(
        ControlsText,
        TEXT("Move/Look: WASD + Mouse | Combat: LMB Light, RMB Heavy, Shift Block, F Parry, Space Dodge | Miracles: R Heal, 1 Blessing, 2 Radiance | Interaction: E Pray, Q Lock, T Skill Tree, Esc Menu"),
        FLinearColor(0.90f, 0.85f, 0.70f),
        13
    );
    ControlsText->SetAutoWrapText(true);
    UCanvasPanelSlot* ControlsSlot = RootPanel->AddChildToCanvas(ControlsText);
    if (ControlsSlot != nullptr)
    {
        ControlsSlot->SetSize(FVector2D(1300.0f, 40.0f));
        ControlsSlot->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
        ControlsSlot->SetAlignment(FVector2D(0.0f, 1.0f));
        ControlsSlot->SetPosition(FVector2D(24.0f, -18.0f));
    }

    PauseOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PauseOverlay"));
    PauseOverlay->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.74f));
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
    PauseMenuPanel->SetBrushColor(FLinearColor(0.09f, 0.09f, 0.07f, 0.96f));
    UCanvasPanelSlot* PauseMenuSlot = PauseCanvas->AddChildToCanvas(PauseMenuPanel);
    if (PauseMenuSlot != nullptr)
    {
        PauseMenuSlot->SetSize(FVector2D(560.0f, 640.0f));
        PauseMenuSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        PauseMenuSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        PauseMenuSlot->SetPosition(FVector2D::ZeroVector);
    }

    UVerticalBox* PauseMenuContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PauseMenuContent"));
    PauseMenuPanel->SetContent(PauseMenuContent);

    UTextBlock* PauseTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PauseTitle"));
    ConfigureText(PauseTitle, TEXT("Pilgrimage Menu"), FLinearColor(0.95f, 0.90f, 0.78f), 22);
    PauseTitle->SetJustification(ETextJustify::Center);
    AddVerticalChild(PauseMenuContent, PauseTitle, FMargin(12.0f, 16.0f, 12.0f, 8.0f));

    UTextBlock* ButtonLabel = nullptr;
    PauseResumeButton = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("ResumeButton"), TEXT("Resume"), ButtonLabel);
    if (PauseResumeButton != nullptr)
    {
        PauseResumeButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleResumePressed);
    }

    UTextBlock* SaveHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SaveHeader"));
    ConfigureText(SaveHeader, TEXT("Save Slots"), FLinearColor(0.94f, 0.88f, 0.74f), 18);
    AddVerticalChild(PauseMenuContent, SaveHeader, FMargin(12.0f, 16.0f, 12.0f, 6.0f));

    PauseSaveSlot1Button = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("SaveSlot1Button"), TEXT("Save Slot 1"), ButtonLabel);
    if (PauseSaveSlot1Button != nullptr)
    {
        PauseSaveSlot1Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSaveSlot1Pressed);
    }
    PauseSaveSlot2Button = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("SaveSlot2Button"), TEXT("Save Slot 2"), ButtonLabel);
    if (PauseSaveSlot2Button != nullptr)
    {
        PauseSaveSlot2Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSaveSlot2Pressed);
    }
    PauseSaveSlot3Button = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("SaveSlot3Button"), TEXT("Save Slot 3"), ButtonLabel);
    if (PauseSaveSlot3Button != nullptr)
    {
        PauseSaveSlot3Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSaveSlot3Pressed);
    }

    UTextBlock* LoadHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LoadHeader"));
    ConfigureText(LoadHeader, TEXT("Load Slots"), FLinearColor(0.94f, 0.88f, 0.74f), 18);
    AddVerticalChild(PauseMenuContent, LoadHeader, FMargin(12.0f, 14.0f, 12.0f, 6.0f));

    PauseLoadSlot1Button = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("LoadSlot1Button"), TEXT("Load Slot 1"), ButtonLabel);
    if (PauseLoadSlot1Button != nullptr)
    {
        PauseLoadSlot1Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleLoadSlot1Pressed);
    }
    PauseLoadSlot2Button = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("LoadSlot2Button"), TEXT("Load Slot 2"), ButtonLabel);
    if (PauseLoadSlot2Button != nullptr)
    {
        PauseLoadSlot2Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleLoadSlot2Pressed);
    }
    PauseLoadSlot3Button = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("LoadSlot3Button"), TEXT("Load Slot 3"), ButtonLabel);
    if (PauseLoadSlot3Button != nullptr)
    {
        PauseLoadSlot3Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleLoadSlot3Pressed);
    }

    SlotSummary1Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotSummary1Text"));
    ConfigureText(SlotSummary1Text, TEXT("Slot 1: Empty"), FLinearColor(0.82f, 0.82f, 0.78f), 14);
    AddVerticalChild(PauseMenuContent, SlotSummary1Text, FMargin(14.0f, 8.0f, 12.0f, 0.0f));

    SlotSummary2Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotSummary2Text"));
    ConfigureText(SlotSummary2Text, TEXT("Slot 2: Empty"), FLinearColor(0.82f, 0.82f, 0.78f), 14);
    AddVerticalChild(PauseMenuContent, SlotSummary2Text, FMargin(14.0f, 2.0f, 12.0f, 0.0f));

    SlotSummary3Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotSummary3Text"));
    ConfigureText(SlotSummary3Text, TEXT("Slot 3: Empty"), FLinearColor(0.82f, 0.82f, 0.78f), 14);
    AddVerticalChild(PauseMenuContent, SlotSummary3Text, FMargin(14.0f, 2.0f, 12.0f, 0.0f));

    PauseNewPilgrimageButton = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("NewPilgrimageButton"), TEXT("New Pilgrimage"), ButtonLabel);
    if (PauseNewPilgrimageButton != nullptr)
    {
        PauseNewPilgrimageButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleNewPilgrimagePressed);
    }

    PauseOptionsButton = CreateMenuButton(WidgetTree, PauseMenuContent, TEXT("PauseOptionsButton"), TEXT("Options"), ButtonLabel);
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
    StartMenuPanel->SetBrushColor(FLinearColor(0.08f, 0.07f, 0.05f, 0.90f));
    UCanvasPanelSlot* StartPanelSlot = StartCanvas->AddChildToCanvas(StartMenuPanel);
    if (StartPanelSlot != nullptr)
    {
        StartPanelSlot->SetSize(FVector2D(620.0f, 470.0f));
        StartPanelSlot->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
        StartPanelSlot->SetAlignment(FVector2D(0.0f, 1.0f));
        StartPanelSlot->SetPosition(FVector2D(54.0f, -42.0f));
    }

    UVerticalBox* StartMenuContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StartMenuContent"));
    StartMenuPanel->SetContent(StartMenuContent);

    UTextBlock* StartTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StartTitle"));
    ConfigureText(StartTitle, TEXT("THE NAZARENE"), FLinearColor(0.98f, 0.92f, 0.80f), 30);
    StartTitle->SetJustification(ETextJustify::Left);
    AddVerticalChild(StartMenuContent, StartTitle, FMargin(20.0f, 26.0f, 18.0f, 4.0f));

    UTextBlock* StartSubtitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StartSubtitle"));
    ConfigureText(StartSubtitle, TEXT("The stone is rolled away."), FLinearColor(0.92f, 0.86f, 0.66f), 18);
    StartSubtitle->SetJustification(ETextJustify::Left);
    AddVerticalChild(StartMenuContent, StartSubtitle, FMargin(20.0f, 0.0f, 18.0f, 4.0f));

    UTextBlock* StartLoreLine = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StartLoreLine"));
    ConfigureText(StartLoreLine, TEXT("An empty tomb. Dawn over Jerusalem. Begin the pilgrimage."), FLinearColor(0.86f, 0.82f, 0.74f), 14);
    StartLoreLine->SetAutoWrapText(true);
    StartLoreLine->SetJustification(ETextJustify::Left);
    AddVerticalChild(StartMenuContent, StartLoreLine, FMargin(20.0f, 0.0f, 20.0f, 18.0f));

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
    OptionsPanel->SetBrushColor(FLinearColor(0.10f, 0.09f, 0.07f, 0.98f));
    UCanvasPanelSlot* OptionsPanelSlot = OptionsCanvas->AddChildToCanvas(OptionsPanel);
    if (OptionsPanelSlot != nullptr)
    {
        OptionsPanelSlot->SetSize(FVector2D(720.0f, 960.0f));
        OptionsPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        OptionsPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        OptionsPanelSlot->SetPosition(FVector2D::ZeroVector);
    }

    UVerticalBox* OptionsContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("OptionsContent"));
    OptionsPanel->SetContent(OptionsContent);

    UTextBlock* OptionsTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OptionsTitle"));
    ConfigureText(OptionsTitle, TEXT("Options"), FLinearColor(0.98f, 0.92f, 0.80f), 28);
    OptionsTitle->SetJustification(ETextJustify::Center);
    AddVerticalChild(OptionsContent, OptionsTitle, FMargin(16.0f, 22.0f, 16.0f, 10.0f));

    OptionsSummaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OptionsSummaryText"));
    ConfigureText(OptionsSummaryText, TEXT("Loading settings..."), FLinearColor(0.90f, 0.86f, 0.76f), 15);
    OptionsSummaryText->SetAutoWrapText(true);
    AddVerticalChild(OptionsContent, OptionsSummaryText, FMargin(18.0f, 4.0f, 18.0f, 12.0f));

    UTextBlock* OptionsButtonLabel = nullptr;
    TArray<UButton*> OptionsButtons;

    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("SensitivityDownButton"), TEXT("Sensitivity -"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSensitivityDownPressed);
        FirstOptionsButton = Button;
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("SensitivityUpButton"), TEXT("Sensitivity +"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSensitivityUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("InvertLookYButton"), TEXT("Toggle Invert Look Y"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleInvertLookYPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("FovDownButton"), TEXT("Field Of View -"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFovDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("FovUpButton"), TEXT("Field Of View +"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFovUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("GammaDownButton"), TEXT("Gamma -"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleGammaDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("GammaUpButton"), TEXT("Gamma +"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleGammaUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("VolumeDownButton"), TEXT("Master Volume -"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleVolumeDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("VolumeUpButton"), TEXT("Master Volume +"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleVolumeUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("FrameLimitDownButton"), TEXT("Frame Limit -"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFrameLimitDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("FrameLimitUpButton"), TEXT("Frame Limit +"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleFrameLimitUpPressed);
        OptionsButtons.Add(Button);
    }

    UTextBlock* AccessibilityHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AccessibilityHeader"));
    ConfigureText(AccessibilityHeader, TEXT("-- Accessibility --"), FLinearColor(0.94f, 0.88f, 0.74f), 16);
    AccessibilityHeader->SetJustification(ETextJustify::Center);
    AddVerticalChild(OptionsContent, AccessibilityHeader, FMargin(12.0f, 12.0f, 12.0f, 4.0f));

    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("SubtitleScaleDownButton"), TEXT("Subtitle Size -"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSubtitleScaleDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("SubtitleScaleUpButton"), TEXT("Subtitle Size +"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleSubtitleScaleUpPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("ColorblindToggleButton"), TEXT("Colorblind Mode"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleColorblindTogglePressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("HighContrastToggleButton"), TEXT("High Contrast HUD"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleHighContrastTogglePressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("ScreenShakeToggleButton"), TEXT("Screen Shake Reduction"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleScreenShakeTogglePressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("HUDScaleDownButton"), TEXT("HUD Scale -"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleHUDScaleDownPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("HUDScaleUpButton"), TEXT("HUD Scale +"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleHUDScaleUpPressed);
        OptionsButtons.Add(Button);
    }

    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("OptionsApplyButton"), TEXT("Apply And Save"), OptionsButtonLabel))
    {
        Button->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleOptionsApplyPressed);
        OptionsButtons.Add(Button);
    }
    if (UButton* Button = CreateMenuButton(WidgetTree, OptionsContent, TEXT("OptionsBackButton"), TEXT("Back"), OptionsButtonLabel))
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

    DeathOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DeathOverlay"));
    DeathOverlay->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.86f));
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
    ConfigureText(DeathTitleText, TEXT("You Were Struck Down"), FLinearColor(0.95f, 0.34f, 0.24f), 34);
    DeathTitleText->SetJustification(ETextJustify::Center);
    AddVerticalChild(DeathContent, DeathTitleText, FMargin(16.0f, 220.0f, 16.0f, 12.0f));

    DeathRetryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DeathRetryText"));
    ConfigureText(DeathRetryText, TEXT("Attempt 1 begins."), FLinearColor(0.95f, 0.89f, 0.79f), 18);
    DeathRetryText->SetJustification(ETextJustify::Center);
    AddVerticalChild(DeathContent, DeathRetryText, FMargin(16.0f, 0.0f, 16.0f, 24.0f));

    UTextBlock* RiseButtonLabel = nullptr;
    RiseAgainButton = CreateMenuButton(WidgetTree, DeathContent, TEXT("RiseAgainButton"), TEXT("Rise Again"), RiseButtonLabel);
    if (RiseAgainButton != nullptr)
    {
        RiseAgainButton->OnClicked.AddDynamic(this, &UNazareneHUDWidget::HandleRiseAgainPressed);
    }

    LoadingOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("LoadingOverlay"));
    LoadingOverlay->SetBrushColor(FLinearColor(0.01f, 0.01f, 0.01f, 0.88f));
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
    ConfigureText(LoadingTitle, TEXT("Loading Pilgrimage..."), FLinearColor(0.95f, 0.90f, 0.82f), 30);
    LoadingTitle->SetJustification(ETextJustify::Center);
    AddVerticalChild(LoadingContent, LoadingTitle, FMargin(20.0f, 220.0f, 20.0f, 18.0f));

    LoadingTipText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LoadingTipText"));
    ConfigureText(LoadingTipText, TEXT("Lore Tip"), FLinearColor(0.88f, 0.84f, 0.72f), 18);
    LoadingTipText->SetJustification(ETextJustify::Center);
    LoadingTipText->SetAutoWrapText(true);
    AddVerticalChild(LoadingContent, LoadingTipText, FMargin(120.0f, 0.0f, 120.0f, 12.0f));

    // Skill Tree Widget (separate viewport widget, toggled by T key)
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
            SlotSummary1Text->SetText(FText::FromString(TEXT("Slot 1: Save subsystem unavailable")));
        }
        if (SlotSummary2Text != nullptr)
        {
            SlotSummary2Text->SetText(FText::FromString(TEXT("Slot 2: Save subsystem unavailable")));
        }
        if (SlotSummary3Text != nullptr)
        {
            SlotSummary3Text->SetText(FText::FromString(TEXT("Slot 3: Save subsystem unavailable")));
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
        TEXT("Mouse Sensitivity: %.2f\nInvert Look Y: %s\nField Of View: %.0f\nDisplay Gamma: %.2f\nMaster Volume: %.0f%%\nFrame Rate Limit: %.0f\nSubtitle Scale: %.2f\nColorblind Mode: %s\nHigh Contrast HUD: %s\nScreen Shake Reduction: %s\nHUD Scale: %.2f"),
        Settings.MouseSensitivity,
        Settings.bInvertLookY ? TEXT("Enabled") : TEXT("Disabled"),
        Settings.FieldOfView,
        Settings.DisplayGamma,
        Settings.MasterVolume * 100.0f,
        Settings.FrameRateLimit,
        Settings.SubtitleTextScale,
        ColorblindModeNames[ColorblindIndex],
        Settings.bHighContrastHUD ? TEXT("Enabled") : TEXT("Disabled"),
        Settings.bScreenShakeReduction ? TEXT("Enabled") : TEXT("Disabled"),
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

    if (HealthText != nullptr)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("Health %.0f / %.0f"), Health, MaxHealth)));
    }
    if (StaminaText != nullptr)
    {
        StaminaText->SetText(FText::FromString(FString::Printf(TEXT("Stamina %.0f / %.0f"), Stamina, MaxStamina)));
    }
    if (FaithText != nullptr)
    {
        FaithText->SetText(FText::FromString(FString::Printf(TEXT("Faith %.0f"), Player->GetFaith())));
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
            HealthBar->SetFillColorAndOpacity(FLinearColor(0.98f, 0.24f * PulseAlpha, 0.20f * PulseAlpha, 1.0f));
        }
    }
    else
    {
        HealthPulseTimer = 0.0f;
        if (HealthBar)
        {
            HealthBar->SetFillColorAndOpacity(FLinearColor(0.83f, 0.24f, 0.20f, 1.0f));
        }
    }

    bStaminaCritical = TargetStaminaPercent <= 0.15f;
    if (bStaminaCritical)
    {
        StaminaPulseTimer += CachedDeltaTime;
        const float StaminaPulse = 0.5f + 0.5f * FMath::Sin(StaminaPulseTimer * 6.0f);
        if (StaminaBar)
        {
            StaminaBar->SetFillColorAndOpacity(FLinearColor(0.22f, 0.68f * StaminaPulse, 0.24f * StaminaPulse, 1.0f));
        }
    }
    else
    {
        StaminaPulseTimer = 0.0f;
        if (StaminaBar)
        {
            StaminaBar->SetFillColorAndOpacity(FLinearColor(0.22f, 0.68f, 0.24f, 1.0f));
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

    if (LockTargetText != nullptr)
    {
        const FString TargetName = Player->GetLockTargetName();
        LockTargetText->SetText(FText::FromString(FString::Printf(TEXT("Lock-On %s"), TargetName.IsEmpty() ? TEXT("None") : *TargetName)));
    }
    if (ContextHintText != nullptr)
    {
        ContextHintText->SetText(FText::FromString(Player->GetContextHint()));
    }

    if (CriticalStateText != nullptr)
    {
        FString CriticalState;
        FLinearColor CriticalColor(0.95f, 0.40f, 0.28f, 1.0f);

        if (HealthRatio <= 0.25f)
        {
            CriticalState = TEXT("Critical Health - Retreat or heal now");
            CriticalColor = FLinearColor(0.98f, 0.25f, 0.20f, 1.0f);
        }
        else if (StaminaRatio <= 0.15f)
        {
            CriticalState = TEXT("Exhausted - stamina nearly depleted");
            CriticalColor = FLinearColor(0.96f, 0.72f, 0.24f, 1.0f);
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

    if (CombatStateText != nullptr)
    {
        const FString BlessingState = Player->IsMiracleUnlocked(FName(TEXT("blessing")))
            ? FString::Printf(TEXT("Blessing %.1fs"), Player->GetBlessingCooldownRemaining())
            : TEXT("Blessing Locked");
        const FString RadianceState = Player->IsMiracleUnlocked(FName(TEXT("radiance")))
            ? FString::Printf(TEXT("Radiance %.1fs"), Player->GetRadianceCooldownRemaining())
            : TEXT("Radiance Locked");

        CombatStateText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Lvl %d | XP %d (Next %d) | Skill Pts %d\nHeal %.1fs | %s | %s"),
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

    // Task 7: Dismiss menu camera when starting a new game
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

    // Task 7: Dismiss menu camera when continuing
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

