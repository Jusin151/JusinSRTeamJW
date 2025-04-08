
// 카메라 위치 (Specular 계산에 필요, C++에서 설정)
float3 g_CameraPosition;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;
textureCUBE g_CubeTexture;

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

    AddressU = Wrap;
    AddressV = Wrap;
};

#define MAX_LIGHTS 8 // 셰이더에서 처리할 최대 라이트 수 (필요에 따라 조절)

struct Light
{
    int     Type; // 0: UnUsed 1: Point, 2: Spot, 3: Direction
    float3 _unused_;
    float4  Color; // 라이트 색상 및 강도
    float4  Position; // 월드 공간 위치 (Point/Spot)
    float4  Direction; // 월드 공간 방향 (Directional/Spot)
    float   Range; // 도달 범위 (Point/Spot)
    float   Attenuation0; // 감쇠 상수항 (Constant Attenuation)
    float   Attenuation1; // 감쇠 선형항 (Linear Attenuation)
    float   Attenuation2; // 감쇠 제곱항 (Quadratic Attenuation)
    // --- 스포트라이트용 추가 정보 (필요시) ---
    // float SpotFalloff; // 스포트라이트 감쇠 지수 (Power)
    // float SpotCosInnerAngle; // 스포트라이트 내부 각 코사인값
    // float SpotCosOuterAngle; // 스포트라이트 외부 각 코사인값
};
Light g_LightTest : LIGHT;
Light g_Lights[MAX_LIGHTS] : LIGHTS; // 라이트 정보 배열
int g_NumActiveLights; // 현재 객체에 영향을 주는 활성 라이트 개수 (C++에서 설정)

struct Material
{
    float4 Diffuse; // 재질의 확산광 반사율 (텍스처와 곱해짐)
    float4 Ambient; // 재질의 주변광 반사율
    float4 Specular; // 재질의 정반사광 색상
    float4 Emissive; // 재질의 자체 발광 색상
    float Power; // 재질의 정반사 지수 (Shininess)
};

Material g_Material : MATERIAL;

// 빛 관련 전역 변수 추가 (C++에서 설정 필요)
float3 g_LightPosition = float3(0.8f, 1.0f, 8.f);
float3 g_LightDirection = normalize(float3(0.f, -1.f, 0.f)); // 예시: 조명 방향 (카메라 기준?)
float4 g_LightColor = float4(0.3f, 0.3f, 0.3f, 1.f); // 예시: 조명 색상
float4 g_AmbientLightColor = float4(0.5f, 0.5f, 0.5f, 1.f); // 예시: 주변광 색상

// <<< 재질 속성 전역 변수 추가 (C++에서 설정) >>>
float4 g_MaterialAmbient = float4(0.2f, 0.2f, 0.2f, 1.f); // 재질의 주변광 반사율
float4 g_MaterialDiffuse = float4(1.f, 1.f, 1.f, 1.f); // 재질의 확산광 반사율 (텍스처와 곱해짐)
float4 g_MaterialSpecular = float4(1.f, 1.f, 1.f, 1.f); // 재질의 정반사광 색상
float g_MaterialSpecularPower = 32.f; // 재질의 정반사 지수 (Shininess)
float4 g_MaterialEmissive = float4(0.f, 0.f, 0.f, 1.f); // 재질의 자체 발광 색상


// 타일링 전역변수 추가
float2 g_ScaleFactor    = float2(1.0f, 1.0f);
float2 g_OffsetFactor   = float2(0.0f, 0.0f);

// --- 안개 효과 전역 변수 추가 (C++에서 설정) ---
float3 g_FogColor = float3(0.5f, 0.9f, 0.9f); // 안개 색상 (하얀색)
float g_FogStart = 8.0f; // 안개 시작 거리
float g_FogEnd = 20.0f; // 안개 끝 거리

struct VS_IN
{
    float3 vPosition    : POSITION;
    float3 vNormal      : NORMAL0;
    float2 vTexcoord    : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition    :  POSITION;  // 클립 공간 위치
    float2 vTexcoord    :  TEXCOORD0; // 텍스처 좌표
    float4 vWorldPos    :  TEXCOORD1; // 월드 공간 위치 (필요시 사용)
    float3 vNormal      :  TEXCOORD2; // 월드 공간 법선 벡터 추가!
    float3 vViewPos     :  TEXCOORD3; // 뷰 공간 위치 (안개 계산용) <-- 추가
    float3 vScale       :  TEXCOORD4;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    // 월드 변환 (World Position 계산)
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    float3 tmpX = g_WorldMatrix._11_12_13;
    float3 tmpY = g_WorldMatrix._21_22_23;
    float3 tmpZ = g_WorldMatrix._31_32_33;
    
