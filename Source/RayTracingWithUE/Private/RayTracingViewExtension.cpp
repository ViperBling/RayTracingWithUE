﻿#include "RayTracingViewExtension.h"
#include "ShaderCompilerCore.h"
#include "ScreenPass.h"
#include "SceneTexturesConfig.h"
#include "SceneTextureParameters.h"
#include "ShaderParameterUtils.h"
#include "PixelShaderUtils.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterialInputs.h"

#include "RayTracingWorldSubSystem.h"
#include "RayTracingRendering.h"
#include "ScenePrivate.h"


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

	// auto& View = static_cast<const FViewInfo&>(InView);
	//
	// if (!ShouldProcessThisView(View))
	// {
	// 	return;
	// }
	//
	// FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	// FScreenPassTextureViewport Viewport(View.ViewRect);
	// FRHIBlendState* BlendState = TStaticBlendState<
	// 		CW_RGB, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
	// 		CW_ALPHA
	// 	>::GetRHI();
	//
	// auto* PassParameters = GraphBuilder.AllocParameters<FRayTracingTestPS::FParameters>();
	// PassParameters->RayTracingViewExtension.RenderTargets = RenderTargets;
	// PassParameters->RayTracingViewExtension.SceneTextures = SceneTextures;
	//
	// FPixelShaderUtils::AddFullscreenPass(
	// 	GraphBuilder,
	// 	ShaderMap,
	// 	RDG_EVENT_NAME("RayTracingTest"),
	// 	TShaderMapRef<FRayTracingTestPS>(ShaderMap),
	// 	PassParameters,
	// 	Viewport.Rect,
	// 	BlendState);

	// Add Some Pass
	
}

void FRayTracingViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& InView, const FPostProcessingInputs& Inputs)
{
	check(InView.bIsViewInfo)

	auto& View = static_cast<const FViewInfo&>(InView);
	
	if (!ShouldProcessThisView(View))
	{
		return;
	}

	if (SettingsProxy.bEnableRayTracing)
	{
		FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
		FScreenPassTextureViewport Viewport(View.ViewRect);

		// FRDGTextureRef RayTracingOut{};

		FRDGTextureDesc Desc = FRDGTextureDesc::Create2D(Viewport.Extent, PF_A32B32G32R32F, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV);
		RayTracingResultTexture = GraphBuilder.CreateTexture(Desc, TEXT("RayTracingOut"));

		auto* RayTracingCSParams = GraphBuilder.AllocParameters<FRayTracingCS::FParameters>();
		RayTracingCSParams->View = View.ViewUniformBuffer;
		// RayTracingCSParams->InputSRV = GraphBuilder.CreateSRV(Inputs.SceneTextures->GetContents()->SceneColorTexture);
		RayTracingCSParams->OutputUAV = GraphBuilder.CreateUAV(RayTracingResultTexture);

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("RayTracingCompute"),
			TShaderMapRef<FRayTracingCS>(ShaderMap),
			RayTracingCSParams,
			FComputeShaderUtils::GetGroupCount(Viewport.Rect.Size(), FIntPoint(16, 16)));
	
		auto* FullScreenPassParameters = GraphBuilder.AllocParameters<FRayTracingOutPS::FParameters>();
		FullScreenPassParameters->InRayTracingResult = GraphBuilder.CreateSRV(RayTracingResultTexture);
		FullScreenPassParameters->RayTracingViewExtension.RenderTargets[0] = FRenderTargetBinding((*Inputs.SceneTextures)->SceneColorTexture, ERenderTargetLoadAction::EClear);
	
		FPixelShaderUtils::AddFullscreenPass(
			GraphBuilder,
			ShaderMap,
			RDG_EVENT_NAME("OutputRayTracingResult"),
			TShaderMapRef<FRayTracingOutPS>(ShaderMap),
			FullScreenPassParameters,
			Viewport.Rect,
			TStaticBlendState<CW_RGB>::GetRHI());
	}
}

// void FRayTracingViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass Pass, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
// {
// 	if (Pass == EPostProcessingPass::Tonemap)
// 	{
// 		// InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FRayTracingViewExtension::))
// 	}
// }

bool FRayTracingViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	return Context.GetWorld() == WorldSubSystem->GetWorld();
}

bool FRayTracingViewExtension::ShouldProcessThisView(const FSceneView& InView)
{
	return InView.Family->Scene->GetWorld() == WorldSubSystem->GetWorld();
}
