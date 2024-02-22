#include "RayTracingViewExtension.h"

void FRayTracingViewExtension::PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView,
	const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures)
{
	FWorldSceneViewExtension::PostRenderBasePassDeferred_RenderThread(GraphBuilder, InView, RenderTargets,
	                                                                  SceneTextures);
}
