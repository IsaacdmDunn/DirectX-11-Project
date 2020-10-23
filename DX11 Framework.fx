//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

    float4 DiffuseMtrl;
    float4 DiffuseLight;
    float4 AmbientMtrl;
    float4 AmbientLight;
    float4 SpecularLight;
    float4 SpecularMtrl;
    float SpecularPower;
    float3 padding;
    float3 EyePosW;
    float padding2;
    float3 LightVecW;

}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float3 EyePos : POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float4 Normal : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    float3 toEye = normalize(EyePosW - output.Pos.xyz);
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Norm = Normal;

    
    output.EyePos = EyePosW;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    // fragment to light vector data
    const float3 vToL = LightVecW - input.Pos.xyz;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    // diffuse intensity
    const float3 diffuse = DiffuseMtrl * 1.0f * max(0.0f, dot(dirToL, input.Norm));
    // reflected light vector
    const float3 w = input.Norm * dot(vToL, input.Norm);
    const float3 r = w * 2.0f - vToL;

    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = (DiffuseMtrl * 1.0f) * SpecularPower * pow(max(0.0f, dot(normalize(-r), normalize(input.Pos.xyz))), SpecularPower);
    // final color

    float3 ambient = AmbientMtrl * AmbientLight;

    float4 finalColour = float4(diffuse + ambient + specular, 1.0f);

    return float4(finalColour);
}