    float3 scale = { length(tmpX), length(tmpY), length(tmpZ) };
    
    Out.vScale = scale;
    
    // 뷰 공간 위치 계산 (World * View) <-- 추가
    Out.vViewPos = mul(Out.vWorldPos, g_ViewMatrix).xyz;

    // 최종 클립 공간 위치 계산 (World * View * Projection)
    //Out.vPosition = mul(Out.vWorldPos, g_ViewMatrix);
    Out.vPosition = mul(float4(Out.vViewPos, 1.f), g_ProjMatrix);

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
    // 뷰 공간 위치 계산 (World * View) <-- 추가
    Out.vViewPos = mul(Out.vWorldPos, g_ViewMatrix).xyz;

    // 최종 클립 공간 위치 계산 (World * View * Projection)
    //Out.vPosition = mul(Out.vWorldPos, g_ViewMatrix);
    //Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    Out.vPosition = mul(float4(Out.vViewPos, 1.f), g_ProjMatrix); // vViewPos 사용하도록 수정
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
    float3 vViewPos : TEXCOORD3; // 뷰 공간 위치 (안개 계산용) <-- 추가
    float3 vScale : TEXCOORD4;
};

struct PS_OUT
{
    vector vColor : COLOR0;
};

/* 픽셀의 색을 결정한다. */
PS_OUT PS_LIT(PS_IN In, float facing : VFACE)
{
    PS_OUT Out;

    // --- 조명 계산에 필요한 벡터들 계산 ---
    float3 normal = normalize(In.vNormal * sign(facing));
    float3 viewDir = normalize(g_CameraPosition - In.vWorldPos.xyz);
    //float3 lightVec = -g_LightDirection; // 라이트 방향

    // --- 텍스처 샘플링 ---
    float2 tiledUV = In.vTexcoord * g_ScaleFactor + g_OffsetFactor;
    float4 baseColor = tex2D(DefaultSampler, tiledUV);

    // --- 조명 요소 계산 ---

    // --- 최종 색상 계산을 위한 변수 초기화 ---
    // 1. 기본 색상 계산 (Ambient + Emissive)
    float3 accumulatedColor = (baseColor.rgb * g_Material.Ambient.rgb * g_Material.Diffuse.rgb) + g_Material.Emissive.rgb;
    
    // --- 전역(Global) 방향성 라이트 계산 추가 ---
    /*float3 globalLightVec = normalize(-g_LightDirection); // 라이트를 향하는 벡터
    float NdotL_global = saturate(dot(normal, globalLightVec)); // 법선과 라이트 벡터 내적

    if (NdotL_global > 0.0f) // 라이트를 받는 부분만 계산
    {
        // 전역 라이트 Diffuse 계산
        float3 globalDiffuse = baseColor.rgb * g_Material.Diffuse.rgb * NdotL_global * g_LightColor.rgb;

        // 전역 라이트 Specular 계산
        float3 halfwayDir_global = normalize(globalLightVec + viewDir);
        float NdotH_global = saturate(dot(normal, halfwayDir_global));
        float specPower_global = pow(NdotH_global, g_Material.Power); // 재질의 Power 사용
        float3 globalSpecular = g_Material.Specular.rgb * specPower_global * g_LightColor.rgb;

        // 전역 라이트 기여도 누적 (방향성 라이트는 감쇠 없음)
        accumulatedColor += (globalDiffuse + globalSpecular);
    }*/
    // --- 전역 라이트 계산 끝 ---

    // 2. 각 라이트 슬롯에 대한 기여도 계산 및 누적
    for (int i = 0; i < g_NumActiveLights; ++i)
    {
        
        // 현재 처리할 라이트 정보 가져오기
        Light currentLight = g_Lights[i];

        // --- 타입 확인: 0번(LT_UNUSED)이면 계산 건너뛰기 ---
        if (currentLight.Type == 0) // LT_UNUSED
        {
            continue; // 다음 라이트로 넘어감
        }
        
        

        // 현재 라이트 계산에 필요한 변수 초기화
        float3 lightVec = float3(0.0f, 0.0f, 0.0f); // 픽셀에서 라이트로 향하는 정규화된 벡터
        float attenuation = 0.0f; // 라이트 감쇠 값 (0.0 ~ 1.0)
        float NdotL = 0.0f; // 법선과 라이트 벡터 내적 값

        // 라이트 타입별 계산 (타입 번호 변경됨!)
        if (currentLight.Type == 3) // Directional Light (LT_DIR = 3)
        {
            lightVec = normalize(-currentLight.Direction).xyz;
            NdotL = saturate(dot(normal, lightVec));
            attenuation = 1.0f; // 방향성은 감쇠 없음
        }
        else if (currentLight.Type == 1) // Point Light (LT_POINT = 1)
        {
            float3 dirToLight = currentLight.Position.xyz - In.vWorldPos.xyz;
            float distSq = dot(dirToLight, dirToLight);
            float dist = sqrt(distSq);

            if (dist < currentLight.Range)
            {
                lightVec = dirToLight / dist;
                NdotL = saturate(dot(normal, lightVec));
                attenuation = saturate(1.0f / (currentLight.Attenuation0 +
                                                currentLight.Attenuation1 * dist +
                                                currentLight.Attenuation2 * distSq));
            }
            else
            {
                NdotL = 0.0f;
                attenuation = 0.0f;
            }
        }
        else if (currentLight.Type == 2) // Spot Light (LT_SPOT = 2)
        {
            // 스포트라이트 계산 (현재는 포인트 라이트와 동일하게 처리, 추후 확장 가능)
            float3 dirToLight = currentLight.Position.xyz - In.vWorldPos.xyz;
            float distSq = dot(dirToLight, dirToLight);
            float dist = sqrt(distSq);

            if (dist < currentLight.Range)
            {
                lightVec = dirToLight / dist;
                NdotL = saturate(dot(normal, lightVec));
                attenuation = saturate(1.0f / (currentLight.Attenuation0 +
                                                currentLight.Attenuation1 * dist +
                                                currentLight.Attenuation2 * distSq));

                // TODO: 여기에 스포트라이트 원뿔각/감쇠 계산 추가
                // float spotFactor = ... 계산 ...
                // attenuation *= spotFactor;
            }
            else
            {
                NdotL = 0.0f;
                attenuation = 0.0f;
            }
        } // End of light type branching
        
        

        // 현재 라이트의 기여도가 유효하다면 (NdotL > 0, attenuation > 0)
        // (Type 0은 위에서 continue로 걸러졌으므로 여기서는 1, 2, 3 타입만 고려됨)
        
        if (NdotL > 0.0f && attenuation > 0.0f)
        {
            
            // 디퓨즈(Diffuse) 계산
            float3 diffuse = baseColor.rgb * g_Material.Diffuse.rgb * NdotL * currentLight.Color.rgb;

            // 스페큘러(Specular) 계산
            float3 halfwayDir = normalize(lightVec + viewDir);
            float NdotH = saturate(dot(normal, halfwayDir));
            float specPower = pow(NdotH, g_Material.Power);
            float3 specular = g_Material.Specular.rgb * specPower * currentLight.Color.rgb;

            // 현재 라이트의 최종 기여도 = (디퓨즈 + 스페큘러) * 감쇠
            float3 currentLightContribution = (diffuse + specular) * attenuation;

            // 최종 색상에 현재 라이트 기여도 누적
            accumulatedColor += currentLightContribution;
        }

    } // End of for loop*/


    // --- 최종 색상 조합 및 후처리 ---
    float4 litColor;
    litColor.rgb = saturate(accumulatedColor);
    litColor.a = baseColor.a * g_Material.Diffuse.a; // 알파 값

    // --- 안개 효과 계산 ---
    float distance = length(In.vViewPos);
    float fogFactor = saturate((distance - g_FogStart) / (g_FogEnd - g_FogStart));
    
    // === 새로운 Additive 방식 ===
    // 1. 안개 기여도 계산 (안개 색상 * 안개 인자)
    float3 fogContribution = g_FogColor * fogFactor;
    // 2. 원래 조명 색상에 안개 기여도를 더함 (saturate로 0~1 범위 유지)
    Out.vColor = lerp(litColor, float4(g_FogColor, litColor.a), fogFactor);
    //Out.vColor.rgb = saturate(litColor.rgb + fogContribution);
    // 3. 알파 값은 원래 알파 값 유지
    Out.vColor.a = litColor.a;

    // --- 알파 테스트 ---
    if (Out.vColor.a < 0.8f) discard;

    return Out;
}

