//-----------------------------
// Constant buffers
//-----------------------------
cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp;
    float3 viewPosition;
    float _pad0; // padding
};

// Simple heat parameters for the whole model
cbuffer HeatBuffer : register(b1)
{
    float baseHeat; // 0..1, e.g. 0.8 for "hot person"
    float heatVariation; // 0..1, how much to vary with angle to camera (optional)
    float2 _pad1;
};

SamplerState linearSampler : register(s0);

//-----------------------------
// Vertex / Pixel structs
//-----------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normalW : NORMAL;
    float3 worldPos : TEXCOORD0;
    float2 texCoord : TEXCOORD1;
};

//-----------------------------
// Vertex Shader
//-----------------------------
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 worldPos = mul(float4(input.position, 1.0f), world);
    output.position = mul(worldPos, wvp);
    output.worldPos = worldPos.xyz;

    // Transform normal to world space (assumes no non-uniform scale, otherwise use inverse-transpose)
    output.normalW = mul(float4(input.normal, 0.0f), world).xyz;

    output.texCoord = input.texCoord;
    return output;
}

//-----------------------------
// Heat helpers
//-----------------------------

// Convert scalar heat (0..1) into thermal color
float3 HeatToColor(float heat)
{
    heat = saturate(heat);

    // 0.0 = dark blue
    // 0.25 = cyan/green
    // 0.5 = yellow
    // 0.75 = orange/red
    // 1.0 = white
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

// Compute a scalar heat for this pixel
float ComputeHeat(float3 worldPos, float3 normalW)
{
    // Base constant heat (covers whole model)
    float heat = baseHeat; // e.g. 0.8

    // Optional: add some variation based on angle to camera
    float3 viewDir = normalize(viewPosition - worldPos);
    float nDotV = saturate(dot(normalize(normalW), viewDir));

    // Use rim-light style effect: hotter on edges (1 - nDotV)
    heat += heatVariation * (1.0f - nDotV);

    return saturate(heat);
}

//-----------------------------
// Pixel Shaders
//-----------------------------

// 1) Heat mask: outputs scalar heat in red channel (others unused)
float4 PS_HeatMask(VS_OUTPUT input) : SV_TARGET
{
    float heat = ComputeHeat(input.worldPos, input.normalW);
    return float4(heat, 0.0f, 0.0f, 1.0f);
}

// 2) Full thermal color: directly visible thermal view
float4 PS_HeatColor(VS_OUTPUT input) : SV_TARGET
{
    float heat = ComputeHeat(input.worldPos, input.normalW);
    float3 thermalColor = HeatToColor(heat);
    return float4(thermalColor, 1.0f);
}

//-----------------------------
// Techniques
//-----------------------------
technique11 HeatMaskTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetPixelShader(CompileShader(ps_5_0, PS_HeatMask()));
    }
}

technique11 HeatColorTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetPixelShader(CompileShader(ps_5_0, PS_HeatColor()));
    }
}
