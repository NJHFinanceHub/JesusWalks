#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NazareneTypes.h"
#include "NazareneRegionDataAsset.generated.h"

UCLASS(BlueprintType)
class THENAZARENEAAA_API UNazareneRegionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regions")
	TArray<FNazareneRegionDefinition> Regions;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("RegionData", GetFName());
	}
};