// 새로운 'Unlit' 픽셀 셰이더
PS_OUT PS_WEAPON(PS_IN In, float facing : VFACE) // VFACE는 양면 렌더링 시 필요할 수 있음
{
    PS_OUT Out;

    // 1. 텍스처 색상 가져오기
    float2 tiledUV = In.vTexcoord * g_ScaleFactor + g_OffsetFactor;
    float4 baseColor = tex2D(DefaultSampler, tiledUV);

    // 2. (선택 사항) 재질의 Diffuse 색상을 곱하여 기본 색상 조절
    baseColor *= g_Material.Diffuse; // Material 구조체 사용 시
    Out.vColor = baseColor;

    // 4. 알파 값 처리 (텍스처 알파와 재질 알파 곱하기 등)
    // Out.vColor.a = baseColor.a * g_Material.Diffuse.a; // 만약 Diffuse를 곱했다면

    // 5. (선택 사항) 알파 값에 따른 discard 로직
    if (Out.vColor.a < 0.2f)
        discard;

    return Out;
}

// 새로운 'Unlit' 픽셀 셰이더
PS_OUT PS_UNLIT(PS_IN In, float facing : VFACE) // VFACE는 양면 렌더링 시 필요할 수 있음
{
    PS_OUT Out;

    // 1. 텍스처 색상 가져오기
    float2 tiledUV = In.vTexcoord * g_ScaleFactor + g_OffsetFactor;
    float4 baseColor = tex2D(DefaultSampler, tiledUV);

    // 2. (선택 사항) 재질의 Diffuse 색상을 곱하여 기본 색상 조절
    baseColor *= g_Material.Diffuse; // Material 구조체 사용 시

    // --- 안개 효과 계산 시작 ---
    float distance = length(In.vViewPos); // 보간된 뷰 공간 위치 사용
    float fogFactor = saturate((distance - g_FogStart) / (g_FogEnd - g_FogStart));

    // 3. 텍스처(또는 조절된) 색상과 안개 색상을 혼합
    // === 새로운 Additive 방식 ===
    // 1. 안개 기여도 계산 (안개 색상 * 안개 인자)
    float3 fogContribution = g_FogColor * fogFactor;
    // 2. 원래 조명 색상에 안개 기여도를 더함 (saturate로 0~1 범위 유지)
    Out.vColor.rgb = saturate(baseColor.rgb + fogContribution);
    // 3. 알파 값은 원래 알파 값 유지
    Out.vColor.a = baseColor.a;
//    Out.vColor = lerp(baseColor, float4(g_FogColor, baseColor.a), fogFactor);
    // --- 안개 효과 계산 끝 ---

    // 4. 알파 값 처리 (텍스처 알파와 재질 알파 곱하기 등)
    // Out.vColor.a = baseColor.a * g_Material.Diffuse.a; // 만약 Diffuse를 곱했다면

    // 5. (선택 사항) 알파 값에 따른 discard 로직
    if (Out.vColor.a < 0.8f)
         discard;

    return Out;
}

