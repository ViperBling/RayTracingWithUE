#include "RayTracingWorldSubSystem.h"
#include "EngineUtils.h"
#include "SceneViewExtension.h"
#include "SystemTextures.h"
#include "RayTracingWorldSettings.h"
#include "Engine/TextureCube.h"

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
		if (RayTracingWorldSettings->SkyDomeCube != nullptr)
		{
			TempSettings.SkyDomeCube = static_cast<FRHITextureCube*>(RayTracingWorldSettings->SkyDomeCube->GetResource()->TextureRHI);
		}
		else
		{
			TempSettings.SkyDomeCube = GSystemTextures.CubeBlackDummy->GetRHI();
		}
		FScopeLock Lock(&Mutex);
		SettingsProxy = TempSettings;

	    for (auto& Data : RayTracingWorldSettings->RTRenderData)
	    {
	        UE_LOG(LogTemp, Warning, TEXT("DataID : %i, Position: %s, Radius: %f"), Data.ComponentID, *Data.Position.ToString(), Data.Radius);
	    }
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
