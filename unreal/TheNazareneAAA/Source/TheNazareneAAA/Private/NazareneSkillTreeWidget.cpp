#include "NazareneSkillTreeWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "NazarenePlayerCharacter.h"
#include "NazareneSkillTree.h"

namespace
{
    static void ConfigureSkillText(UTextBlock* Text, const FString& Value, const FLinearColor& Color, int32 Size)
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

    static void AddToVertical(UVerticalBox* Parent, UWidget* Child, const FMargin& Padding)
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
}

void UNazareneSkillTreeWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (WidgetTree == nullptr)
    {
        return;
    }

    UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SkillTreeRoot"));
    if (RootPanel == nullptr)
    {
        return;
    }
    WidgetTree->RootWidget = RootPanel;

    // Main container border
    UBorder* MainBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SkillTreeMainBorder"));
    MainBorder->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.04f, 0.96f));
    UCanvasPanelSlot* MainSlot = RootPanel->AddChildToCanvas(MainBorder);
    if (MainSlot != nullptr)
    {
        MainSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        MainSlot->SetOffsets(FMargin(40.0f, 30.0f, 40.0f, 30.0f));
    }

    UVerticalBox* MainContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SkillTreeMainContent"));
    MainBorder->SetContent(MainContent);

    // Header
    HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SkillTreeHeader"));
    ConfigureSkillText(HeaderText, TEXT("SKILL TREE | Level 1 | XP: 0 | Points: 0"), FLinearColor(0.95f, 0.90f, 0.78f), 20);
    HeaderText->SetJustification(ETextJustify::Center);
    AddToVertical(MainContent, HeaderText, FMargin(16.0f, 14.0f, 16.0f, 10.0f));

    // Branch columns container
    UHorizontalBox* ColumnsBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("BranchColumns"));
    AddToVertical(MainContent, ColumnsBox, FMargin(8.0f, 4.0f, 8.0f, 8.0f));

    // Create 4 branch columns
    const ENazareneSkillBranch Branches[] = {
        ENazareneSkillBranch::Combat,
        ENazareneSkillBranch::Movement,
        ENazareneSkillBranch::Miracles,
        ENazareneSkillBranch::Defense
    };
    const TCHAR* BranchNames[] = {
        TEXT("COMBAT"),
        TEXT("MOVEMENT"),
        TEXT("MIRACLES"),
        TEXT("DEFENSE")
    };

    for (int32 Index = 0; Index < 4; ++Index)
    {
        UBorder* ColumnBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), *FString::Printf(TEXT("BranchBorder_%d"), Index));
        ColumnBorder->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.06f, 0.88f));
        UHorizontalBoxSlot* ColumnSlot = ColumnsBox->AddChildToHorizontalBox(ColumnBorder);
        if (ColumnSlot != nullptr)
        {
            ColumnSlot->SetPadding(FMargin(6.0f, 0.0f, 6.0f, 0.0f));
            ColumnSlot->SetSize(FSlateChildSize(1.0f));
        }

        UVerticalBox* ColumnContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), *FString::Printf(TEXT("BranchColumn_%d"), Index));
        ColumnBorder->SetContent(ColumnContent);

        CreateBranchColumn(ColumnContent, Branches[Index], BranchNames[Index]);
    }
}

void UNazareneSkillTreeWidget::CreateBranchColumn(UVerticalBox* ParentColumn, ENazareneSkillBranch Branch, const FString& BranchLabel)
{
    if (WidgetTree == nullptr || ParentColumn == nullptr)
    {
        return;
    }

    // Branch header
    const FString BranchWidgetName = FString::Printf(TEXT("BranchHeader_%s"), *BranchLabel);
    UTextBlock* BranchHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *BranchWidgetName);
    ConfigureSkillText(BranchHeader, BranchLabel, FLinearColor(0.93f, 0.86f, 0.54f), 16);
    BranchHeader->SetJustification(ETextJustify::Center);
    AddToVertical(ParentColumn, BranchHeader, FMargin(8.0f, 10.0f, 8.0f, 6.0f));

    TArray<FName> SkillIds = UNazareneSkillTree::GetBranchSkills(Branch);
    for (const FName& SkillId : SkillIds)
    {
        FNazareneSkillDefinition Definition;
        if (!UNazareneSkillTree::GetSkillDefinition(SkillId, Definition))
        {
            continue;
        }

        FNazareneSkillNodeWidgets NodeWidgets;
        NodeWidgets.SkillId = SkillId;

        // Node border
        const FString NodeName = FString::Printf(TEXT("SkillNode_%s"), *SkillId.ToString());
        NodeWidgets.NodeBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), *NodeName);
        NodeWidgets.NodeBorder->SetBrushColor(FLinearColor(0.18f, 0.18f, 0.16f, 0.92f));
        AddToVertical(ParentColumn, NodeWidgets.NodeBorder, FMargin(6.0f, 4.0f, 6.0f, 4.0f));

        UVerticalBox* NodeContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), *FString::Printf(TEXT("SkillNodeContent_%s"), *SkillId.ToString()));
        NodeWidgets.NodeBorder->SetContent(NodeContent);

        // Skill name
        const FString NameWidgetName = FString::Printf(TEXT("SkillName_%s"), *SkillId.ToString());
        NodeWidgets.NameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *NameWidgetName);
        ConfigureSkillText(NodeWidgets.NameText, Definition.Name, FLinearColor(0.95f, 0.92f, 0.82f), 14);
        AddToVertical(NodeContent, NodeWidgets.NameText, FMargin(8.0f, 6.0f, 8.0f, 2.0f));

        // Skill description
        const FString DescWidgetName = FString::Printf(TEXT("SkillDesc_%s"), *SkillId.ToString());
        NodeWidgets.DescText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *DescWidgetName);
        const FString DescStr = FString::Printf(TEXT("%s\nCost: %d | XP Req: %d"), *Definition.Description, Definition.Cost, Definition.XPRequirement);
        ConfigureSkillText(NodeWidgets.DescText, DescStr, FLinearColor(0.78f, 0.76f, 0.68f), 12);
        NodeWidgets.DescText->SetAutoWrapText(true);
        AddToVertical(NodeContent, NodeWidgets.DescText, FMargin(8.0f, 0.0f, 8.0f, 4.0f));

        // Unlock button
        const FString ButtonName = FString::Printf(TEXT("SkillUnlockBtn_%s"), *SkillId.ToString());
        NodeWidgets.UnlockButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *ButtonName);
        AddToVertical(NodeContent, NodeWidgets.UnlockButton, FMargin(8.0f, 2.0f, 8.0f, 6.0f));

        const FString BtnLabelName = FString::Printf(TEXT("SkillUnlockLabel_%s"), *SkillId.ToString());
        NodeWidgets.ButtonLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *BtnLabelName);
        ConfigureSkillText(NodeWidgets.ButtonLabel, TEXT("Locked"), FLinearColor(0.92f, 0.88f, 0.78f), 13);
        NodeWidgets.ButtonLabel->SetJustification(ETextJustify::Center);
        NodeWidgets.UnlockButton->AddChild(NodeWidgets.ButtonLabel);
        NodeWidgets.UnlockButton->SetIsEnabled(false);

        // Bind to the shared click handler - the handler identifies which button was clicked
        // by checking which button in SkillNodes is hovered at the time of the click
        NodeWidgets.UnlockButton->OnClicked.AddDynamic(this, &UNazareneSkillTreeWidget::HandleSkillButtonClicked);

        SkillNodes.Add(NodeWidgets);
    }
}

