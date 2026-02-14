#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NazareneTypes.h"
#include "NazareneCampaignGameMode.generated.h"

class ANazareneEnemyCharacter;
class ANazareneHUD;
class ANazareneMenuCameraActor;
class ANazareneNPC;
class ANazarenePlayerCharacter;
class ANazareneTravelGate;
class UAudioComponent;
class UBehaviorTree;
class UNazareneGameInstance;
class UNazareneRegionDataAsset;
class UNazareneSaveSubsystem;
class USoundBase;

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
    virtual void Tick(float DeltaSeconds) override;

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

    UPROPERTY(EditDefaultsOnly, Category = "Campaign")
    TObjectPtr<UNazareneRegionDataAsset> RegionDataAsset;

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    void OnMenuDismissed();

private:
    void SpawnMenuCamera();
    void DestroyMenuCamera();
    void SpawnMenuSetpiece(const FVector& CameraCenter);
    void DestroyMenuSetpiece();
    void BuildDefaultRegions();
    void LoadRegion(int32 TargetRegionIndex);
    void ClearRegionActors();
    void SpawnRegionEnvironment(const FNazareneRegionDefinition& Region);
    bool TryLoadRegionSublevel(const FNazareneRegionDefinition& Region);
    void UnloadRegionSublevel();
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
    USoundBase* ResolveRegionMusic(const FNazareneRegionDefinition& Region) const;
    FString GetRandomLoreTip() const;
    void ConfigureEnemyBehaviorTree(ANazareneEnemyCharacter* Enemy) const;
    void ApplyRegionalEnemyTuning(ANazareneEnemyCharacter* Enemy, const FNazareneRegionDefinition& Region, bool bIsWaveEnemy) const;
    int32 XPForLevel(int32 LevelValue) const;

    UFUNCTION()
    void HandleEnemyRedeemed(ANazareneEnemyCharacter* Enemy, float FaithReward);

    void SpawnWaveEnemies(const FNazareneEncounterWave& Wave);
    void CheckDeferredWaves(ENazareneSpawnTrigger Trigger, int32 Param = 0);

    UFUNCTION()
    void HandleReinforcementWave(ANazareneEnemyCharacter* Boss, int32 WaveIndex);

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
    TObjectPtr<UAudioComponent> RegionMusicComponent;

    UPROPERTY()
    TMap<FName, TObjectPtr<ANazareneEnemyCharacter>> EnemyBySpawnId;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> RegionActors;

    UPROPERTY()
    TArray<FNazareneEncounterWave> DeferredWaves;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSoftObjectPtr<USoundBase> GalileeMusic;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSoftObjectPtr<USoundBase> DecapolisMusic;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSoftObjectPtr<USoundBase> WildernessMusic;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSoftObjectPtr<USoundBase> JerusalemMusic;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSoftObjectPtr<USoundBase> GethsemaneMusic;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSoftObjectPtr<USoundBase> ViaDolorosaMusic;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSoftObjectPtr<USoundBase> EmptyTombMusic;

    UPROPERTY(EditAnywhere, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BTMeleeShieldAsset;

    UPROPERTY(EditAnywhere, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BTSpearAsset;

    UPROPERTY(EditAnywhere, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BTRangedAsset;

    UPROPERTY(EditAnywhere, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BTDemonAsset;

    UPROPERTY(EditAnywhere, Category = "AI")
    TSoftObjectPtr<UBehaviorTree> BTBossAsset;

    UPROPERTY(EditAnywhere, Category = "Progression")
    float ChapterHealthScaleStep = 0.08f;

    UPROPERTY(EditAnywhere, Category = "Progression")
    float ChapterDamageScaleStep = 0.06f;

    UPROPERTY(EditAnywhere, Category = "Progression")
    float ChapterFaithRewardScaleStep = 0.05f;

    UPROPERTY(EditAnywhere, Category = "Progression")
    float WaveEnemyBonusScale = 0.07f;

    UPROPERTY(EditAnywhere, Category = "Progression")
    float RetryAssistHealthReductionPerRetry = 0.03f;

    UPROPERTY(EditAnywhere, Category = "Progression")
    float RetryAssistDamageReductionPerRetry = 0.02f;

    FName LoadedRegionLevelPackage = NAME_None;

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

    UPROPERTY()
    TObjectPtr<ANazareneMenuCameraActor> MenuCamera;

    UPROPERTY()
    TObjectPtr<UAudioComponent> CrossfadeAudioComponent;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> MenuSetpieceActors;

    float MusicCrossfadeDuration = 2.0f;
    float CrossfadeElapsed = 0.0f;
    bool bCrossfading = false;
    float CrossfadeStartVolume = 1.0f;

    void CrossfadeToMusic(USoundBase* NewMusic);
    void TickCrossfade(float DeltaSeconds);
};
