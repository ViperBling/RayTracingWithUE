#pragma once
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"

BEGIN_SHADER_PARAMETER_STRUCT(FRayTracingViewExtensionParameters,)
	// SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTextures)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FRayTracingCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FRayTracingCS);
	SHADER_USE_PARAMETER_STRUCT(FRayTracingCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

class FRayTracingOutPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FRayTracingOutPS);
	SHADER_USE_PARAMETER_STRUCT(FRayTracingOutPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InRayTracingResult)
		SHADER_PARAMETER_STRUCT(FRayTracingViewExtensionParameters, RayTracingViewExtension)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

IMPLEMENT_GLOBAL_SHADER(FRayTracingCS, "/Plugins/RayTracingWithUE/RayTracing.usf", "RayTracingCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FRayTracingOutPS, "/Plugins/RayTracingWithUE/RayTracing.usf", "RayTracingOutPS", SF_Pixel);