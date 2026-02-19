//Renders a particle with color and a texture

cbuffer ParticleBuffer : register(b0)
{
    matrix wvp;
}

cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

Texture2D textureMap : register(t0);
SamplerState textureSampler : register(s0);

struct VS_Input
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_Output
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

VS_Output VS(VS_Input input)
{
    VS_Output output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.texCoord = input.texCoord;
    return output;
}
float4 PS(VS_Output input) : SV_Target
{
    return textureMap.Sample(textureSampler, input.texCoord) * color;

}