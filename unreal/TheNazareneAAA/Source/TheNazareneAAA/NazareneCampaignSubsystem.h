#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NazareneSaveGame.h"
#include "NazareneCampaignSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FNazareneRegionDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EncountersToClear = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SoulRewardOnClear = 100;
};

UCLASS()
class THENAZARENEAAA_API UNazareneCampaignSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void StartNewCampaign();

    UFUNCTION(BlueprintCallable)
    void RecordPrayer();

    UFUNCTION(BlueprintCallable)
    bool RecordEncounterWin(FName RegionId, bool bBossEncounter);

    UFUNCTION(BlueprintCallable)
    void RegisterRegionDefinition(FNazareneRegionDefinition Definition);

    UFUNCTION(BlueprintCallable)
    const FNazareneCampaignState& GetCampaignState() const;

    UFUNCTION(BlueprintCallable)
    void ApplyLoadedCampaignState(const FNazareneCampaignState& LoadedState);

private:
    UPROPERTY()
    FNazareneCampaignState CampaignState;

    UPROPERTY()
    TMap<FName, FNazareneRegionDefinition> RegionDefinitions;

    FNazareneRegionState* FindRegionStateMutable(FName RegionId);
};
