cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
    float4x4 gViewProj;
};

struct VS_VertexIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
};

struct VS_VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float4 Color : COLOR;
};

VS_VertexOut VS(VS_VertexIn vin)
{
    VS_VertexOut vout;
    
    // Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
    
    vout.PosW = posW.xyz;
    
    vout.Color = vin.Color;
	
    return vout;
}

float4 PS(VS_VertexOut pin) : SV_Target
{
    //float4 raytracing = float4(0.5f, 0.5f, 0.5f, 1.0f);
    return pin.Color;
}