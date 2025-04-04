
//float, float2, float3, float4 == vector
//float1x3, float2x3, float3x3, float4x4 = matrix
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;
textureCUBE g_CubeTexture;

#define MAX_LIGHTS 8 // 셰이더에서 처리할 최대 라이트 수 (필요에 따라 조절)

struct Light
{
    int Type; // 0: Directional, 1: Point, 2: Spot
    float3 Position; // 월드 공간 위치 (Point/Spot)
    float3 Direction; // 월드 공간 방향 (Directional/Spot)
    float4 Color; // 라이트 색상 및 강도
    float Range; // 도달 범위 (Point/Spot)
    float Attenuation0; // 감쇠 상수항 (Constant Attenuation)
    float Attenuation1; // 감쇠 선형항 (Linear Attenuation)
    float Attenuation2; // 감쇠 제곱항 (Quadratic Attenuation)
    // --- 스포트라이트용 추가 정보 (필요시) ---
    // float SpotFalloff; // 스포트라이트 감쇠 지수 (Power)
    // float SpotCosInnerAngle; // 스포트라이트 내부 각 코사인값
    // float SpotCosOuterAngle; // 스포트라이트 외부 각 코사인값
};

struct Material
{
    float4  Diffuse;    // 재질의 확산광 반사율 (텍스처와 곱해짐)
    float4  Ambient;    // 재질의 주변광 반사율
    float4  Specular;   // 재질의 정반사광 색상
    float4  Emissive;   // 재질의 자체 발광 색상
    float   Power;      // 재질의 정반사 지수 (Shininess)
};

Material g_Material : MATERIAL;
Light g_Lights[MAX_LIGHTS] : LIGHTS; // 라이트 정보 배열
int g_NumActiveLights; // 현재 객체에 영향을 주는 활성 라이트 개수 (C++에서 설정)

// 빛 관련 전역 변수 추가 (C++에서 설정 필요)
float3 g_LightDirection = normalize(float3(-1.f, -1.f, -1.f)); // 예시: 조명 방향 (카메라 기준?)
float4 g_LightColor = float4(1.f, 1.f, 1.f, 1.f); // 예시: 조명 색상
float4 g_AmbientLightColor = float4(0.2f, 0.2f, 0.2f, 1.f); // 예시: 주변광 색상

// 카메라 위치 (Specular 계산에 필요, C++에서 설정)
float3 g_CameraPosition;

// <<< 재질 속성 전역 변수 추가 (C++에서 설정) >>>
float4 g_MaterialAmbient = float4(0.2f, 0.2f, 0.2f, 1.f); // 재질의 주변광 반사율
float4 g_MaterialDiffuse = float4(1.f, 1.f, 1.f, 1.f); // 재질의 확산광 반사율 (텍스처와 곱해짐)
float4 g_MaterialSpecular = float4(1.f, 1.f, 1.f, 1.f); // 재질의 정반사광 색상
float  g_MaterialSpecularPower = 32.f; // 재질의 정반사 지수 (Shininess)
float4 g_MaterialEmissive = float4(0.f, 0.f, 0.f, 1.f); // 재질의 자체 발광 색상

sampler CubeSampler = sampler_state
{
    texture = g_CubeTexture;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
};

sampler DefaultSampler = sampler_state
{
    texture = g_Texture;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
};

//VertexBuffer = 정점0 정점1 정점2 정점3
//IndexBuffer = 0 1 2 0 2 3 


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
};

// VS -> PS 로 전달할 데이터 구조체 (World Normal 추가)
struct VS_OUT
{
    float4 vPosition : POSITION; // 클립 공간 위치
    float2 vTexcoord : TEXCOORD0; // 텍스처 좌표
    float4 vWorldPos : TEXCOORD1; // 월드 공간 위치 (필요시 사용)
    float3 vNormal : TEXCOORD2; // 월드 공간 법선 벡터 추가!
};


