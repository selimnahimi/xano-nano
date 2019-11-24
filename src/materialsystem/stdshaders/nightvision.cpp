// ----------------------------------------------------------------------------
// MYSHADER.CPP
//
// This file defines the C++ component of the example shader.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

// Must include this. Contains a bunch of macro definitions along with the
// declaration of CBaseShader.
#include "BaseVSShader.h"

// We're going to be making a screenspace effect. Therefore, we need the
// screenspace vertex shader.
#include "SDK_screenspaceeffect_vs20.inc"

// We also need to include the pixel shader for our own shader.
// Note that the shader compiler generates both 2.0 and 2.0b versions.
// Need to include both.
#include "passthrough_vs20.inc"
#include "post_nightvision_ps20.inc"

// ----------------------------------------------------------------------------
// This macro defines the start of the shader. Effectively, every shader is
// 
// ----------------------------------------------------------------------------
BEGIN_SHADER(nightvision, "Help for my shader.")

// ----------------------------------------------------------------------------
// This block is where you'd define inputs that users can feed to your
// shader.
// ----------------------------------------------------------------------------
BEGIN_SHADER_PARAMS
SHADER_PARAM(NVLEVEL, SHADER_PARAM_TYPE_FLOAT, "1.0", "")
SHADER_PARAM(FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_FullFrameFB", "")
END_SHADER_PARAMS

// ----------------------------------------------------------------------------
// This is the shader initialization block. This disgusting macro defines
// a bunch of ick that makes this shader work.
// ----------------------------------------------------------------------------
SHADER_INIT
{
	if (params[FBTEXTURE]->IsDefined())
	{
		LoadTexture(FBTEXTURE);
	}
}

// ----------------------------------------------------------------------------
// We want this shader to operate on the frame buffer itself. Therefore,
// we need to set this to true.
// ----------------------------------------------------------------------------
bool NeedsFullFrameBufferTexture(IMaterialVar **params, bool bCheckSpecificToThisFrame /* = true */) const
{
	return true;
}

// ----------------------------------------------------------------------------
// This block should return the name of the shader to fall back to if
// we fail to bind this shader for any reason.
// ----------------------------------------------------------------------------
SHADER_FALLBACK
{
	// Requires DX9 + above
	if (g_pHardwareConfig->GetDXSupportLevel() < 90)
	{
		Assert(0);
		DevMsg("DX level is below 90!");
		return "Wireframe";
	}
	return 0;
}

// ----------------------------------------------------------------------------
// This implements the guts of the shader drawing code.
// ----------------------------------------------------------------------------
SHADER_DRAW
{
	// ----------------------------------------------------------------------------
	// This section is called when the shader is bound for the first time.
	// You should setup any static state variables here.
	// ----------------------------------------------------------------------------
	SHADOW_STATE
	{
		pShaderShadow->EnableTexture(SHADER_SAMPLER0, true); //enables SHADER_TEXTURE_STAGE0

		// Setup the vertex format.
		int fmt = VERTEX_POSITION;
		pShaderShadow->VertexShaderVertexFormat(fmt, 1, 0, 0);

		// We don't need to write to the depth buffer.
		pShaderShadow->EnableDepthWrites(false);

		// Precache and set the screenspace shader.
		DECLARE_STATIC_VERTEX_SHADER(passthrough_vs20);
		SET_STATIC_VERTEX_SHADER(passthrough_vs20);

		// Precache and set the example shader.
		DECLARE_STATIC_PIXEL_SHADER(post_nightvision_ps20);
		SET_STATIC_PIXEL_SHADER(post_nightvision_ps20);
		DefaultFog();
	}

		// ----------------------------------------------------------------------------
		// This section is called every frame.
		// ----------------------------------------------------------------------------
		DYNAMIC_STATE
	{
		BindTexture(SHADER_SAMPLER0, FBTEXTURE, -1);
		
		float Scale = params[NVLEVEL]->GetFloatValue(); // get the value of NVLEVEL and turn it to a float
		float vScale[4] = { Scale, Scale, Scale, 1 }; //new float using NVLEVEL values
		pShaderAPI->SetPixelShaderConstant(0, vScale); //set the first shader variable to our float

		// Use the sdk_screenspaceeffect_vs20 vertex shader.
		DECLARE_DYNAMIC_VERTEX_SHADER(passthrough_vs20);
		SET_DYNAMIC_VERTEX_SHADER(passthrough_vs20);

		// Use our custom pixel shader.
		DECLARE_DYNAMIC_PIXEL_SHADER(post_nightvision_ps20);
		SET_DYNAMIC_PIXEL_SHADER(post_nightvision_ps20);
	}

		// NEVER FORGET THIS CALL! This is what actually
		// draws your shader!
	Draw();
}

END_SHADER