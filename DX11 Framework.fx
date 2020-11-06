//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);


cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 diffuseMat;
    float4 diffuseLight;
    float4 ambientMat;
    float4 ambientLight;
    float4 specularMat;
    float4 specularLight;
    float specularPower;
    float3 eyePosW;
    float3 lightVecW;

}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 normalW : NORMAL;
    float3 eye : POSITION;
    float2 Tex : TEXCOORD;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Tex : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.eye = normalize(eyePosW.xyz - output.Pos.xyz);
    float3 normalW = mul(float4(Normal, 0.0f), World).xyz;
    output.normalW = normalize(normalW);
    output.Tex = Tex;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    //normalise
    input.normalW = normalize(input.normalW);

    //calculate lighting
    float reflection = reflect(-lightVecW, input.normalW);
    float diffuseAmount = max(dot(lightVecW, input.normalW), 0.0f);
    float specularAmount = pow(max(dot(reflection, input.eye), 0), specularPower);

    float3 specular = specularAmount * (specularMat * specularLight).rgb;
    float3 ambient = ambientMat * ambientLight;
    float3 diffuse = diffuseAmount * (diffuseMat * diffuseLight).rgb;

    //clamp color values between 0 and 1
    float4 colour;
    colour.rgb = clamp(diffuse, 0, 1) + ambient + clamp(specular, 0, 1);
    colour.a = diffuseMat.a;

    float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);

    return colour;
}
