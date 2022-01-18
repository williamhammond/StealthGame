#include "FPSAIGuard.h"

#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Kismet/GameplayStatics.h"


AFPSAIGuard::AFPSAIGuard()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;

	PatrolIndex = 0;
}

void AFPSAIGuard::BeginPlay()
{
	OriginalRotation = GetActorRotation();
	if (bIsPatrol && PatrolPoints.Num() > 0)
	{
		CurrentTarget = PatrolPoints[PatrolIndex];
		UE_LOG(LogTemp, Warning, TEXT("Initial patrol target: %s"), *GetNameSafe(CurrentTarget));
		UNavigationSystem::SimpleMoveToActor(GetController(), CurrentTarget);
	}
	Super::BeginPlay();
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn)
	{
		return;
	}
	AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->CompleteMission(SeenPawn, false);
	}
	SetGuardState(EAIState::Alerted);

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Yellow, false, 10.0f);
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (!NoiseInstigator || GuardState == EAIState::Alerted)
	{
		return;
	}

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f, false);
	SetGuardState(EAIState::Suspicious);
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;
	OnRep_GuardState();
}

void AFPSAIGuard::Patrol()
{
	if (GuardState != EAIState::Idle || !bIsPatrol)
	{
		return;
	}

	const FVector Delta = GetActorLocation() - CurrentTarget->GetActorLocation();
	const float DistanceToTarget = Delta.Size();
	// UE_LOG(LogTemp, Warning, TEXT("The DistanceToTarget is : %f"), DistanceToTarget);
	if (DistanceToTarget < 100)
	{
		PatrolIndex = (PatrolIndex + 1) % (PatrolPoints.Num());
		UE_LOG(LogTemp, Warning, TEXT("PatrolIndex updated to : %d"), PatrolIndex);
		CurrentTarget = PatrolPoints[PatrolIndex];
		UE_LOG(LogTemp, Warning, TEXT("Current patrol target: %s"), *GetNameSafe(CurrentTarget));
		UNavigationSystem::SimpleMoveToActor(GetController(), CurrentTarget);
	}
}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSAIGuard, GuardState);
}

void AFPSAIGuard::Tick(float DeltaTime)
{
	Patrol();
	Super::Tick(DeltaTime);
}