PS_OUT PS_TEST_LIGHTING(PS_IN In, float facing : VFACE)
{
    PS_OUT Out;

    // --- 기본 벡터 계산 ---
    float3 normal = normalize(In.vNormal * sign(facing));
    float3 viewDir = normalize(g_CameraPosition - In.vWorldPos.xyz);

    // --- 텍스처 샘플링 ---
    float4 baseColor = tex2D(DefaultSampler, In.vTexcoord); // 타일링 미적용

    // --- 최종 색상 계산을 위한 변수 초기화 ---
    float3 accumulatedColor = (baseColor.rgb * g_Material.Diffuse.rgb * g_AmbientLightColor.rgb) + g_Material.Emissive.rgb;

    Light currentLight = g_Lights[0];
    
    /*float3 lightVec = float3(0.0f, 0.0f, 0.0f);
    float attenuation = 0.0f;
    float NdotL = 0.0f;*/
    
            // --- 확인하고 싶은 값에 따라 아래 코드 중 하나만 주석 해제 ---

            // [디버깅 1] 라이트 타입(Type) 확인하기
            // Type 값에 따라 다른 색상 출력 (0:검정, 1:빨강, 2:초록, 3:파랑)
            
            /*if (currentLight.Type == 1) { Out.vColor = float4(1.f, 0.f, 0.f, 1.f); return Out; } // Point = Red
            else if (currentLight.Type == 2) { Out.vColor = float4(0.f, 1.f, 0.f, 1.f); return Out; } // Spot = Green
            else if (currentLight.Type == 3) { Out.vColor = float4(0.f, 0.f, 1.f, 1.f); return Out; } // Dir = Blue
            else { Out.vColor = float4(0.f, 0.f, 0.f, 1.f); return Out; } // Unused(0) or Unknown = Black*/
            

            // [디버깅 2] 라이트 색상(Color/fDiffuse) 확인하기
            // 라이트의 Color 값을 그대로 출력 (Alpha는 1로 고정)
            
            /*Out.vColor = float4(currentLight.Color.rgb, 1.0f);
            return Out; */

            // [디버깅 3] 라이트 위치(Position) 확인하기 (x,y,z -> R,G,B)
            // 위치 값은 범위가 크므로 적절히 스케일링하여 [0,1] 범위로 만들어 시각화
            // 예: 월드 좌표 기준 -50 ~ +50 범위를 [0,1]로 매핑 가정
            
            /*float3 posColor = saturate(abs(currentLight.Position) / 50.f); // 절대값을 50으로 나누어 [0,1]로 만듬
            Out.vColor = float4(posColor, 1.0f);
            return Out;*/
            

            // [디버깅 4] 라이트 방향(Direction) 확인하기 (x,y,z -> R,G,B)
            // 방향 벡터 값(-1 ~ +1)을 [0,1] 범위로 변환하여 시각화
            
            /*float3 dirForColor = (normalize(currentLight.Direction) + 1.0f) * 0.5f;
            Out.vColor = float4(dirForColor, 1.0f);
            return Out;*/
            

            // [디버깅 5] 라이트 범위(Range) 확인하기 (빨간색 채널 사용)
            // Range 값을 특정 최대값으로 나누어 [0,1] 범위로 만들어 시각화
            // 예: 최대 Range 50으로 가정
            
            
            /*float rangeColor = currentLight.Range / 50.f;  //saturate(currentLight.Range / 50.f);
            Out.vColor = float4(rangeColor, 0.f, 0.f, 1.f);
            return Out;*/
            

            // [디버깅 6] 라이트 감쇠(Attenuation1) 확인하기 (녹색 채널 사용)
            // Attenuation1 값을 특정 최대값으로 나누어 [0,1] 범위로 만들어 시각화
            // 예: 최대 Attenuation1 1.0으로 가정
            
            /* float atten1Color = saturate(currentLight.Attenuation1 / 1.0f);
            Out.vColor = float4(0.f, atten1Color, 0.f, 1.f);
            return Out; */
    

            // [디버깅 7] 실제 계산된 감쇠값(attenuation) 확인하기 (회색조)
            // 이 디버깅 코드를 사용하려면 아래의 실제 계산 로직 일부가 필요함
            
            /*float3 dirToLight = currentLight.Position.xyz - In.vWorldPos.xyz;
            float distSq = dot(dirToLight, dirToLight);
            float dist = sqrt(distSq);
            float attenuation = 0.0f;
            if(currentLight.Type == 1 || currentLight.Type == 2) { // 포인트 또는 스팟
                 if (dist < currentLight.Range) {
                     attenuation = saturate(1.0f / (currentLight.Attenuation0 + currentLight.Attenuation1 * dist + currentLight.Attenuation2 * distSq));
                 }
            } else if (currentLight.Type == 3) { // 방향성
                 attenuation = 1.0f;
            }
            Out.vColor = float4(attenuation, attenuation, attenuation, 1.0f);
            return Out;*/
            

            // [디버깅 8] 실제 계산된 NdotL 값 확인하기 (회색조)
            // 이 디버깅 코드를 사용하려면 아래의 실제 계산 로직 일부가 필요함
            
            float3 lightVec = float3(0,0,0);
            // ... (타입에 따라 lightVec 계산 로직 필요) ...
            if (currentLight.Type == 3) lightVec = normalize(-currentLight.Direction).xyz;
            else if (currentLight.Type == 1 || currentLight.Type == 2) {
                float3 dirToLight = currentLight.Position.xyz - In.vWorldPos.xyz;
                float dist = length(dirToLight);
                if(dist > 0.001f) lightVec = dirToLight / dist; // Normalize
            }
            float NdotL = saturate(dot(normal, lightVec));
            Out.vColor = float4(NdotL, NdotL, NdotL, 1.0f);
            return Out;

            // --- 디버깅 코드 끝 ---
        // ==============================================================
        //            ★★★ 디버깅 코드 삽입 끝 ★★★
        // ==============================================================


        // --- 원래의 라이트 계산 로직 ---


    // --- 최종 색상 조합 및 후처리 ---
    float4 litColor;
    litColor.rgb = saturate(accumulatedColor);
    litColor.a = baseColor.a; // 알파 값

    // --- 안개 효과 계산 ---
    float distance = length(In.vViewPos);
    float fogFactor = saturate((distance - g_FogStart) / (g_FogEnd - g_FogStart));
    Out.vColor = lerp(litColor, float4(g_FogColor, litColor.a), fogFactor);

    return Out;
}

