#pragma once
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"

BEGIN_SHADER_PARAMETER_STRUCT(FRayTracingViewExtensionParameters,)
	// SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTextures)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FRayTracingTestPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FRayTracingTestPS);
	SHADER_USE_PARAMETER_STRUCT(FRayTracingTestPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT(FRayTracingViewExtensionParameters, RayTracingViewExtension)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};
IMPLEMENT_GLOBAL_SHADER(FRayTracingTestPS, "/Plugins/RayTracingWithUE/RayTracing.usf", "RayTracingTestPS", SF_Pixel);