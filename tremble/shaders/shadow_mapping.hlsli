#ifndef SHADOWSHLSL
#define SHADOWSHLSL
#include "samplers.hlsli"

cbuffer cbShadowInfo : register(b3)
{
    uint shadow_maps;
}

Texture2DArray shadowMap : register(t9);

struct mapData
{
    float4x4 map_matrix;
};

StructuredBuffer<mapData> shadowMaps : register(t10);

float GetShadowAmountPerMap(float4 pos, int index)
{
    float shade = 0.1f;

     // position on shadow map
    float4 mapPosition = mul(pos, shadowMaps[index].map_matrix);

    mapPosition = mapPosition / mapPosition.w;
   
    if (mapPosition.x < -1.0f || mapPosition.x > 1.0f ||
		mapPosition.y < -1.0f || mapPosition.y > 1.0f ||
		mapPosition.z < 0.0f || mapPosition.z > 1.0f)
    {
        shade = 1.0f;
    }
    else
    {
	    //turn coords into [0, 1] system
        mapPosition.x = mapPosition.x / 2 + 0.5f;
        mapPosition.y = mapPosition.y / -2 + 0.5f;

	    //check for shadow        
        float shadowMapDepth = shadowMap.Sample(samplerPointClamp, float3(mapPosition.xy, index)).r;
		if (shadowMapDepth >= (mapPosition.z - 0.001f)) //TODO fix depth maps into a lower bias (0.0003f)
        {
            shade = 1.0f;
        }
    }
    return shade;
}

float4 GetShadowAmount(float4 pos)
{

    // no maps present
    if (shadow_maps == 0)
    {
        return float4(1, 1, 1, 1);
    }

    float lit = 0.1f;

    for (uint i = 0; i < shadow_maps; i++)
    {
	    // position on shadow map
        float4 mapPosition = mul(pos, shadowMaps[i].map_matrix);

        if (mapPosition.x < -1.0f || mapPosition.x > 1.0f ||
		mapPosition.y < -1.0f || mapPosition.y > 1.0f ||
		mapPosition.z < 0.0f || mapPosition.z > 1.0f)
        {
            // outside shadow map
        }
        else
        {

	    //turn coords into [0, 1] system
            mapPosition.x = mapPosition.x / 2 + 0.5;
            mapPosition.y = mapPosition.y / -2 + 0.5;

	    //check for shadow
            float shadowMapDepth = shadowMap.Sample(samplerLinearClamp, float3(mapPosition.xy, i)).r;
            if (shadowMapDepth >= (mapPosition.z - 0.0002f))
            {
                lit = 1;

            }
        }
    }
    return float4(lit, lit, lit, 1);
}

#endif