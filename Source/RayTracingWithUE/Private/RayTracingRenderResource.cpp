#include "RayTracingRenderResource.h"
#include "RayTracingWorldSettings.h"
#include "EngineUtils.h"

URTRenderingComponent::URTRenderingComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , Material()
    , Radius(100)
{
}

#if WITH_EDITOR
void URTRenderingComponent::PostEditComponentMove(bool bFinished)
{
    Super::PostEditComponentMove(bFinished);
    UpdateDataToProxy();
}

void URTRenderingComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    UpdateDataToProxy();
}
#endif

void URTRenderingComponent::OnRegister()
{
    Super::OnRegister();
    AddRTComponentToProxy();
}

void URTRenderingComponent::OnUnregister()
{
    Super::OnUnregister();
    RemoveRTComponentFromProxy();
}

void URTRenderingComponent::AddRTComponentToProxy()
{
    if (!RayTracingWorldSettings)
    {
        GetRTSettingActor();
    }
    else
    {
        RayTracingWorldSettings->AddRTSceneComponent(this);
    }
}

void URTRenderingComponent::RemoveRTComponentFromProxy()
{
    if (!RayTracingWorldSettings)
    {
        GetRTSettingActor();
    }
    else
    {
        RayTracingWorldSettings->RemoveRTSceneComponent(this);
    }
}

void URTRenderingComponent::UpdateDataToProxy()
{
    if (!RayTracingWorldSettings)
    {
        GetRTSettingActor();
    }
    else
    {
        RayTracingWorldSettings->UpdateRTSceneComponent(this);
    }
}

void URTRenderingComponent::GetRTSettingActor()
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (FActorIterator It(GetWorld()); It; ++It)
        {
            ARayTracingWorldSettings* Settings = Cast<ARayTracingWorldSettings>(*It);
            if (Settings)
            {
                RayTracingWorldSettings = Settings;
            }
        }
    }
}