/*PS_OUT PS_LIT_MULTIPASS(PS_IN In, float facing : VFACE)
{
    PS_OUT Out;

    // --- 기본 벡터 계산 ---
    float3 normal = normalize(In.vNormal * sign(facing));
    float3 viewDir = normalize(g_CameraPosition - In.vWorldPos.xyz);

    // --- 텍스처 샘플링 ---
    float2 tiledUV = In.vTexcoord * In.vScale; // 스케일 고려
    float4 baseColor = tex2D(DefaultSampler, tiledUV);

    // --- 최종 색상 계산을 위한 변수 초기화 ---
    float3 accumulatedColor = float3(0.0f, 0.0f, 0.0f); // 추가 패스를 위해 0으로 초기화

    // 첫 번째 패스(Base Pass)인 경우에만 Ambient와 Emissive를 계산
    if (g_CurrentPassIndex == 0)
    {
        // 참고: Ambient 계산 방식 확인 필요 (g_Material.Ambient와 g_Material.Diffuse 둘 다 곱하는 것이 일반적이지 않을 수 있음)
        // 일반적인 Ambient: baseColor.rgb * g_Material.Ambient.rgb * g_AmbientLightColor.rgb
        // 또는: baseColor.rgb * g_Material.Ambient.rgb (텍스처와 재질 주변광만 고려)
        // 현재 코드 유지:
        accumulatedColor = (baseColor.rgb * g_Material.Ambient.rgb * g_Material.Diffuse.rgb) + g_Material.Emissive.rgb;
    }

    // 이번 패스에서 처리할 라이트들에 대한 계산 및 누적
    // for 루프의 시작과 끝을 C++에서 전달된 인덱스와 개수로 조정
    int loopEnd = g_LightStartIndex + g_NumLightsInPass;
    for (int i = g_LightStartIndex; i < loopEnd; ++i)
    {
        // g_Lights 배열 범위 확인 (선택적이지만 안전함)
        if (i >= MAX_LIGHTS) // 또는 실제 C++에서 관리하는 전체 라이트 수
            break;

        Light currentLight = g_Lights[i];

        if (currentLight.Type == 0)
            continue; // LT_UNUSED

        // --- 라이트 타입별 계산 (기존 코드와 거의 동일) ---
        float3 lightVec = float3(0.0f, 0.0f, 0.0f);
        float attenuation = 0.0f;
        float NdotL = 0.0f;

        if (currentLight.Type == 3) // Directional
        {
            lightVec = normalize(-currentLight.Direction.xyz); // Direction은 xyz 사용
            NdotL = saturate(dot(normal, lightVec));
            attenuation = 1.0f;
        }
        else if (currentLight.Type == 1) // Point
        {
            float3 dirToLight = currentLight.Position.xyz - In.vWorldPos.xyz; // Position은 xyz 사용
            float distSq = dot(dirToLight, dirToLight);
            float dist = sqrt(distSq);

            if (dist < currentLight.Range)
            {
                lightVec = dirToLight / dist;
                NdotL = saturate(dot(normal, lightVec));
                attenuation = saturate(1.0f / (currentLight.Attenuation0 +
                                              currentLight.Attenuation1 * dist +
                                              currentLight.Attenuation2 * distSq));
            }
            // else: NdotL=0, attenuation=0 (초기값 유지)
        }
        else if (currentLight.Type == 2) // Spot
        {
            // 스포트라이트 계산 (현재는 포인트 라이트와 유사하게 처리됨, 필요시 확장)
            float3 dirToLight = currentLight.Position.xyz - In.vWorldPos.xyz; // Position은 xyz 사용
            float distSq = dot(dirToLight, dirToLight);
            float dist = sqrt(distSq);

            if (dist < currentLight.Range)
            {
                lightVec = dirToLight / dist;
                NdotL = saturate(dot(normal, lightVec));
                attenuation = saturate(1.0f / (currentLight.Attenuation0 +
                                              currentLight.Attenuation1 * dist +
                                              currentLight.Attenuation2 * distSq));

                // TODO: 여기에 스포트라이트 원뿔각/감쇠 계산 추가
                // float spotFactor = ... 계산 ...
                // attenuation *= spotFactor;
            }
             // else: NdotL=0, attenuation=0 (초기값 유지)
        }

        // 현재 라이트의 기여도가 유효하다면 누적
        if (NdotL > 0.0f && attenuation > 0.0f)
        {
            // 디퓨즈(Diffuse) 계산
            float3 diffuse = baseColor.rgb * g_Material.Diffuse.rgb * NdotL * currentLight.Color.rgb;

            // 스페큘러(Specular) 계산
            float3 halfwayDir = normalize(lightVec + viewDir);
            float NdotH = saturate(dot(normal, halfwayDir));
            float specPower = pow(NdotH, g_Material.Power);
            float3 specular = g_Material.Specular.rgb * specPower * currentLight.Color.rgb;

            // 현재 라이트의 최종 기여도 = (디퓨즈 + 스페큘러) * 감쇠
            float3 currentLightContribution = (diffuse + specular) * attenuation;

            // 최종 색상에 현재 라이트 기여도 누적
            accumulatedColor += currentLightContribution;
        }
    } // End of for loop

    // --- 최종 색상 조합 및 후처리 ---

    // 첫 번째 패스에서만 안개를 적용하거나, 모든 패스가 끝난 후 후처리로 안개를 적용하는 것이 좋음.
    // 여기서는 첫 번째 패스에서만 안개를 적용하는 예시
    float4 finalColor;
    finalColor.rgb = saturate(accumulatedColor);
    finalColor.a = baseColor.a * g_Material.Diffuse.a; // 알파 값

    if (g_CurrentPassIndex == 0)
    {
        // --- 안개 효과 계산 (첫 패스에만 적용) ---
        float distance = length(In.vViewPos);
        float fogFactor = saturate((distance - g_FogStart) / (g_FogEnd - g_FogStart));
        Out.vColor = lerp(finalColor, float4(g_FogColor, finalColor.a), fogFactor);
    }
    else
    {
        // 추가 패스에서는 계산된 조명 기여도만 출력 (안개 없음)
        Out.vColor = finalColor;
    }

    // --- 알파 테스트 ---
    // if (Out.vColor.a < 0.1f) discard; // 필요하다면 모든 패스에 적용

    return Out;
}*/

PS_OUT PS_MAIN_BLACK(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = vector(0.f, 0.f, 0.f, 1.f);
        
    return Out;
}

technique DefaultTechnique
{
    
    pass DefaultPass
    {
        CullMode = None;
        aLPHAbLENDeNABLE = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        BlendOp = Add;

        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_UNLIT();
    }

    pass PlanePass
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
        PixelShader = compile ps_3_0 PS_LIT(); // 빛 계산이 포함된 PS_MAIN 사용
    }

    pass Weapon
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
        PixelShader = compile ps_3_0 PS_WEAPON(); // 빛 계산이 포함된 PS_MAIN 사용
    }

    pass BLACK
    {
        VertexShader = compile vs_3_0 VS_MAIN();
        PixelShader = compile ps_3_0 PS_MAIN_BLACK();
    }

    pass TEST
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
        PixelShader = compile ps_3_0 PS_TEST_LIGHTING(); // 빛 계산이 포함된 PS_MAIN 사용
    }
}   