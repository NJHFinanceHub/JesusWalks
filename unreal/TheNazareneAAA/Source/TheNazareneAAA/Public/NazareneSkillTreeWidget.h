#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NazareneSkillTree.h"
#include "NazareneSkillTreeWidget.generated.h"

class ANazarenePlayerCharacter;
class UBorder;
class UButton;
class UTextBlock;
class UVerticalBox;

USTRUCT()
struct FNazareneSkillNodeWidgets
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UBorder> NodeBorder = nullptr;

    UPROPERTY()
    TObjectPtr<UTextBlock> NameText = nullptr;

    UPROPERTY()
    TObjectPtr<UTextBlock> DescText = nullptr;

    UPROPERTY()
    TObjectPtr<UButton> UnlockButton = nullptr;

    UPROPERTY()
    TObjectPtr<UTextBlock> ButtonLabel = nullptr;

    FName SkillId = NAME_None;
};

UCLASS()
class THENAZARENEAAA_API UNazareneSkillTreeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void SetPlayerCharacter(ANazarenePlayerCharacter* InPlayer);
    void RefreshSkillTree();

private:
    void CreateBranchColumn(UVerticalBox* ParentColumn, ENazareneSkillBranch Branch, const FString& BranchLabel);

    UFUNCTION()
    void HandleSkillButtonClicked();

    UPROPERTY()
    TObjectPtr<ANazarenePlayerCharacter> CachedPlayer;

    UPROPERTY()
    TObjectPtr<UTextBlock> HeaderText;

    UPROPERTY()
    TArray<FNazareneSkillNodeWidgets> SkillNodes;
};
