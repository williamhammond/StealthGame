#include "FPSBlackHole.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AFPSBlackHole::AFPSBlackHole()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	AttractorSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttractorSphere"));
	AttractorSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttractorSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttractorSphere->SetupAttachment(MeshComp);
	AttractorSphere->SetSphereRadius(50000.0);

	DestroyerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DestroyerSphere"));
	DestroyerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DestroyerSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	DestroyerSphere->SetupAttachment(MeshComp);
}

void AFPSBlackHole::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<UPrimitiveComponent*> Components;
	AttractorSphere->GetOverlappingComponents(Components);
	for (UPrimitiveComponent* Component : Components)
	{
		if (Component && Component->IsSimulatingPhysics())
		{
			FVector DirectionToAttract = GetActorLocation() - Component->GetComponentLocation();
			FVector Force = DirectionToAttract * ForceStrength;
			Component->AddForce(Force);
		}
	}

	DestroyerSphere->GetOverlappingComponents(Components);
	for (UPrimitiveComponent* Component : Components)
	{
		if (Component && Component->IsSimulatingPhysics())
		{
			Component->DestroyComponent();
		}
	}
}
