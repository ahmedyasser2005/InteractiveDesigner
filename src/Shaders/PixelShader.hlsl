struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

Texture2D objTexture : register(t0);
SamplerState objSampler : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    return objTexture.Sample(objSampler, input.Tex);
}