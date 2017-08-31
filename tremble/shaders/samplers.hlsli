#ifndef SAMPLERSHLSL
#define SAMPLERSHLSL

SamplerState samplerPointWrap : register (s0);
SamplerState samplerPointClamp : register (s1);
SamplerState samplerLinearWrap : register (s2);
SamplerState samplerLinearClamp : register (s3);
SamplerState samplerAnisotropicWrap : register (s4);
SamplerState samplerAnisotropicClamp : register (s5);

#endif