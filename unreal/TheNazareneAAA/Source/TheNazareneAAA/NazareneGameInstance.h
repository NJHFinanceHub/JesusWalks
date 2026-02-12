#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NazareneGameInstance.generated.h"

UCLASS()
class THENAZARENEAAA_API UNazareneGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
};
