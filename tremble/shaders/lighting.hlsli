#include "lights.hlsli"
#include "material.hlsli"
#include "cbuffers.hlsli"
#include "shadow_mapping.hlsli"

#ifndef LIGHTINGHLSL
#define LIGHTINGHLSL

#define NUM_LIGHTS 64
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

float ComputeAttenuation(float d, float falloff_start, float falloff_end)
{
	return saturate((falloff_end - d) / (falloff_end - falloff_start));
}

float3 ComputeBlinnPhong(float3 light_color, float3 to_light, float3 to_eye, float3 normal, float3 ambient, MaterialData material)
{
	float3 half_vec = normalize(to_light + to_eye);
	float spec_factor = pow(max(dot(normal, half_vec), 0.0f), 10.0f);

	float3 spec_term = material.specular * spec_factor;

    return (material.diffuse + spec_term) * light_color;
}

float3 ComputePointLight(Light light, float3 ambient, MaterialData mat, float3 position, float3 normal, float3 to_eye)
{
    float3 to_light = light.PositionWS.xyz - position;
	float d = length(to_light);

	float3 color;

    if (d > light.Range)
	{
		color = float3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		to_light /= d;

		float NdotL = max(dot(to_light, normal), 0.0f);
		float3 light_strength = light.Color.xyz * NdotL;

		float attenuation = ComputeAttenuation(d, 0, light.Range);
		light_strength *= attenuation;

		color = ComputeBlinnPhong(light_strength, to_light, to_eye, normal, ambient, mat);
	}

	return color;
}

float3 ComputeDirectionalLight(Light light, float3 ambient, MaterialData mat, float3 normal, float3 to_eye)
{
    float3 to_light = normalize(-light.DirectionWS.xyz);

	float NdotL = max(dot(to_light, normal), 0.0f);
    float3 light_strength = light.Color.xyz * NdotL;

	return ComputeBlinnPhong(light_strength, to_light, to_eye, normal, ambient, mat);
}

float3 ComputeSpotLight(Light light, float3 ambient, MaterialData mat, float3 position, float3 normal, float3 to_eye)
{
    float3 light_to_p = normalize(position - light.PositionWS.xyz);
    float cos_direction = dot(light_to_p, light.DirectionWS.xyz);

    float3 result;

    if (light.SpotlightAngle <= cos_direction)
    {
        result = ComputePointLight(light, ambient, mat, position, normal, to_eye);
    }
    else
    {
        result = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

float3 ComputeDualConeSpotLight(Light light, float3 ambient, MaterialData mat, float3 position, float3 normal, float3 to_eye)
{
    //float3 light_to_p = normalize(position - light.position);
    //float cos_direction = dot(light_to_p, light.direction);

    //float3 result;

    //if (light.cos_cone_outer_angle <= cos_direction)
    //{
    //    PointLight pl;
    //    pl.color = light.color;
    //    pl.falloff_start = light.falloff_start;
    //    pl.position = light.position;
    //    pl.falloff_end = light.falloff_end;

    //    result = smoothstep(light.cos_cone_outer_angle, light.cos_cone_inner_angle, cos_direction) * ComputePointLight(pl, ambient, mat, position, normal, to_eye);;
    //}
    //else
    //{
    //    result = float3(0.0f, 0.0f, 0.0f);
    //}

    //return result;

    return ComputeSpotLight(light, ambient, mat, position, normal, to_eye);

}

float3 ComputeLighting(
	float3 position,
    float4 world_position,
	float3 normal,
	float3 to_eye,
	MaterialData mat,
	float3 ambient,
	StructuredBuffer<Light> lights
)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);

    float shadow;

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        if (lights[i].Enabled)
        {

            if (lights[i].Type != DIRECTIONAL_LIGHT && length(lights[i].PositionWS.xyz - position) > lights[i].Range)
            {
            }
            else
            {
                shadow = 1.0f;
                if (lights[i].ShadowRange != 0)
                {
					shadow = min(shadow, GetShadowAmountPerMap(world_position, lights[i].ShadowIndex + 0));
					if (lights[i].ShadowRange > 1)
					{
						shadow = min(shadow, GetShadowAmountPerMap(world_position, lights[i].ShadowIndex + 1));
						if (lights[i].ShadowRange > 2)
						{
							shadow = min(shadow, GetShadowAmountPerMap(world_position, lights[i].ShadowIndex + 2));
							if (lights[i].ShadowRange > 3)
							{
								shadow = min(shadow, GetShadowAmountPerMap(world_position, lights[i].ShadowIndex + 3));
								if (lights[i].ShadowRange > 4)
								{
									shadow = min(shadow, GetShadowAmountPerMap(world_position, lights[i].ShadowIndex + 4));
									if (lights[i].ShadowRange > 5)
									{
										shadow = min(shadow, GetShadowAmountPerMap(world_position, lights[i].ShadowIndex + 5));
									}
								}
							}
							
						}
					}
				}

                switch (lights[i].Type)
                {
                    case DIRECTIONAL_LIGHT:
                        color += ComputeDirectionalLight(lights[i], ambient, mat, normal, to_eye) * shadow;
                        break;
                    case POINT_LIGHT:
                        color += ComputePointLight(lights[i], ambient, mat, position, normal, to_eye) * shadow;
                        break;
                    case SPOT_LIGHT:
                        color += ComputeSpotLight(lights[i], ambient, mat, position, normal, to_eye) * shadow;
                        break;
                }
            }
        }
    }

	return clamp(color, float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f));
}
#endif