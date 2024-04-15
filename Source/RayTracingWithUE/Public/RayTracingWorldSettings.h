#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RayTracingWorldSettings.generated.h"

class UEditorActorSubsystem;

UCLASS()
class RAYTRACINGWITHUE_API ARayTracingWorldSettings : public AActor
{
	GENERATED_BODY()
public:

	ARayTracingWorldSettings();
	
	UFUNCTION(CallInEditor, Category = RTWithUE)
	void GatherSceneMeshData();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RayTracingWithUE")
	bool bEnableRayTracing = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = RayTracingWithUE)
	TObjectPtr<UTextureCube> SkyDomeCube = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
