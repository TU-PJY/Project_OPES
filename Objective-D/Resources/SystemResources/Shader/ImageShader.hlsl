cbuffer cbGameObjectInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
    float3 gf3ObjectColor : packoffset(c4);
    float AlphaValue : packoffset(c4.w);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gf3CameraPosition : packoffset(c8);
}

cbuffer cbFlipInfo : register(b2)
{
    int X_Flip;
    int Y_Flip;
}

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

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

VS_OUTPUT VSImageColor(VS_INPUT input)
{
    VS_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
    output.positionH = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
   
    output.uv.x = lerp(output.uv.x, 1.0f - output.uv.x, X_Flip);
    output.uv.y = lerp(output.uv.y, 1.0f - output.uv.y, Y_Flip);

    return (output);
}

float4 PSImageColor(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.uv);
    float3 meshColor = gf3ObjectColor;
    
    texColor.rgb += meshColor;

    if (texColor.a < 0.01)
        discard;
  
    float4 outputColor = float4(texColor.rgb, texColor.a * AlphaValue);
    return outputColor;
}