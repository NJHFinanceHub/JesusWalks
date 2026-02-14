#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NazareneTypes.h"
#include "NazareneHUDWidget.generated.h"

class ANazareneEnemyCharacter;
class ANazarenePlayerCharacter;
class UButton;
class UBorder;
class UProgressBar;
class UTextBlock;
class UNazareneDamageNumberWidget;
class UNazareneEnemyHealthBarWidget;
class UNazareneSkillTreeWidget;

UCLASS()
class THENAZARENEAAA_API UNazareneHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void SetRegionName(const FString& InRegionName);
    void SetObjective(const FString& InObjective);
    void ShowMessage(const FString& InMessage, float Duration = 3.5f);
    void SetStartMenuVisible(bool bVisible);
    bool IsStartMenuVisible() const;
    void SetPauseMenuVisible(bool bVisible);
    bool IsPauseMenuVisible() const;
    void RefreshSlotSummaries();
    void ShowDamageNumber(const FVector& WorldLocation, float Amount, ENazareneDamageNumberType Type);
    void ShowDeathOverlay(int32 RetryCount);
    void SetLoadingOverlayVisible(bool bVisible, const FString& LoreTip);

    void SetSkillTreeVisible(bool bVisible);
    bool IsSkillTreeVisible() const;

private:
    void RefreshVitals(const ANazarenePlayerCharacter* Player);
    void SyncEnemyHealthBars(ANazarenePlayerCharacter* Player);

    UFUNCTION()
    void HandleResumePressed();

    UFUNCTION()
    void HandleStartPilgrimagePressed();

    UFUNCTION()
    void HandleContinuePilgrimagePressed();

    UFUNCTION()
    void HandleOptionsPressed();

    UFUNCTION()
    void HandleOptionsBackPressed();

    UFUNCTION()
    void HandleOptionsApplyPressed();

    UFUNCTION()
    void HandleSensitivityDownPressed();

    UFUNCTION()
    void HandleSensitivityUpPressed();

    UFUNCTION()
    void HandleInvertLookYPressed();

    UFUNCTION()
    void HandleFovDownPressed();

    UFUNCTION()
    void HandleFovUpPressed();

    UFUNCTION()
    void HandleGammaDownPressed();

    UFUNCTION()
    void HandleGammaUpPressed();

    UFUNCTION()
    void HandleVolumeDownPressed();

    UFUNCTION()
    void HandleVolumeUpPressed();

    UFUNCTION()
    void HandleFrameLimitDownPressed();

    UFUNCTION()
    void HandleFrameLimitUpPressed();

    UFUNCTION()
    void HandleSubtitleScaleDownPressed();

    UFUNCTION()
    void HandleSubtitleScaleUpPressed();

    UFUNCTION()
    void HandleColorblindTogglePressed();

    UFUNCTION()
    void HandleHighContrastTogglePressed();

    UFUNCTION()
    void HandleScreenShakeTogglePressed();

    UFUNCTION()
    void HandleHUDScaleDownPressed();

    UFUNCTION()
    void HandleHUDScaleUpPressed();

    UFUNCTION()
    void HandleQuitPressed();

    UFUNCTION()
    void HandleSaveSlot1Pressed();

    UFUNCTION()
    void HandleSaveSlot2Pressed();

    UFUNCTION()
    void HandleSaveSlot3Pressed();

    UFUNCTION()
    void HandleLoadSlot1Pressed();

    UFUNCTION()
    void HandleLoadSlot2Pressed();

    UFUNCTION()
    void HandleLoadSlot3Pressed();

    UFUNCTION()
    void HandleNewPilgrimagePressed();

    UFUNCTION()
    void HandleRiseAgainPressed();

    UFUNCTION()
    void HandleSkillTreeClosePressed();

