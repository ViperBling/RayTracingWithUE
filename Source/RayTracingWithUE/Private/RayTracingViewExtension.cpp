#include "RayTracingViewExtension.h"
#include "ShaderCompilerCore.h"
#include "ScreenPass.h"
#include "SceneTexturesConfig.h"
#include "SceneTextureParameters.h"
#include "ShaderParameterUtils.h"
#include "PixelShaderUtils.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterialInputs.h"
#include "RayTracingWorldSettings.h"
#include "RayTracingWorldSubSystem.h"
#include "RayTracingRendering.h"
#include "ScenePrivate.h"

FRayTracingViewExtension::FRayTracingViewExtension(const FAutoRegister& AutoRegister, URayTracingWorldSubSystem* InWorldSubSystem)
	: FSceneViewExtensionBase(AutoRegister)
	, WorldSubSystem(InWorldSubSystem)
    , ViewRectSize(FIntPoint::ZeroValue)
    , LastFrameViewMatrix(FMatrix::Identity)
{
}

void FRayTracingViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	SettingsProxy = WorldSubSystem->GetSettings();
}

void FRayTracingViewExtension::PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView)
{
}

void FRayTracingViewExtension::PostRenderBasePassDeferred_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView, const FRenderTargetBindingSlots& RenderTargets, TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures)
{
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
	    
		FRDGTextureDesc Desc = FRDGTextureDesc::Create2D(Viewport.Extent, PF_A32B32G32R32F, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		RayTracingResultTexture = GraphBuilder.CreateTexture(Desc, TEXT("RayTracingOut"), ERDGTextureFlags::MultiFrame);

	    uint32 MeshCount = SettingsProxy.RTSphereInfos.Num();
	    TRefCountPtr<FRDGPooledBuffer> RTSphereInfoBuffer;
		auto& RHICmdList = GraphBuilder.RHICmdList;
		FRDGBufferDesc BufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FRTSphereInfo), FMath::Clamp(MeshCount, 1, MeshCount));
		AllocatePooledBuffer(BufferDesc, RTSphereInfoBuffer, TEXT("RTSphereInfoBuffer"));

	    if (MeshCount > 0)
	    {
	        void* BufferData = RHICmdList.LockBuffer(RTSphereInfoBuffer->GetRHI(), 0, sizeof(FRTSphereInfo) * MeshCount, RLM_WriteOnly);
	        FMemory::Memcpy(BufferData, SettingsProxy.RTSphereInfos.GetData(), sizeof(FRTSphereInfo) * MeshCount);
	        RHICmdList.UnlockBuffer(RTSphereInfoBuffer->GetRHI());
	    }
	    
		auto SphereInfoBuffer = GraphBuilder.RegisterExternalBuffer(RTSphereInfoBuffer, TEXT("SphereInfoBuffer"));
	    
	    bool bShouldResize = false;
	    if (ViewRectSize != Viewport.Extent || LastFrameViewMatrix != InView.ViewMatrices.GetViewMatrix())
	    {
	        ViewRectSize = Viewport.Extent;
	        LastFrameViewMatrix = InView.ViewMatrices.GetViewMatrix();
	        bShouldResize = true;
	    }
	    
	    if (!LastFrameResult.IsValid() || bShouldResize)
	    {
	        FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("LastFrameResult"), Viewport.Extent, PF_A32B32G32R32F);
	        CreateDesc.SetFlags(ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable);
	        LastFrameResult = RHICreateTexture(CreateDesc);

	        FrameCounter = 0;
	    }
	    FRDGTextureRef LastFrameResultRDG = RegisterExternalTexture(GraphBuilder, LastFrameResult, TEXT("LastFrameResult"));
	    
		auto* RayTracingCSParams = GraphBuilder.AllocParameters<FRayTracingCS::FParameters>();
		RayTracingCSParams->View = View.ViewUniformBuffer;
		RayTracingCSParams->RTSphereInfoBuffer = GraphBuilder.CreateSRV(SphereInfoBuffer, PF_R32G32B32F);
	    RayTracingCSParams->NumSpheres = MeshCount;
	    RayTracingCSParams->FrameCounter = FrameCounter;
		RayTracingCSParams->SkyDomeCube = SettingsProxy.SkyDomeCube == nullptr ? GSystemTextures.CubeBlackDummy->GetRHI() : SettingsProxy.SkyDomeCube;
		RayTracingCSParams->SkyDomeCubeSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
		RayTracingCSParams->LastFrameResult = GraphBuilder.CreateSRV(LastFrameResultRDG);
		RayTracingCSParams->OutputUAV = GraphBuilder.CreateUAV(RayTracingResultTexture);
	    
		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("RayTracingCompute"),
			TShaderMapRef<FRayTracingCS>(ShaderMap),
			RayTracingCSParams,
			FComputeShaderUtils::GetGroupCount(Viewport.Extent, FIntPoint(16, 16)));

	    // 将RayTracingResultTexture内容复制到LastFrameResult
	    FRHICopyTextureInfo CopyInfo;
	    AddCopyTexturePass(GraphBuilder, RayTracingResultTexture, LastFrameResultRDG, CopyInfo);
	    
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

	    FrameCounter++;
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
