struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR0;
};

struct VS_INSTANCE_INPUT
{
    float4x4 World : TEXCOORD1; // 월드 행렬
    float Size : TEXCOORD2; // 파티클 크기
    float4 InstanceColor : COLOR1; // 인스턴스 색상
};

struct VS_OUTPUT
{
    float4 Pos : SV_Position;
    float4 Color : COLOR0;
    float PointSize : PSIZE;
};

// 정점 셰이더 함수
VS_OUTPUT VS_Main(VS_INPUT input, VS_INSTANCE_INPUT inst)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), inst.World);
    output.Pos = mul(output.Pos, g_mViewProj);
    output.Color = input.Color * inst.InstanceColor;
    output.PointSize = inst.Size; // 파티클 크기 설정
    return output;
}