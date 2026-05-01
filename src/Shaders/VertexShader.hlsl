struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

// This vertex shader generates a full-screen triangle without a vertex buffer
VS_OUTPUT VS(uint id : SV_VertexID)
{
    VS_OUTPUT output;
    output.Tex = float2((id << 1) & 2, id & 2);
    output.Pos = float4(output.Tex * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}