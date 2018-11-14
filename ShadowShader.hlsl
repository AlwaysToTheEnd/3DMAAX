#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

#include "Common.hlsl"

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