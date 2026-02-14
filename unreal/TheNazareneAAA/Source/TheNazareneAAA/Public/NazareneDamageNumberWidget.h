#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NazareneTypes.h"
#include "NazareneDamageNumberWidget.generated.h"

class UTextBlock;

UCLASS()
class THENAZARENEAAA_API UNazareneDamageNumberWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void InitializeDamageNumber(APlayerController* InOwningController, const FVector& InWorldLocation, float InAmount, ENazareneDamageNumberType InType);
    bool IsExpired() const { return bExpired; }

private:
    void UpdateVisualState(float Alpha);

private:
    UPROPERTY()
    TObjectPtr<UTextBlock> ValueText;

    UPROPERTY()
    TObjectPtr<APlayerController> OwningController;

    FVector WorldLocation = FVector::ZeroVector;
    float Amount = 0.0f;
    float ElapsedTime = 0.0f;
    float Lifetime = 0.9f;
    float DriftSpeed = 120.0f;
    ENazareneDamageNumberType Type = ENazareneDamageNumberType::Normal;
    bool bExpired = false;
};
