#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RayTracingWorldSubSystem.generated.h"

struct FRTTriangle;
struct FRTMeshInfo;
struct FRTMeshRenderData;
class ARayTracingWorldSettings;

struct FRayTracingSettingsRenderProxy
{
	bool bEnableRayTracing;
	int SamplerPerPixel;
	FRHITextureCube* SkyDomeCube;
	TArray<FRTMeshRenderData> RTRenderData;

    TArray<FRTMeshInfo> RTMeshInfos;
    TArray<FRTTriangle> RTTriangles;
};

UCLASS()
class RAYTRACINGWITHUE_API URayTracingWorldSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool IsTickableInEditor() const override { return true; }
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

	void OnActorSpawned(AActor* Actor);
	void OnActorDeleted(AActor* Actor);
	void OnActorListChanged();

	FRayTracingSettingsRenderProxy GetSettings() const
	{
		FScopeLock Lock(&Mutex);
		return SettingsProxy;
	}

private:
	TSharedPtr<class FRayTracingViewExtension, ESPMode::ThreadSafe> RayTracingViewExtension;
	ARayTracingWorldSettings* RayTracingWorldSettings {};
	mutable FCriticalSection Mutex;
	FRayTracingSettingsRenderProxy SettingsProxy = {};
};