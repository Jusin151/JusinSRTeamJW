struct VS_INPUT
{
    float3 position : POSITION; // 파티클 기본 위치
    float4 color : COLOR; // 파티클 색상
    float3 instanceOffset : TEXCOORD0; // 인스턴싱 오프셋
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

float4x4 g_mWorldViewProj;
float g_fTimeDelta;

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // 인스턴싱된 위치 적용
    float3 worldPos = input.position + input.instanceOffset;
    
    // 중력 적용 (시간에 따라 Y축 감소)
    worldPos.y -= g_fTimeDelta * 0.5;
    
    // 월드뷰프로젝션 변환
    output.position = mul(float4(worldPos, 1.0f), g_mWorldViewProj);
    
    // 색상 전달
    output.color = input.color;
    
    return output;
}