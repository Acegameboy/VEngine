cbuffer TransformBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    float3 viewPosition;
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

cbuffer PointLightBuffer : register(b3)
{
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float3 lightPosition; 
    float lightRange; 
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
    float3 worldNormal : Normal;
    float2 textCoord : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldNormal = normalize(mul(input.normal, (float3x3) world)); 
    output.textCoord = input.textCoord;

    output.worldPosition = mul(float4(input.position, 1.0f), world).xyz;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 diffuseColor = (useTexture) ? textureMap.Sample(textureSampler, input.textCoord) : 1.0f;
    if (!useLighting)
    {
        return diffuseColor;
    }
    
    float3 n = normalize(input.worldNormal); 
    float3 L = lightPosition - input.worldPosition; 
    float distance = length(L); 
    L = normalize(L); 

    float3 v = normalize(viewPosition - input.worldPosition); 
    float3 r = reflect(-L, n); 

    float attenuation = saturate(1.0 - (distance / lightRange));

    float4 ambient = lightAmbient * materialAmbient;

    float4 diffuse = max(dot(n, L), 0.0f) * lightDiffuse * materialDiffuse * attenuation;

    float s = pow(max(dot(r, v), 0.0f), materialShininess);
    float4 specular = s * lightSpecular * materialSpecular * attenuation;

    float4 finalColor = (materialEmissive + diffuse + ambient) * diffuseColor + specular;
    return finalColor;
}