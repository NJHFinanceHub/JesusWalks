#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NazareneTypes.h"
#include "NazareneGameInstance.generated.h"

UCLASS()
class THENAZARENEAAA_API UNazareneGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintCallable)
    void StartNewGame();

    UFUNCTION(BlueprintCallable)
    const FNazareneCampaignState& GetCampaignState() const;

    UFUNCTION(BlueprintCallable)
    FNazareneCampaignState& GetMutableCampaignState();

    UFUNCTION(BlueprintCallable)
    bool EnsureMiracleUnlocked(FName MiracleId);

    UFUNCTION(BlueprintCallable)
    bool AddInventoryItem(const FNazareneInventoryItem& Item);

    UFUNCTION(BlueprintCallable)
    bool HasInventoryItem(FName ItemId) const;

    UFUNCTION(BlueprintCallable)
    void MarkFlag(FName FlagId);

    UFUNCTION(BlueprintCallable)
    bool IsFlagSet(FName FlagId) const;

    UFUNCTION(BlueprintCallable)
    void QueuePendingPayload(const FNazareneSavePayload& Payload);

    UFUNCTION(BlueprintCallable)
    bool ConsumePendingPayload(FNazareneSavePayload& OutPayload);

private:
    UPROPERTY()
    FNazareneCampaignState CampaignState;

    UPROPERTY()
    FNazareneSavePayload PendingPayload;

    UPROPERTY()
    bool bHasPendingPayload = false;
};