/* 정점의 변환(월드, 뷰, 투영) */
/* 정점의 구성을 바꾼다.  */
VS_OUT VS_MAIN(VS_IN In)      
{
    VS_OUT Out;

    // 월드 변환 (World Position 계산)
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

    // 최종 클립 공간 위치 계산 (World * View * Projection)
    Out.vPosition = mul(Out.vWorldPos, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    // 텍스처 좌표 전달
    Out.vTexcoord = In.vTexcoord;

    // 법선 벡터를 월드 공간으로 변환하여 전달
    // WorldMatrix에 non-uniform scaling이 없다면 이것으로 충분
    // 만약 있다면, WorldInverseTranspose 행렬을 사용해야 함
    Out.vNormal = normalize(mul(In.vNormal, (float3x3) g_WorldMatrix));

    return Out;
}

VS_OUT VS_ORTHO(VS_IN In)
{
    VS_OUT Out;

    // 월드 변환 (World Position 계산)
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

    // 최종 클립 공간 위치 계산 (World * View * Projection)
    Out.vPosition = mul(Out.vWorldPos, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

    // 텍스처 좌표 전달
    Out.vTexcoord = In.vTexcoord;

    // 법선 벡터를 월드 공간으로 변환하여 전달
    // WorldMatrix에 non-uniform scaling이 없다면 이것으로 충분
    // 만약 있다면, WorldInverseTranspose 행렬을 사용해야 함
    Out.vNormal = normalize(mul(In.vNormal, (float3x3) g_WorldMatrix));

    return Out;
}

/* 정점3개가 나온다 */
/* w나누기연산. */
/* 뷰포트변환 */
/* 래스터라이즈 -> 픽셀만들었다. */

struct PS_IN
{
    float4 vPosition : POSITION; // 클립 공간 위치
    float2 vTexcoord : TEXCOORD0; // 텍스처 좌표
    float4 vWorldPos : TEXCOORD1; // 월드 공간 위치 (필요시 사용)
    float3 vNormal : TEXCOORD2; // 월드 공간 법선 벡터 추가!
};

struct PS_OUT
{
    vector vColor : COLOR0;
};

/* 픽셀의 색을 결정한다. */
// vector PS_MAIN(PS_IN In) : COLOR0


PS_OUT PS_MAIN(PS_IN In, float facing : VFACE)
{
    PS_OUT Out;

    // 1. 법선 벡터 방향 보정 (VFACE 사용)
    float3 normal = normalize(In.vNormal * sign(facing));

    // 2. 시선 벡터 계산 (표면 -> 카메라)
    float3 viewDir = normalize(g_CameraPosition - In.vWorldPos.xyz);

    // 3. 라이트 벡터 계산 (표면 -> 광원)
    float3 lightVec = -g_LightDirection; // g_LightDirection이 광원에서 나가는 방향일 경우

    // 4. 주변광(Ambient) 계산
    float4 ambient = g_AmbientLightColor * g_Material.Ambient;

    // 5. 확산광(Diffuse) 계산
    float NdotL = saturate(dot(normal, lightVec));
    float4 diffuse = NdotL * g_LightColor * g_Material.Diffuse;

    // 6. 정반사광(Specular) 계산 (Blinn-Phong 모델 사용)
    float3 halfwayDir = normalize(lightVec + viewDir); // 하프 벡터
    float NdotH = saturate(dot(normal, halfwayDir));
    float specPower = pow(NdotH, g_Material.Power); // 하이라이트 집중도
    float4 specular = specPower * g_LightColor * g_Material.Specular;

    // 7. 텍스처 색상 가져오기
    float4 baseColor = tex2D(DefaultSampler, In.vTexcoord);

    // 8. 최종 색상 조합
    // 최종색상 = 자체발광 + 주변광 + (확산광 * 텍스처) + 정반사광
    Out.vColor.rgb = g_Material.Emissive.rgb + ambient.rgb + (diffuse.rgb * baseColor.rgb) + specular.rgb;

    // 9. 알파 값 처리 (텍스처 알파 * 재질 확산광 알파)
    Out.vColor.a = baseColor.a * g_Material.Diffuse.a;

    // --- 필요하다면 이전 알파 처리 로직 유지 ---
    // if (Out.vColor.a < 0.1f)
    //    discard;
    // Out.vColor.a = 0.5f;
    // ---

    return Out;
}

// 새로운 'Unlit' 픽셀 셰이더
PS_OUT PS_UNLIT(PS_IN In, float facing : VFACE) // VFACE는 양면 렌더링 시 필요할 수 있음
{
    PS_OUT Out;

    // 1. 텍스처 색상 가져오기
    float4 baseColor = tex2D(DefaultSampler, In.vTexcoord);

    // 2. (선택 사항) 재질의 Diffuse 색상을 곱하여 기본 색상 조절
    // baseColor *= g_Material.Diffuse; // Material 구조체 사용

    // 3. 최종 색상으로 텍스처 색상(또는 조절된 색상) 바로 사용
    Out.vColor = baseColor;

    // 4. 알파 값 처리 (기존 PS_MAIN과 유사하게 처리하거나 필요에 맞게 수정)
    //    텍스처 알파와 재질 알파를 곱하는 방식 유지
    Out.vColor.a = baseColor.a * g_Material.Diffuse.a;

    // 5. (선택 사항) 알파 값에 따른 discard 로직 (필요 시)
    // if (Out.vColor.a < 0.1f)
    //    discard;

    return Out;
}

PS_OUT PS_MAIN_RED(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = vector(1.f, 0.f, 0.f, 1.f);
        
    return Out;
}



technique DefaultTechnique
{
    pass DefaultPass
    {

        aLPHAbLENDeNABLE = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        BlendOp = Add;

        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_UNLIT();
    }

    pass DoubleSidedPlane
    {
        // --- 렌더 상태 설정 ---
        // 양면 렌더링을 위해 컬링 비활성화
        CullMode = None;

        // 알파 블렌딩 설정 (기존 코드 유지, 오타 수정)
        AlphaBlendEnable = True; // aLPHAbLENDeNABLE -> AlphaBlendEnable
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        BlendOp = Add;

        // --- 셰이더 설정 ---
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MAIN(); // 빛 계산이 포함된 PS_MAIN 사용
    }

    pass DoubleSidedPlane
    {
        // --- 렌더 상태 설정 ---
        // 양면 렌더링을 위해 컬링 비활성화
        CullMode = None;

        // 알파 블렌딩 설정 (기존 코드 유지, 오타 수정)
        AlphaBlendEnable = True; // aLPHAbLENDeNABLE -> AlphaBlendEnable
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        BlendOp = Add;

        // --- 셰이더 설정 ---
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MAIN(); // 빛 계산이 포함된 PS_MAIN 사용
    }

    pass asdouble
    {
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MAIN_RED();
    }
}