#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/StaticMeshActor.h"
#include "RayTracingWorldSubSystem.h"
#include "RayTracingRenderResource.generated.h"

class UBillboardComponent;
class ARayTracingWorldSettings;
class URayTracingWorldSubSystem;

UENUM()
enum class EMaterialType : uint32
{
    E_Light = 0,
    E_Diffuse = 1,
    E_Metal = 2,
    E_Dialectric = 3,
};

USTRUCT()
struct FRayTracingMaterial
{
    GENERATED_USTRUCT_BODY()

    FRayTracingMaterial()
        : MaterialType(EMaterialType::E_Diffuse)
        , Albedo(FLinearColor::White)
        , Emission(0.0f)
        , Roughness(0.0f)
        , RefractiveIndex(1.0f)
    {}
    
    UPROPERTY(EditAnywhere, Category = RayTracingMaterial)
    EMaterialType MaterialType;

    UPROPERTY(EditAnywhere, Category = RayTracingMaterial)
    FLinearColor Albedo;

    UPROPERTY(EditAnywhere, Category = RayTracingMaterial)
    float Emission;

    UPROPERTY(EditAnywhere, Category = RayTracingMaterial)
    float Roughness;

    UPROPERTY(EditAnywhere, Category = RayTracingMaterial)
    float RefractiveIndex;
};

UCLASS(hidecategories = (Collision, Object, Physics, SceneComponent, Activation, "Components|Activation", Mobility), ClassGroup = Rendering, meta = (BlueprintSpawnableComponent))
class URTRenderingComponent : public USceneComponent
{
    GENERATED_UCLASS_BODY()
public:
    
#if WITH_EDITOR
    virtual void PostEditComponentMove(bool bFinished) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
    virtual void OnRegister() override;
    virtual void OnUnregister() override;

    void AddRTComponentToProxy();
    void RemoveRTComponentFromProxy();
    void UpdateDataToProxy();

public:

    UPROPERTY(EditAnywhere, Category = RayTracingMaterial)
    FRayTracingMaterial Material;

    UPROPERTY()
    FVector3f Position = FVector3f::ZeroVector;
    
    UPROPERTY(EditAnywhere, Category = RayTracingMaterial)
    float Radius = 100;

private:
    TObjectPtr<ARayTracingWorldSettings> RayTracingWorldSettings;
};
