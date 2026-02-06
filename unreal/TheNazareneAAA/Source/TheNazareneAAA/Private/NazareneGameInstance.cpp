#include "NazareneGameInstance.h"

void UNazareneGameInstance::Init()
{
    Super::Init();
    StartNewGame();
}

void UNazareneGameInstance::StartNewGame()
{
    CampaignState = FNazareneCampaignState();
    PendingPayload = FNazareneSavePayload();
    bHasPendingPayload = false;
}

const FNazareneCampaignState& UNazareneGameInstance::GetCampaignState() const
{
    return CampaignState;
}

FNazareneCampaignState& UNazareneGameInstance::GetMutableCampaignState()
{
    return CampaignState;
}

bool UNazareneGameInstance::EnsureMiracleUnlocked(FName MiracleId)
{
    if (MiracleId.IsNone())
    {
        return false;
    }

    if (CampaignState.UnlockedMiracles.Contains(MiracleId))
    {
        return false;
    }

    CampaignState.UnlockedMiracles.Add(MiracleId);
    return true;
}

void UNazareneGameInstance::MarkFlag(FName FlagId)
{
    if (FlagId.IsNone())
    {
        return;
    }

    if (!CampaignState.Flags.Contains(FlagId))
    {
        CampaignState.Flags.Add(FlagId);
    }
}

bool UNazareneGameInstance::IsFlagSet(FName FlagId) const
{
    if (FlagId.IsNone())
    {
        return false;
    }
    return CampaignState.Flags.Contains(FlagId);
}

void UNazareneGameInstance::QueuePendingPayload(const FNazareneSavePayload& Payload)
{
    PendingPayload = Payload;
    bHasPendingPayload = true;
}

bool UNazareneGameInstance::ConsumePendingPayload(FNazareneSavePayload& OutPayload)
{
    if (!bHasPendingPayload)
    {
        return false;
    }

    OutPayload = PendingPayload;
    PendingPayload = FNazareneSavePayload();
    bHasPendingPayload = false;
    return true;
}

