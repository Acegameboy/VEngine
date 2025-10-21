// Description: Cel Shader that uses cel shading effect on objects
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
    float3 worldNormal : Normal;
    float2 textCoord : TEXCOORD0;
    float3 dirToLight : TEXCOORD1;
    float3 dirToView : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3)world);
    output.textCoord = input.textCoord;
    
    float3 worldPosition = mul(float4(input.position, 1.0f), world).xyz;
    output.dirToLight = -lightDirection;
    output.dirToView = normalize(viewPosition - worldPosition);
    
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
    float3 L = normalize(input.dirToLight);
    float3 v = normalize(input.dirToView);
    
    //Emissive
    float edgeThickness = 0.85f;
    float edgeThreshold = 0.01f;
    float e = 1.0f - saturate(dot(view, n));
    e = smoothstep(edgeThickness - 0.01f, edgeThickness + 0.1f,  e);
    float4 emissive = materialEmissive;
    //ambient color
    float4 ambient = lightAmbient * materialAmbient;
    //diffuse color
    float4 d = max(dot(L, n), 0.0f) * lightDiffuse * materialDiffuse;
    d = smoothstep(0.005f, 0.1f, d);
    float4 diffuse = d * lightDiffuse * materialDiffuse;
    //specular color
    float3 r = reflect(-L, n);
    float s = pow(max(dot(r, n), 0.0f), materialShininess);
    s = smoothstep(0.005f, 0.1f, s);
    float4 specular = s * lightSpecular * materialSpecular;
    //final color
    float4 finalColor = (emissive + diffuse + ambient) * diffuseColor + specular;
    return finalColor;
}