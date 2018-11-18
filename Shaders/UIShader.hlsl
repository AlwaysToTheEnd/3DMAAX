#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
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
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 TexC    : TEXCOORD;

    nointerpolation uint MatIndex : MATINDEX;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    InstanceData instData = gInstanceData[instanceID];
    float4x4 world = instData.World;
    float4x4 texTransform = instData.TexTransform;
    uint matIndex = instData.MaterialIndex;

    vout.MatIndex = matIndex;
	
    MaterialData matData = gMaterialData[matIndex];
	
    vout.PosH = mul(float4(vin.PosL*instData.SizeScale, 1.0f), world);
    vout.PosH = mul(vout.PosH, gViewProj);
	
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    MaterialData matData = gMaterialData[pin.MatIndex];
    uint diffuseTexIndex = matData.DiffuseMapIndex;
	
    if(diffuseTexIndex==5)
    {
        return float4(gShadowMap.Sample(gsamLinearWrap, pin.TexC).rrr, 1.0f);
    }
    else
    {
        return gDiffuseMap[diffuseTexIndex].Sample(gsamLinearWrap, pin.TexC);
    }
}
