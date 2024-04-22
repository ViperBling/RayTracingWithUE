#include "RayTracingRenderResource.h"
#include "RayTracingWorldSettings.h"
#include "EngineUtils.h"

URTRenderingComponent::URTRenderingComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , Material()
    , Radius(100)
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
    ComponentTags.Add(TEXT("RTComponent"));
}

#if WITH_EDITOR
void URTRenderingComponent::PostEditComponentMove(bool bFinished)
{
    Super::PostEditComponentMove(bFinished);
    UpdateDataToProxy();
}

void URTRenderingComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    UpdateDataToProxy();
    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void URTRenderingComponent::OnRegister()
{
    Super::OnRegister();
    AddRTComponentToProxy();
}

void URTRenderingComponent::OnUnregister()
{
    RemoveRTComponentFromProxy();
    Super::OnUnregister();
}

void URTRenderingComponent::AddRTComponentToProxy()
{
    USceneComponent* Parent = Cast<USceneComponent>(GetAttachParent());
    if (Parent)
    {
        Position = Parent->GetComponentTransform().GetLocation();
    }
    RayTracingWorldSettings->AddRTSceneComponent(this);
}

void URTRenderingComponent::RemoveRTComponentFromProxy()
{
    RayTracingWorldSettings->RemoveRTSceneComponent(this);
}

void URTRenderingComponent::UpdateDataToProxy()
{
    USceneComponent* Parent = Cast<USceneComponent>(GetAttachParent());
    if (Parent)
    {
        Position = Parent->GetComponentTransform().GetLocation();
    }
    RayTracingWorldSettings->UpdateRTSceneComponent(this);
}
