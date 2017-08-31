#ifndef MATERIALHLSL
#define MATERIALHLSL

struct MaterialData
{
	float3 emissive;
	float3 diffuse;
	float3 specular;
	float shininess;
	float3 ambient_reflectance;
};

#endif