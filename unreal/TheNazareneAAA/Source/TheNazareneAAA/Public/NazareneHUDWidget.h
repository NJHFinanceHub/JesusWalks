#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NazareneHUDWidget.generated.h"

class ANazarenePlayerCharacter;
class UBorder;
class UProgressBar;
class UTextBlock;

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

private:
    void RefreshVitals(const ANazarenePlayerCharacter* Player);

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

    FString CachedRegionName = TEXT("Chapter 1: Galilee Shores");
    FString CachedObjective = TEXT("Redeem the guardian.");
    float MessageTimer = 0.0f;
    bool bOptionsOpenedFromStartMenu = true;
};
