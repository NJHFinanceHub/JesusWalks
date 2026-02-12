#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NazareneSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FNazareneRegionState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ClearedEncounters = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bBossDefeated = false;
};

USTRUCT(BlueprintType)
struct FNazareneCampaignState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ChapterIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PrayerCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TotalSoulCurrency = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FNazareneRegionState> Regions;
};

UCLASS()
class THENAZARENEAAA_API UNazareneSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNazareneCampaignState CampaignState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PlayerHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PlayerFaith = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector PlayerWorldLocation = FVector::ZeroVector;
};