void UNazareneSkillTreeWidget::HandleSkillButtonClicked()
{
    if (CachedPlayer == nullptr)
    {
        return;
    }

    // Determine which button was clicked by checking which one is hovered
    for (const FNazareneSkillNodeWidgets& Node : SkillNodes)
    {
        if (Node.UnlockButton != nullptr && Node.UnlockButton->IsHovered())
        {
            CachedPlayer->AttemptUnlockSkill(Node.SkillId);
            RefreshSkillTree();
            return;
        }
    }
}

void UNazareneSkillTreeWidget::SetPlayerCharacter(ANazarenePlayerCharacter* InPlayer)
{
    CachedPlayer = InPlayer;
    RefreshSkillTree();
}

void UNazareneSkillTreeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    RefreshSkillTree();
}

void UNazareneSkillTreeWidget::RefreshSkillTree()
{
    if (CachedPlayer == nullptr)
    {
        return;
    }

    // Update header
    if (HeaderText != nullptr)
    {
        const FString HeaderStr = FString::Printf(
            TEXT("SKILL TREE | Level %d | XP: %d (Next: %d) | Points: %d"),
            CachedPlayer->GetPlayerLevel(),
            CachedPlayer->GetTotalXP(),
            CachedPlayer->GetXPToNextLevel(),
            CachedPlayer->GetSkillPoints()
        );
        HeaderText->SetText(FText::FromString(HeaderStr));
    }

    const TArray<FName>& UnlockedSkills = CachedPlayer->GetUnlockedSkills();

    for (FNazareneSkillNodeWidgets& Node : SkillNodes)
    {
        if (Node.NodeBorder == nullptr)
        {
            continue;
        }

        const bool bUnlocked = UnlockedSkills.Contains(Node.SkillId);
        const bool bAvailable = UNazareneSkillTree::CanUnlockSkill(
            Node.SkillId,
            UnlockedSkills,
            CachedPlayer->GetTotalXP(),
            CachedPlayer->GetSkillPoints()
        );

        if (bUnlocked)
        {
            // Green - unlocked
            Node.NodeBorder->SetBrushColor(FLinearColor(0.12f, 0.32f, 0.14f, 0.92f));
            if (Node.ButtonLabel != nullptr)
            {
                Node.ButtonLabel->SetText(FText::FromString(TEXT("Unlocked")));
            }
            if (Node.UnlockButton != nullptr)
            {
                Node.UnlockButton->SetIsEnabled(false);
            }
        }
        else if (bAvailable)
        {
            // Gold border - available
            Node.NodeBorder->SetBrushColor(FLinearColor(0.38f, 0.32f, 0.12f, 0.92f));
            if (Node.ButtonLabel != nullptr)
            {
                Node.ButtonLabel->SetText(FText::FromString(TEXT("Unlock")));
            }
            if (Node.UnlockButton != nullptr)
            {
                Node.UnlockButton->SetIsEnabled(true);
            }
        }
        else
        {
            // Gray - locked
            Node.NodeBorder->SetBrushColor(FLinearColor(0.14f, 0.14f, 0.13f, 0.92f));
            if (Node.ButtonLabel != nullptr)
            {
                Node.ButtonLabel->SetText(FText::FromString(TEXT("Locked")));
            }
            if (Node.UnlockButton != nullptr)
            {
                Node.UnlockButton->SetIsEnabled(false);
            }
        }
    }
}
