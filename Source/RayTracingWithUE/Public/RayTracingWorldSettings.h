#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RayTracingRenderResource.h"
#include "RayTracingWorldSettings.generated.h"

struct FRayTracingMaterial;
class URTRenderingComponent;
class UEditorActorSubsystem;
class UTextureCube;

struct FRTMeshRenderData
{
	FRTMeshRenderData()
		: ComponentID(0)
		, Position(FVector::ZeroVector)
		, Radius(0.0f)
		, Material(FRayTracingMaterial())
	{}

    bool operator==(const FRTMeshRenderData& Other) const
	{
	    return ComponentID == Other.ComponentID;
	}

    uint32 ComponentID;
    FVector3f Position;
    float Radius;
    FRayTracingMaterial Material;
};

struct FRTTriangle
{
    FRTTriangle()
        : PosA(FVector3f::ZeroVector)
        , PosB(FVector3f::ZeroVector)
        , PosC(FVector3f::ZeroVector)
        , NormalA(FVector3f::ZeroVector)
        , NormalB(FVector3f::ZeroVector)
        , NormalC(FVector3f::ZeroVector)
    {}
    
    FVector3f PosA;
    FVector3f PosB;
    FVector3f PosC;
    FVector3f NormalA;
    FVector3f NormalB;
    FVector3f NormalC;
};

struct FRTMeshInfo
{
    FRTMeshInfo()
        : ComponentID(0)
        , FirstTriangleIdx(0)
        , NumTriangles(0)
        , BoundMin(FVector3f::ZeroVector)
        , BoundMax(FVector3f::ZeroVector)
        , Material(FRayTracingMaterial())
    {}
    bool operator==(const FRTMeshInfo& Other) const
    {
        return ComponentID == Other.ComponentID;
    }
    
    uint32 ComponentID;
    uint32 FirstTriangleIdx;
    uint32 NumTriangles;
    FVector3f BoundMin;
    FVector3f BoundMax;
    FRayTracingMaterial Material;
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

    TArray<FRTMeshRenderData> RTRenderData;
    TArray<FRTMeshInfo> RTMeshInfos;
    TArray<FRTTriangle> RTTriangles;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
