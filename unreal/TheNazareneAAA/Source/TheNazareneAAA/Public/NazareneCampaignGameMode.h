#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NazareneTypes.h"
#include "NazareneCampaignGameMode.generated.h"

class ANazareneEnemyCharacter;
class ANazareneHUD;
class ANazarenePlayerCharacter;
class ANazareneTravelGate;
class UNazareneGameInstance;
class UNazareneSaveSubsystem;

UENUM()
enum class ENazareneChapterStage : uint8
{
    ConsecratePrayerSite = 0,
    DefeatGuardian = 1,
    ReachTravelGate = 2,
    Completed = 3
};


UENUM(BlueprintType)
enum class ENazareneMusicState : uint8
{
    Peace = 0,
    Tension = 1,
    Combat = 2,
    Boss = 3,
    Victory = 4
};

UCLASS()
class THENAZARENEAAA_API ANazareneCampaignGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ANazareneCampaignGameMode();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    void RequestTravel(int32 TargetRegionIndex);

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    bool SaveToSlot(int32 SlotId);

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    bool LoadFromSlot(int32 SlotId);

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    bool SaveCheckpoint();

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    bool LoadCheckpoint();

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    void NotifyPrayerSiteRest(FName SiteId);

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    void NotifyPlayerDefeated();

    UFUNCTION(BlueprintPure, Category = "Audio")
    ENazareneMusicState GetMusicState() const { return MusicState; }

private:
    void BuildDefaultRegions();
    void LoadRegion(int32 TargetRegionIndex);
    void ClearRegionActors();
    void SpawnRegionEnvironment(const FNazareneRegionDefinition& Region);
    void SpawnRegionActors(const FNazareneRegionDefinition& Region);
    void ApplySavePayload(const FNazareneSavePayload& Payload);
    FNazareneSavePayload BuildSavePayload() const;
    void SyncCompletionState();
    void OnBossRedeemed();
    bool ApplyRegionReward(const FNazareneRegionDefinition& Region);
    void QueueIntroStoryIfNeeded();
    void AdvanceStoryLine();
    void UpdateChapterStageFromState();
    void EnsureRetryCounterForCurrentRegion();
    int32 GetCurrentRegionRetryCount() const;
    void SetCurrentRegionRetryCount(int32 RetryCount);
    FString BuildObjectiveText(const FNazareneRegionDefinition& Region, bool bCompleted) const;
    void EnableTravelGate(bool bEnabled);
    void UpdateHUDForRegion(const FNazareneRegionDefinition& Region, bool bCompleted) const;
    void SetMusicState(ENazareneMusicState NewState, bool bAnnounceOnHUD = false);

    UFUNCTION()
    void HandleEnemyRedeemed(ANazareneEnemyCharacter* Enemy, float FaithReward);

private:
    UPROPERTY()
    TArray<FNazareneRegionDefinition> Regions;

    UPROPERTY()
    TObjectPtr<UNazareneGameInstance> Session;

    UPROPERTY()
    TObjectPtr<UNazareneSaveSubsystem> SaveSubsystem;

    UPROPERTY()
    TObjectPtr<ANazarenePlayerCharacter> PlayerCharacter;

    UPROPERTY()
    TObjectPtr<ANazareneTravelGate> TravelGate;

    UPROPERTY()
    TObjectPtr<ANazareneEnemyCharacter> BossEnemy;

    UPROPERTY()
    TMap<FName, TObjectPtr<ANazareneEnemyCharacter>> EnemyBySpawnId;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> RegionActors;

    int32 RegionIndex = 0;
    bool bRegionCompleted = false;
    bool bPrayerSiteConsecrated = false;
    bool bSuppressRedeemedCallbacks = false;
    ENazareneChapterStage ChapterStage = ENazareneChapterStage::ConsecratePrayerSite;
    int32 StoryLineIndex = 0;
    ENazareneMusicState MusicState = ENazareneMusicState::Peace;

    UPROPERTY()
    TArray<FString> ActiveStoryLines;

    FTimerHandle StoryLineTimerHandle;
};
