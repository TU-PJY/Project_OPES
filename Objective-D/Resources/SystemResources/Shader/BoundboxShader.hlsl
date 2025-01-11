cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
    float3 gf3ObjectColor : packoffset(c4);
}

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gf3CameraPosition : packoffset(c8);
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXTURECOORD;
};

struct VS_OUTPUT
{
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float2 uv : TEXTURECOORD;
};

VS_OUTPUT VSBoundboxColor(VS_INPUT input)
{
    VS_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
    output.positionH = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

    return (output);
}

float4 PSBoundboxColor(VS_OUTPUT input) : SV_TARGET
{
    float3 meshColor = gf3ObjectColor;
    float4 RetvalMeshColor = float4(meshColor, 1.0);
  
    float4 outputColor = RetvalMeshColor;
    return outputColor;
}