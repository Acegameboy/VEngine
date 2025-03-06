// Description: Simple shader, that applies lighting

cbuffer TransformBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    matrix viewPosition;
}

cbuffer SettingBuffer : register(b1)
{
    bool useTexture;
    bool useLighting;
}

cbuffer MaterialBuffer : register(b2)
{
    float4 materialEmissive;
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float materialShininess;
}

cbuffer DirectionalLightBuffer : register(b3)
{
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float3 lightDirection;
}

Texture2D textureMap : register(t0);
SamplerState textureSampler : register(s0);
    

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 textCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 textCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.textCoord = input.textCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 diffuseColor = (useTexture) ? textureMap.Sample(textureSampler, input.textCoord) : 1.0f;
    if (!useLighting)
    {
        return diffuseColor;
    }
    
    //ambient color
        float4 ambient = materialAmbient;
    //diffuse color
    float4 diffuse = materialDiffuse;
    //specular color
    float4 specular = materialSpecular;
    //final color
    float4 finalColor = (materialEmissive + diffuse + ambient) * diffuseColor + specular;
    return finalColor;
}