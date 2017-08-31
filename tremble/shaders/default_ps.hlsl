#include "cbuffers.hlsli"
#include "samplers.hlsli"
#include "vertex_data.hlsli"
#include "lighting.hlsli"

Texture2D mat_emissive_map : register(t0);
Texture2D mat_ambient_map : register(t1);
Texture2D mat_diffuse_map : register(t2);
Texture2D mat_specular_map : register(t3);
Texture2D mat_shininess_map : register(t4);
Texture2D mat_normal_map : register(t5);

struct VertexOut
{
    float4 PosL : POS_LOCAL;
    float4 PosW : POS_WORLD;
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 Bitangent : BITANGENT;
    float3 Tangent : TANGENT;
    float2 UV : UV;
};

float4 main(VertexOut pin) : SV_TARGET
{
    float3 position = pin.PosW.xyz;
    float3 normal = normalize(pin.Normal);
    float3 to_eye = normalize(eye_pos_w - pin.PosW.xyz);
    
    MaterialData mat;
    mat.ambient_reflectance = material.Reflectance.xyz;
    mat.diffuse = material.DiffuseColor.xyz;
    mat.emissive = material.EmissiveColor.xyz;
    mat.shininess = 20;
    mat.specular = float3(0.2f, 0.2f, 0.2f);
    
    if (material.HasDiffuseTexture || true)
    {
        float4 col = mat_diffuse_map.Sample(samplerPointWrap, pin.UV);
        mat.diffuse = col.xyz;
    }
    
    if (material.HasSpecularTexture)
    {
        mat.specular = mat_specular_map.Sample(samplerPointWrap, pin.UV).xyz;
    
    }
    
    if (material.HasSpecularPowerTexture)
    {
        mat.shininess = mat_shininess_map.Sample(samplerPointWrap, pin.UV).x;
        //mat.shininess = 255;
    }
    
    if (material.HasNormalTexture)
    {
        normal = (mat_normal_map.Sample(samplerPointWrap, pin.UV) * 2.0f - 1.0f).xyz;
        normal = normalize(normal.x * pin.Tangent + normal.y * pin.Bitangent + normal.z * normal);
    }
    
    float4 color = float4(ComputeLighting(
		position,
        pin.PosW,
		normal,
		to_eye,
		mat,
		float3(0.2, 0.2, 0.2),
        lights
	), 1.0f);
    
    return color;
}