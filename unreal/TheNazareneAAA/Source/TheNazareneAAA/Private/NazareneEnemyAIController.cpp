#include "NazareneEnemyAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"

ANazareneEnemyAIController::ANazareneEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    RuntimePerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("RuntimePerception"));
    SetPerceptionComponent(*RuntimePerception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2200.0f;
    SightConfig->LoseSightRadius = 2700.0f;
    SightConfig->PeripheralVisionAngleDegrees = 72.0f;
    SightConfig->SetMaxAge(4.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    RuntimePerception->ConfigureSense(*SightConfig);
    RuntimePerception->SetDominantSense(SightConfig->GetSenseImplementation());

    RuntimeBehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("RuntimeBehaviorTree"));
    RuntimeBlackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("RuntimeBlackboard"));
}

void ANazareneEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (RuntimePerception != nullptr)
    {
        RuntimePerception->OnTargetPerceptionUpdated.RemoveDynamic(this, &ANazareneEnemyAIController::HandleTargetPerceptionUpdated);
        RuntimePerception->OnTargetPerceptionUpdated.AddDynamic(this, &ANazareneEnemyAIController::HandleTargetPerceptionUpdated);
    }

    SetBehaviorTreeAsset(BehaviorTreeAsset);
    PushTargetToBlackboard(CurrentTargetActor.Get());
}

void ANazareneEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    PushDistanceToBlackboard();
}

void ANazareneEnemyAIController::SetBehaviorTreeAsset(UBehaviorTree* InBehaviorTree)
{
    BehaviorTreeAsset = InBehaviorTree;
    if (BehaviorTreeAsset == nullptr || GetPawn() == nullptr)
    {
        return;
    }

    if (BehaviorTreeAsset->BlackboardAsset != nullptr)
    {
        UBlackboardComponent* BlackboardComponent = RuntimeBlackboard.Get();
        UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComponent);
        RuntimeBlackboard = BlackboardComponent;
    }

    RunBehaviorTree(BehaviorTreeAsset);
}

void ANazareneEnemyAIController::SetFallbackTarget(AActor* TargetActor)
{
    if (CurrentTargetActor != nullptr && IsValid(CurrentTargetActor))
    {
        return;
    }

    PushTargetToBlackboard(TargetActor);
}

AActor* ANazareneEnemyAIController::GetCurrentTargetActor() const
{
    return CurrentTargetActor.Get();
}

void ANazareneEnemyAIController::PushTargetToBlackboard(AActor* TargetActor)
{
    CurrentTargetActor = TargetActor;
    if (RuntimeBlackboard == nullptr)
    {
        return;
    }

    RuntimeBlackboard->SetValueAsObject(TargetActorKey, TargetActor);
}

void ANazareneEnemyAIController::PushDistanceToBlackboard() const
{
    if (RuntimeBlackboard == nullptr)
    {
        return;
    }

    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn == nullptr || !IsValid(CurrentTargetActor))
    {
        RuntimeBlackboard->SetValueAsFloat(TargetDistanceKey, -1.0f);
        return;
    }

    const float Distance = FVector::Dist2D(ControlledPawn->GetActorLocation(), CurrentTargetActor->GetActorLocation());
    RuntimeBlackboard->SetValueAsFloat(TargetDistanceKey, Distance);
}

void ANazareneEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Actor == nullptr)
    {
        return;
    }

    if (Stimulus.WasSuccessfullySensed())
    {
        PushTargetToBlackboard(Actor);
    }
    else if (CurrentTargetActor == Actor)
    {
        PushTargetToBlackboard(nullptr);
    }
}
