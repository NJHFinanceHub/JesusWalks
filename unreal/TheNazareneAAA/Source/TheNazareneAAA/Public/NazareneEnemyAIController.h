#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NazareneEnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;
class UBehaviorTreeComponent;
class UBlackboardComponent;
struct FAIStimulus;

UCLASS()
class THENAZARENEAAA_API ANazareneEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    ANazareneEnemyAIController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetBehaviorTreeAsset(UBehaviorTree* InBehaviorTree);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetFallbackTarget(AActor* TargetActor);

    UFUNCTION(BlueprintCallable, Category = "AI")
    AActor* GetCurrentTargetActor() const;

private:
    void PushTargetToBlackboard(AActor* TargetActor);
    void PushDistanceToBlackboard() const;

    UFUNCTION()
    void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
    UPROPERTY(VisibleAnywhere, Category = "AI")
    TObjectPtr<UAIPerceptionComponent> RuntimePerception;

    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

    UPROPERTY(VisibleAnywhere, Category = "AI")
    TObjectPtr<UBehaviorTreeComponent> RuntimeBehaviorTree;

    UPROPERTY(VisibleAnywhere, Category = "AI")
    TObjectPtr<UBlackboardComponent> RuntimeBlackboard;

    UPROPERTY(EditAnywhere, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FName TargetActorKey = FName(TEXT("TargetActor"));

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FName TargetDistanceKey = FName(TEXT("TargetDistance"));

    UPROPERTY()
    TObjectPtr<AActor> CurrentTargetActor;
};

