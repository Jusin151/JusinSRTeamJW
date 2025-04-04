#pragma once

#define BEGIN(NAMESPACE) namespace NAMESPACE { 
#define END }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define MSG_BOX(message)								\
::MessageBox(nullptr, TEXT(message), L"error", MB_OK)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned int Destroy_Instance();				\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned int ClassName::Destroy_Instance()				\
{														\
	unsigned int iRefCnt = {};							\
	if(nullptr != m_pInstance)							\
	{													\
		iRefCnt = m_pInstance->Release();				\
		if(0 == iRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return iRefCnt;										\
}

#define GRAVITY -9.8f


#pragma region FMOD
#define BANKPATH "../../Resources/Sounds/Banks/Desktop/"
#pragma endregion

#define INIT_PARENT(pArg) \
    CGameObject::OBJECT_DESC* pDesc = static_cast<CGameObject::OBJECT_DESC*>(pArg); \
    if (FAILED(CGameObject::Initialize(pDesc))) { \
        return E_FAIL; \
    }


#define SET_TRANSFORM(jsonObj, transformCom) \
do { \
    if ((jsonObj).contains("position") && (jsonObj)["position"].is_array()) { \
        auto& _pos = (jsonObj)["position"]; \
        if (_pos.size() >= 3 && _pos[0].is_number() && _pos[1].is_number() && _pos[2].is_number()) { \
            (transformCom)->Set_State(CTransform::STATE_POSITION, _float3(_pos[0], _pos[1], _pos[2])); \
        } \
    } \
    if ((jsonObj).contains("rotation") && (jsonObj)["rotation"].is_array()) { \
        auto& _angle = (jsonObj)["rotation"]; \
        if (_angle.size() >= 3 && _angle[0].is_number() && _angle[1].is_number() && _angle[2].is_number()) { \
            (transformCom)->Rotate_EulerAngles(_float3(_angle[0], _angle[1], _angle[2])); \
        } \
    } \
    if ((jsonObj).contains("scale") && (jsonObj)["scale"].is_array()) { \
        auto& _scale = (jsonObj)["scale"]; \
        if (_scale.size() >= 3 && _scale[0].is_number() && _scale[1].is_number() && _scale[2].is_number()) { \
            float scaleX = std::max((float)_scale[0], 0.1f); \
            float scaleY = std::max((float)_scale[1], 0.1f); \
            float scaleZ = std::max((float)_scale[2], 0.1f); \
            (transformCom)->Set_Scale(scaleX, scaleY, scaleZ); \
        } \
    } \
} while(0)

#define MAX_LIGHT_APP = 8;