private:
    void RefreshOptionsSummary();

    UPROPERTY()
    TObjectPtr<UTextBlock> HealthText;

    UPROPERTY()
    TObjectPtr<UTextBlock> StaminaText;

    UPROPERTY()
    TObjectPtr<UTextBlock> FaithText;

    UPROPERTY()
    TObjectPtr<UTextBlock> LockTargetText;

    UPROPERTY()
    TObjectPtr<UTextBlock> ContextHintText;

    UPROPERTY()
    TObjectPtr<UTextBlock> CriticalStateText;

    UPROPERTY()
    TObjectPtr<UTextBlock> CombatStateText;

    UPROPERTY()
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY()
    TObjectPtr<UProgressBar> StaminaBar;

    UPROPERTY()
    TObjectPtr<UTextBlock> RegionNameText;

    UPROPERTY()
    TObjectPtr<UTextBlock> ObjectiveText;

    UPROPERTY()
    TObjectPtr<UTextBlock> MessageText;

    UPROPERTY()
    TObjectPtr<UBorder> StartMenuOverlay;

    UPROPERTY()
    TObjectPtr<UBorder> OptionsOverlay;

    UPROPERTY()
    TObjectPtr<UBorder> PauseOverlay;

    UPROPERTY()
    TObjectPtr<UTextBlock> SlotSummary1Text;

    UPROPERTY()
    TObjectPtr<UTextBlock> SlotSummary2Text;

    UPROPERTY()
    TObjectPtr<UTextBlock> SlotSummary3Text;

    UPROPERTY()
    TObjectPtr<UTextBlock> OptionsSummaryText;

    UPROPERTY()
    TObjectPtr<UBorder> DeathOverlay;

    UPROPERTY()
    TObjectPtr<UTextBlock> DeathRetryText;

    UPROPERTY()
    TObjectPtr<UBorder> LoadingOverlay;

    UPROPERTY()
    TObjectPtr<UTextBlock> LoadingTipText;

    UPROPERTY()
    TObjectPtr<UBorder> SkillTreeOverlay;

    UPROPERTY()
    TObjectPtr<UNazareneSkillTreeWidget> SkillTreeWidget;

    UPROPERTY()
    TObjectPtr<ANazarenePlayerCharacter> CachedPlayerForWidgets;

    UPROPERTY()
    TArray<TObjectPtr<UNazareneDamageNumberWidget>> DamageNumberWidgets;

    UPROPERTY()
    TArray<TObjectPtr<UNazareneEnemyHealthBarWidget>> EnemyHealthBarWidgets;

    // Navigation focus targets
    UPROPERTY()
    TObjectPtr<UButton> StartNewGameButton;

    UPROPERTY()
    TObjectPtr<UButton> StartContinueButton;

    UPROPERTY()
    TObjectPtr<UButton> StartOptionsButton;

    UPROPERTY()
    TObjectPtr<UButton> StartQuitButton;

    UPROPERTY()
    TObjectPtr<UButton> PauseResumeButton;

    UPROPERTY()
    TObjectPtr<UButton> PauseSaveSlot1Button;

    UPROPERTY()
    TObjectPtr<UButton> PauseSaveSlot2Button;

    UPROPERTY()
    TObjectPtr<UButton> PauseSaveSlot3Button;

    UPROPERTY()
    TObjectPtr<UButton> PauseLoadSlot1Button;

    UPROPERTY()
    TObjectPtr<UButton> PauseLoadSlot2Button;

    UPROPERTY()
    TObjectPtr<UButton> PauseLoadSlot3Button;

    UPROPERTY()
    TObjectPtr<UButton> PauseNewPilgrimageButton;

    UPROPERTY()
    TObjectPtr<UButton> PauseOptionsButton;

    UPROPERTY()
    TObjectPtr<UButton> RiseAgainButton;

    UPROPERTY()
    TObjectPtr<UButton> FirstOptionsButton;

    FString CachedRegionName = TEXT("Chapter 1: Galilee Shores");
    FString CachedObjective = TEXT("Redeem the guardian.");
    float MessageTimer = 0.0f;
    bool bOptionsOpenedFromStartMenu = true;

    float DisplayedHealthPercent = 1.0f;
    float DisplayedStaminaPercent = 1.0f;
    float BarLerpSpeed = 4.5f;
    float CachedDeltaTime = 0.0f;
    float HealthPulseTimer = 0.0f;
    float StaminaPulseTimer = 0.0f;
    bool bHealthCritical = false;
    bool bStaminaCritical = false;

    UPROPERTY()
    TObjectPtr<UProgressBar> FaithBar;

    UPROPERTY()
    TObjectPtr<UProgressBar> HealCooldownBar;

    UPROPERTY()
    TObjectPtr<UProgressBar> BlessingCooldownBar;

    UPROPERTY()
    TObjectPtr<UProgressBar> RadianceCooldownBar;

    UPROPERTY()
    TObjectPtr<UTextBlock> HealCooldownLabel;

    UPROPERTY()
    TObjectPtr<UTextBlock> BlessingCooldownLabel;

    UPROPERTY()
    TObjectPtr<UTextBlock> RadianceCooldownLabel;
};
