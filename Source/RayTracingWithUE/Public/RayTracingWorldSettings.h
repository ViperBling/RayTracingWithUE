#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RayTracingRenderResource.h"
#include "RayTracingWorldSettings.generated.h"

struct FRayTracingMaterial;
class URTRenderingComponent;
class UEditorActorSubsystem;
class UTextureCube;

USTRUCT()
struct FRTMeshRenderData
{
    GENERATED_BODY()

    UPROPERTY()
    uint32 ComponentID;
    
    UPROPERTY()
    FVector Position;
    
    UPROPERTY()
    float Radius;
    
    UPROPERTY()
    FRayTracingMaterial Material;

    bool operator==(const FRTMeshRenderData& Other) const
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

    UPROPERTY()
    TArray<FRTMeshRenderData> RTRenderData;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
