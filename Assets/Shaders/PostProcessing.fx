// Simple shader that applies post processing effect to a scene
cbuffer PostProcessBuffer : register(b0)
{
    int mode = 0;
    float param0 = 0.0f;
    float param1 = 0.0f;
    float param2 = 0.0f;
}

Texture2D tuxtureMap0 : register(t0);

Texture2D tuxtureMap1 : register(t1);

Texture2D tuxtureMap2 : register(t2);

Texture2D tuxtureMap3 : register(t3);

SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 textCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    output.textCoord = input.texCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 finalColor = 0.0f;
    if (mode == 0)//None
    {
        finalColor = tuxtureMap0.Sample(textureSampler, input.textCoord);
    }
    else if (mode == 1)//monochrome
    {
        float4 color = tuxtureMap0.Sample(textureSampler, input.textCoord);
        finalColor = (color.r + color.g + color.b) / 3.0f;

    }
    else if (mode == 2)//invert
    {
        float4 color = tuxtureMap0.Sample(textureSampler, input.textCoord);
        finalColor = 1.0f - color;
    }
    else if (mode == 3)// mirror
    {
        float2 texCoord = input.textCoord;
        texCoord.x *= param0;
        texCoord.y *= param1;
        finalColor = tuxtureMap0.Sample(textureSampler, input.textCoord);
    }
    else if(mode == 4)//blur
    {
        float u = input.textCoord.x;
        float v = input.textCoord.y;
        finalColor =
    tuxtureMap0.Sample(textureSampler, float2(u, v))
    + tuxtureMap0.Sample(textureSampler, float2(u + param0, v))
    + tuxtureMap0.Sample(textureSampler, float2(u - param0, v))
    + tuxtureMap0.Sample(textureSampler, float2(u, v + param1))
    + tuxtureMap0.Sample(textureSampler, float2(u, v - param1))
    + tuxtureMap0.Sample(textureSampler, float2(u + param0, v + param1))
    + tuxtureMap0.Sample(textureSampler, float2(u + param0, v - param1))
    + tuxtureMap0.Sample(textureSampler, float2(u - param0, v + param1))
    + tuxtureMap0.Sample(textureSampler, float2(u - param0, v - param1));
        finalColor *= 0.12f;
    }
    else if (mode == 5)//combine2
    {
        float4 color0 = tuxtureMap0.Sample(textureSampler, input.textCoord);
        float4 color1 = tuxtureMap1.Sample(textureSampler, input.textCoord);
        //finalColor = (color0 + color1) * 0.5f;
        color1.a *= param0;
        finalColor = (color0 * (1.0f - color1.a)) + (color1 * color1.a);

    }
    else if (mode == 6) //motion blur
    {
        float u = input.textCoord.x;
        float v = input.textCoord.y;
        
        float dist = distance(input.textCoord, float2(0.5f, 0.5f));
        float weight = saturate(lerp(0.0f, 1.0f, (dist - 0.25f) / 0.25f));
        float p0 = param0 * weight;
        float p1 = param1 * weight;
        
        finalColor =
    tuxtureMap0.Sample(textureSampler, float2(u, v))
    + tuxtureMap0.Sample(textureSampler, float2(u + p0, v))
    + tuxtureMap0.Sample(textureSampler, float2(u - p0, v))
    + tuxtureMap0.Sample(textureSampler, float2(u, v + p1))
    + tuxtureMap0.Sample(textureSampler, float2(u, v - p1))
    + tuxtureMap0.Sample(textureSampler, float2(u + p0, v + p1))
    + tuxtureMap0.Sample(textureSampler, float2(u + p0, v - p1))
    + tuxtureMap0.Sample(textureSampler, float2(u - p0, v + p1))
    + tuxtureMap0.Sample(textureSampler, float2(u - p0, v - p1));
        finalColor *= 0.12f;
    }
    else if (mode == 7) //chromatic aberration
    {
        float2 distortion = float2(param0, -param1);
        float4 redChannel = tuxtureMap0.Sample(textureSampler, input.textCoord + distortion.x * input.textCoord);
        float4 greenChannel = tuxtureMap0.Sample(textureSampler, input.textCoord);
        float4 blueChannel = tuxtureMap0.Sample(textureSampler, input.textCoord + distortion.y * input.textCoord);
        finalColor = float4(redChannel.r, greenChannel.g, blueChannel.b, 1.0f);      
    }
    else if (mode == 8) //wave
    {
        float waveValue = input.textCoord.x * (3.141592f * param1);
        float2 texCoord = input.textCoord;
        texCoord.y += sin(waveValue) * param0;
        finalColor = tuxtureMap0.Sample(textureSampler, input.textCoord);

    }
        return finalColor;
}