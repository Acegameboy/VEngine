// Description: Simple shader, only uses position

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(1, 1, 0, 1);
}