#include "RayTracingViewExtension.h"

FRayTracingViewExtension::FRayTracingViewExtension(const FAutoRegister& AutoRegister, URayTracingWorldSubSystem* InWorldSubSystem)
	: FSceneViewExtensionBase(AutoRegister)
	, WorldSubSystem(InWorldSubSystem)
{
}

void FRayTracingViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	SettingsProxy = WorldSubSystem->GetSettings();
}

void FRayTracingViewExtension::PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView, const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures)
{
	check(InView.bIsViewInfo)
}

void FRayTracingViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& InView, const FPostProcessingInputs& Inputs)
{
	check(InView.bIsViewInfo)
}

void FRayTracingViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass Pass, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if (Pass == EPostProcessingPass::Tonemap)
	{
		// InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FRayTracingViewExtension::))
	}
}

bool FRayTracingViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	return Context.GetWorld() == WorldSubSystem->GetWorld();
}
