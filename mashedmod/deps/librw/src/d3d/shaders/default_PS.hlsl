struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;
	float4 Color		: COLOR0;
};

sampler2D tex0 : register(s0);

float4 fogColor : register(c0);

// MASHED LOCAL PATCH (P7) -- per-PIXEL fog. (start, end, range, disable), the same
// four floats the VS gets at c14; uploaded to PS c1 by d3ddevice.cpp. TexCoord0.z
// now carries the raw eye depth (= clip w) rather than an already-clamped factor,
// so the linear ramp and its clamp are evaluated here, per pixel, matching D3D9's
// D3DFOG_LINEAR table fog. See MASHED_PATCHES.md P7.
float4 fogData : register(c1);

float4 main(VS_out input) : COLOR
{
	float4 color = input.Color;
#ifdef TEX
	color *= tex2D(tex0, input.TexCoord0.xy);
#endif
	float fog = clamp((input.TexCoord0.z - fogData.y)*fogData.z, fogData.w, 1.0);
	color.rgb = lerp(fogColor.rgb, color.rgb, fog);
	return color;
}
