#pragma once

#include "SceneViewExtension.h"
#include "RayTracingWorldSubSystem.h"

class FRayTracingViewExtension : public FSceneViewExtensionBase
{
public:
	FRayTracingViewExtension(const FAutoRegister& AutoRegister, URayTracingWorldSubSystem* InWorldSubSystem);
	~FRayTracingViewExtension() {}

	// FSceneViewExtensionBase implementation
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
    virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView, const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures) override;
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& InView, const FPostProcessingInputs& Inputs) override;
	// virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;

private:
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;
	// End of FSceneViewExtensionBase implementation
	bool ShouldProcessThisView(const FSceneView& InView);

private:
	URayTracingWorldSubSystem* WorldSubSystem {};
	FRayTracingSettingsRenderProxy SettingsProxy {};

    FIntPoint ViewRectSize;
	FRDGTextureRef RayTracingResultTexture {};
	TRefCountPtr<FRHITexture2D> LastFrameResult;
    FMatrix LastFrameViewMatrix;
};
