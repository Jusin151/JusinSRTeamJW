#include "UI_Event.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Event::CUI_Event(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Event::CUI_Event(const CUI_Event& Prototype)
	: CUI_Base(Prototype){}
HRESULT CUI_Event::Initialize_Prototype(){	return S_OK;}
HRESULT CUI_Event::Initialize(void* pArg)
{	

	CUI_Manager::GetInstance()->AddUI(L"UI_Event", this);

	return S_OK;
}
void CUI_Event::Priority_Update(_float fTimeDelta){}

void CUI_Event::Update(_float fTimeDelta)
{
	// 무지개 시간 
	m_fRainbowTimer += fTimeDelta;

	for (auto& it : m_vecEventTexts)
		it.ftime -= fTimeDelta;

	for (auto& it : m_vecEventRenderTexts)
	{
		it.fLifeTime -= fTimeDelta;
	}

	m_vecEventTexts.erase(remove_if(m_vecEventTexts.begin(), m_vecEventTexts.end(),
		[](const EventText& e) { return e.ftime <= 0.f; }),
		m_vecEventTexts.end());

	m_vecEventRenderTexts.erase(remove_if(m_vecEventRenderTexts.begin(), m_vecEventRenderTexts.end(),
		[](const EVENT_RENDER_TEXT& e) { return e.fLifeTime <= 0.f; }),
		m_vecEventRenderTexts.end());
}


void CUI_Event::Late_Update(_float fTimeDelta)
{	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))return;}

HRESULT CUI_Event::Render()
{
	_float2 basePos = _float2(-635.f, 150.f);
	_float lineSpacing = 30.f;

	for (size_t i = 0; i < m_vecEventTexts.size(); ++i)
	{
		const auto& evt = m_vecEventTexts[i];


		if (evt.wstr == L"레벨업!")
		{
			_float2 centerPos = _float2(-100.f, -300.f);
			_float2 centerSize = _float2(20.f, 40.f);

			float hue = fmodf(m_fRainbowTimer * 1000.f, 360.f);
			_float3 levelColor = HSVtoRGB(hue, 1.f, 1.f);

			m_pGameInstance->Render_Font_Size(L"EventFont", evt.wstr, centerPos, centerSize, levelColor);
		}

		else if (evt.wstr == L"마나가 부족합니다..!")
		{
			_float2 centerPos = _float2(-130.f, 0.f);
			_float2 centerSize = _float2(20.f, 40.f);
			_float3 levelColor = _float3(255.f, 0.f, 0.f);

			m_pGameInstance->Render_Font_Size(L"EventFont", evt.wstr, centerPos, centerSize, levelColor);
		}
		else if (evt.wstr == L"악마가 위기감을 느끼기 시작했습니다..!")
		{
			_float2 centerPos = _float2(-330.f, -300.f);
			_float2 centerSize = _float2(20.f, 40.f);
			_float3 levelColor = _float3(255.f, 0.f, 0.f);

			m_pGameInstance->Render_Font_Size(L"EventFont", evt.wstr, centerPos, centerSize, levelColor);
		}
		else
		{
	
			_float2 pos = basePos + _float2(0.f, -(_float)i * lineSpacing);
			_float2 size = _float2(8.f, 20.f);
			_float3 color = _float3(1.f, 1.f, 0.f);
			m_pGameInstance->Render_Font_Size(L"EventFont", evt.wstr, pos, size, color);
		}
	}

	for (const auto& renderText : m_vecEventRenderTexts)
	{
		m_pGameInstance->Render_Font_Size(TEXT("EventFont"), renderText.stText, renderText.vPos, renderText.vFontSize, renderText.vColor);
	}

	return S_OK;
}

_float3 CUI_Event::HSVtoRGB(float h, float s, float v)
{
	float c = v * s;
	float x = c * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
	float m = v - c;

	float r = 0, g = 0, b = 0;

	if (h < 60) { r = c; g = x; b = 0; }
	else if (h < 120) { r = x; g = c; b = 0; }
	else if (h < 180) { r = 0; g = c; b = x; }
	else if (h < 240) { r = 0; g = x; b = c; }
	else if (h < 300) { r = x; g = 0; b = c; }
	else { r = c; g = 0; b = x; }

	return _float3(r + m, g + m, b + m);
}

void CUI_Event::Add_EventRender(const EVENT_RENDER_TEXT& vEventRenderText)
{
	m_vecEventRenderTexts.push_back(vEventRenderText);
}




CUI_Event* CUI_Event::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Event* pInstance = new CUI_Event(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("이벤트 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Event::Clone(void* pArg)
{
	CUI_Event* pInstace = new CUI_Event(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("이벤트 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Event::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

void CUI_Event::OnNotify(void* pArg, const wstring& type)
{
}
