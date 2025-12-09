// Description: Standard effect for basic lighting, texturing, and shading

cbuffer TransformBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    matrix lwvp;
    float3 viewPosition;
}

cbuffer LightBuffer : register(b1)
{
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float3 lightDirection;
}

cbuffer MaterialBuffer : register(b2)
{
    float4 materialEmissive;
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float materialShininess;
}

cbuffer SettingsBuffer : register(b3)
{
    int useDiffuseMap;
    int useSpecMap;
    int useNormalMap;
    int useBumpMap;
    int useShadowMap;
    float bumpWeight;
    float depthBias;
    float padding; // matches SettingsData::padding

    int useThermal; // matches SettingsData::useThermal
    float baseHeat; // matches SettingsData::baseHeat
    float heatVariation; // matches SettingsData::heatVariation
    float padding2; // matches SettingsData::padding2
}

SamplerState textureSampler : register(s0);

Texture2D diffuseMap : register(t0);
Texture2D specMap : register(t1);
Texture2D normalMap : register(t2);
Texture2D bumpMap : register(t3);
Texture2D shadowMap : register(t4);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float2 texCoord : TEXCOORD;
    float3 dirToLight : TEXCOORD1;
    float3 dirToView : TEXCOORD2;
    float4 lightNDCPosition : TEXCOORD3;
};

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    if (useBumpMap != 0)
    {
        float4 bumpMapColor = bumpMap.SampleLevel(textureSampler, input.texCoord, 0.0f);
        float bumpHeight = (bumpMapColor.r * 2.0f) - 1.0f;
        localPosition += (input.normal * bumpHeight * bumpWeight);
    }

    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.texCoord = input.texCoord;
    output.dirToLight = -lightDirection;

    float4 worldPosition = mul(float4(localPosition, 1.0f), world);
    output.dirToView = normalize(viewPosition - worldPosition.xyz);

    if (useShadowMap)
    {
        output.lightNDCPosition = mul(float4(localPosition, 1.0f), lwvp);
    }

    return output;
}

// ======================
// Thermal helpers
// ======================

float3 HeatToColor(float heat)
{
    heat = saturate(heat);

    float3 color = 0.0;

    if (heat < 0.25)
    {
        float t = heat / 0.25;
        color = lerp(float3(0.0, 0.0, 0.2), float3(0.0, 1.0, 0.5), t);
    }
    else if (heat < 0.5)
    {
        float t = (heat - 0.25) / 0.25;
        color = lerp(float3(0.0, 1.0, 0.5), float3(1.0, 1.0, 0.0), t);
    }
    else if (heat < 0.75)
    {
        float t = (heat - 0.5) / 0.25;
        color = lerp(float3(1.0, 1.0, 0.0), float3(1.0, 0.3, 0.0), t);
    }
    else
    {
        float t = (heat - 0.75) / 0.25;
        color = lerp(float3(1.0, 0.3, 0.0), float3(1.0, 1.0, 1.0), t);
    }

    return color;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 n = normalize(input.worldNormal);
    float3 light = normalize(input.dirToLight);
    float3 view = normalize(input.dirToView);

    // update normal value
    if (useNormalMap != 0)
    {
        float3 t = normalize(input.worldTangent);
        float3 b = normalize(cross(n, t));
        float3x3 tbnw = float3x3(t, b, n);
        float4 normalMapColor = normalMap.Sample(textureSampler, input.texCoord);
        float3 unpackedNormalMap = normalize(float3((normalMapColor.xy * 2.0f) - 1.0f, normalMapColor.z));
        n = normalize(mul(unpackedNormalMap, tbnw));
    }

    // Emissive
    float4 emissive = materialEmissive;

    // Ambient
    float4 ambient = lightAmbient * materialAmbient;

    // Diffuse
    float d = saturate(dot(light, n));
    float4 diffuse = d * lightDiffuse * materialDiffuse;

    // Specular
    float3 r = reflect(-light, n);
    float base = saturate(dot(r, view));
    float s = pow(base, materialShininess);
    float4 specular = s * lightSpecular * materialSpecular;

    // textures
    float4 diffuseMapColor = (useDiffuseMap != 0) ? diffuseMap.Sample(textureSampler, input.texCoord) : 1.0f;
    float4 specMapColor = (useSpecMap != 0) ? specMap.Sample(textureSampler, input.texCoord).r : 1.0f;

    float4 finalColor = (emissive + ambient + diffuse) * diffuseMapColor + (specular * specMapColor);

    // Shadowing
    if (useShadowMap != 0)
    {
        float actualDepth = 1.0f - (input.lightNDCPosition.z / input.lightNDCPosition.w);
        float2 shadowUV = input.lightNDCPosition.xy / input.lightNDCPosition.w;
        float u = (shadowUV.x + 1.0f) * 0.5f;
        float v = 1.0f - (shadowUV.y + 1.0f) * 0.5f;
        if (saturate(u) == u && saturate(v) == v)
        {
            float4 savedColor = shadowMap.Sample(textureSampler, float2(u, v));
            float savedDepth = savedColor.r;
            if (savedDepth > actualDepth + depthBias)
            {
                finalColor = (emissive + ambient) * diffuseMapColor;
            }
        }
    }

// ---- Thermal override ----
    if (useThermal != 0)
    {
    // n and view already computed at the top of PS
    // (you have: float3 n = normalize(input.worldNormal); float3 view = normalize(input.dirToView);)

        float heat = baseHeat; // e.g. 0.8 from C++

    // add variation based on view angle so edges are hotter
        float nDotV = saturate(dot(n, view));
        heat += heatVariation * (1.0f - nDotV); // when looking at grazing angles, heat goes up

        heat = saturate(heat);
        float3 thermalColor = HeatToColor(heat);
        finalColor.rgb = thermalColor;
    }
    
    return finalColor;

}
