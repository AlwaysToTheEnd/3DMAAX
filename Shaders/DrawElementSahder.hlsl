// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 0
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
	float4 Color   : COLOR;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float4 Color   : COLOR;
};


VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;

    InstanceData instData = gInstanceData[instanceID];
    float4x4 world = instData.World;

    vout.PosH = mul(float4(vin.PosL*instData.SizeScale, 0.5f), world);
	vout.PosH = mul(vout.PosH, gViewProj);
	vout.Color = vin.Color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}