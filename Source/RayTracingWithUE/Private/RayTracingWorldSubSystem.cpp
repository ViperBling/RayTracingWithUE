#include "RayTracingWorldSubSystem.h"
#include "EngineUtils.h"
#include "SceneViewExtension.h"

void URayTracingWorldSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RayTracingViewExtension = FSceneViewExtensions::NewExtension<FRayTracingViewExtension>(this);

#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		GEngine->OnLevelActorAdded().AddUObject(this, &URayTracingWorldSubSystem::OnActorSpawned);
		GEngine->OnLevelActorDeleted().AddUObject(this, &URayTracingWorldSubSystem::OnActorDeleted);
		GEngine->OnLevelActorListChanged().AddUObject(this, &URayTracingWorldSubSystem::OnActorListChanged);
	}
#endif
}

void URayTracingWorldSubSystem::Deinitialize()
{
	Super::Deinitialize();

#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		GEngine->OnLevelActorAdded().RemoveAll(this);
		GEngine->OnLevelActorDeleted().RemoveAll(this);
		GEngine->OnLevelActorListChanged().RemoveAll(this);
	}
#endif
}

TStatId URayTracingWorldSubSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URayTracingWorldSubSystem, STATGROUP_Tickables);
}

void URayTracingWorldSubSystem::Tick(float DeltaTime)
{
	if (RayTracingWorldSettings)
	{
		FRayTracingSettingsRenderProxy TempSettings;
		TempSettings.SamplerPerPixel = 1;
		TempSettings.bEnableRayTracing = RayTracingWorldSettings->bEnableRayTracing;

		FScopeLock Lock(&Mutex);
		SettingsProxy = TempSettings;
	}
}

void URayTracingWorldSubSystem::OnActorSpawned(AActor* Actor)
{
	ARayTracingWorldSettings* WorldSettings = Cast<ARayTracingWorldSettings>(Actor);
	if (WorldSettings)
	{
		RayTracingWorldSettings = WorldSettings;
	}
}

void URayTracingWorldSubSystem::OnActorDeleted(AActor* Actor)
{
	if ((AActor*)RayTracingWorldSettings == Actor)
	{
		RayTracingWorldSettings = nullptr;
	}
}

void URayTracingWorldSubSystem::OnActorListChanged()
{
	for (TActorIterator<ARayTracingWorldSettings> It(GetWorld()); It; ++It)
	{
		RayTracingWorldSettings = *It;
		break;
	}
}
