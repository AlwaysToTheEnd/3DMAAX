#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

#include "Shaders/LightingUtil.hlsl"

struct VertexIn
{
    float4 m_Position:POSITION;
};

struct VertexOut
{
    float4 m_Position:POSITION;
    float4 m_ClipPosition:TEXCOORD1;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
	float4x4 gTexTransform;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float4x4 gLightView;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

    Light gLights[MaxLights];
};

VertexOut VS(VertexIn vin)
{
	VertexOut Output;
    
    //float4x4 lightViewMatrix=gView;

    //float3 lightPosition=float3(-500.0f,500.0f,-500.0f);
    //float3 dirZ=-normalize(lightPosition);
    //float3 up=float3(0,1,0);
    //float3 dirX=cross(up, dirZ);
    //float3 dirY=cross(dirZ,dirX);

    //lightViewMatrix= float4x4(
    //float4(dirX,-dot(lightPosition,dirX)),
    //float4(dirY,-dot(lightPosition,dirY)),
    //float4(dirZ,-dot(lightPosition,dirZ)),
    //float4(0,0,0,1));
    //lightViewMatrix=transpose(lightViewMatrix);

    Output.m_Position=mul(vin.m_Position,gWorld);
    Output.m_Position=mul(Output.m_Position,gLightView);
    Output.m_Position=mul(Output.m_Position,gProj);

    Output.m_ClipPosition=Output.m_Position;

    return Output;
}

float4 PS(VertexOut pin) : SV_Target
{
    float depth=pin.m_ClipPosition.z/pin.m_ClipPosition.w;
    return float4(depth.xxx,1);
}