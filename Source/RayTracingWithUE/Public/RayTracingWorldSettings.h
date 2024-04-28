#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RayTracingRenderResource.h"
#include "RayTracingWorldSettings.generated.h"

struct FRayTracingMaterial;
class URTRenderingComponent;
class UEditorActorSubsystem;
class UTextureCube;

struct FRTSphereInfo
{
	FRTSphereInfo()
		: ComponentID(0)
		, Position(FVector::ZeroVector)
		, Radius(0.0f)
		, Material(FRayTracingMaterial())
	{}

    uint32 ComponentID;
    FVector3f Position;
    float Radius;
    FRayTracingMaterial Material;

    bool operator==(const FRTSphereInfo& Other) const
    {
        return ComponentID == Other.ComponentID;
    }
};

UCLASS()
class RAYTRACINGWITHUE_API ARayTracingWorldSettings : public AActor
{
	GENERATED_BODY()
public:

	ARayTracingWorldSettings();
	
	UFUNCTION(CallInEditor, Category = RTWithUE)
	void GatherSceneMeshData();

    void AddRTSceneComponent(URTRenderingComponent* RTComponent);
    void RemoveRTSceneComponent(URTRenderingComponent* RTComponent);
    void UpdateRTSceneComponent(URTRenderingComponent* RTComponent);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RayTracingWithUE")
	bool bEnableRayTracing = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = RayTracingWithUE)
	TObjectPtr<UTextureCube> SkyDomeCube = nullptr;

    TArray<FRTSphereInfo> RTSphereInfos;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
