#include "NazareneGameInstance.h"

#include "NazareneCampaignSubsystem.h"
#include "TheNazareneAAA.h"

void UNazareneGameInstance::Init()
{
    Super::Init();

    if (UNazareneCampaignSubsystem* Campaign = GetSubsystem<UNazareneCampaignSubsystem>())
    {
        Campaign->RegisterRegionDefinition({TEXT("Galilee"), 3, 120});
        Campaign->RegisterRegionDefinition({TEXT("Judea"), 4, 180});
        Campaign->RegisterRegionDefinition({TEXT("Jerusalem"), 5, 300});
    }

    UE_LOG(LogNazarene, Log, TEXT("Nazarene GameInstance initialized."));
}
