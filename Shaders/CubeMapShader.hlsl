#include "Common.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};
 
VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.PosL = vin.PosL;
	
    float4 posW = mul(float4(vin.PosL, 1.0f), gInstanceData[0].World);

    posW.xyz += gEyePosW;

    vout.PosH = mul(posW, gViewProj).xyww;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return gCubeTexture.Sample(gsamLinearWrap, pin.PosL);
}

