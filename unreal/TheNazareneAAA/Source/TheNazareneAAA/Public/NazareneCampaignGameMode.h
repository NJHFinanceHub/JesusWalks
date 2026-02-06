#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NazareneTypes.h"
#include "NazareneCampaignGameMode.generated.h"

class ANazareneEnemyCharacter;
class ANazarenePlayerCharacter;
class ANazarenePrayerSite;
class ANazareneTravelGate;
class UNazareneGameInstance;

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
    int32 GetCurrentRegionIndex() const;

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    FNazareneSavePayload BuildSavePayload() const;

    UFUNCTION(BlueprintCallable, Category = "Campaign")
    bool ApplyLoadedPayload(const FNazareneSavePayload& Payload);

private:
    void InitializeDefaultRegions();
    void LoadRegion(int32 RegionIndex);
    void ClearRegionActors();
    void SpawnRegionActors(const FNazareneRegionDefinition& Region);
    void SaveCheckpoint();
    void ApplyRegionReward(const FNazareneRegionDefinition& Region);
    void UpdatePlayerProgressionStats();

    UFUNCTION()
    void HandleEnemyRedeemed(ANazareneEnemyCharacter* Enemy, float FaithReward);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<ANazarenePlayerCharacter> PlayerCharacterClass;

    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<ANazareneEnemyCharacter> EnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<ANazarenePrayerSite> PrayerSiteClass;

    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<ANazareneTravelGate> TravelGateClass;

    UPROPERTY(EditDefaultsOnly, Category = "Campaign")
    TArray<FNazareneRegionDefinition> Regions;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> SpawnedRegionActors;

    UPROPERTY()
    TObjectPtr<ANazarenePlayerCharacter> PlayerCharacter = nullptr;

    UPROPERTY()
    TObjectPtr<ANazareneEnemyCharacter> BossEnemy = nullptr;

    UPROPERTY()
    TObjectPtr<ANazareneTravelGate> TravelGate = nullptr;

    UPROPERTY()
    TObjectPtr<UNazareneGameInstance> NazareneGameInstance = nullptr;

    int32 CurrentRegionIndex = 0;
    bool bRegionCompleted = false;
    bool bSuppressRedeemEvents = false;
};

