#include "JsonLoader.h"
#include <Player.h>
#include <Terrain.h>
#include <Structure.h>
#include <Camera_Free.h>
#include <Camera_FirstPerson.h>
#include "UI_Headers.h" 
#include "GamePlay_Button.h"
#include "Weapon_Headers.h"
#include "Anubis.h"
#include "Crocman.h"
#include "PointShop.h"
#include "Harpoon.h"
#include "Item.h"
#include "Harpoonguy.h"


HRESULT CJsonLoader::Load_Prototypes(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device, const _wstring& filePath)
{
	// JSON íŒŒì¼ ë¡œë“œ
	ifstream file(filePath);
	if (!file.is_open())
	{
		MSG_BOX("í”„ë¡œí† íƒ€ìž… JSON íŒŒì¼ì„ ì°¾ì„ ìˆ˜ ì—†ìŠµë‹ˆë‹¤.");
		return E_FAIL;
	}

	json j;
	file >> j;

	if (j.contains("textures"))
	{
		for (const auto& texture : j["textures"])
		{
			_wstring tag = ISerializable::Utf8ToWide(texture["tag"]);
			_wstring path = ISerializable::Utf8ToWide(texture["path"]);
			LEVEL level = static_cast<LEVEL>(texture["level"].get<_uint>());
			_uint count = texture["count"];
			if (FAILED(pGameInstance->Add_Prototype(level, tag,
				CTexture::Create(pGraphic_Device, CTexture::TYPE_2D, path.c_str(), count))))
				return E_FAIL;
		}
	}


	if (j.contains("gameObjects"))
	{
		for (const auto& obj : j["gameObjects"])
		{
			_wstring tag = ISerializable::Utf8ToWide(obj["tag"]);
			LEVEL level = static_cast<LEVEL>(obj["level"].get<_uint>());
			string className = obj["class"];
			CBase* pGameObject = Create_Object_ByClassName(className, pGraphic_Device);
			if (!pGameObject)
				continue;

			if (FAILED(pGameInstance->Add_Prototype(level, tag, pGameObject)))
				return E_FAIL;
		}
	}

	if (j.contains("buffers"))
	{
		for (const auto& buffer : j["buffers"])
		{
			_wstring tag = ISerializable::Utf8ToWide(buffer["tag"]);
			LEVEL level = static_cast<LEVEL>(buffer["level"].get<_uint>());
			string className = buffer["class"];

			CBase* pBuffer = nullptr;

			if (className == "CVIBuffer_Terrain")
			{
				_uint width = buffer["width"].get<_uint>();
				_uint height = buffer["height"].get<_uint>();
				pBuffer = CVIBuffer_Terrain::Create(pGraphic_Device, width, height);
			}
			else if (className == "CItem")
			{

			}


			if (!pBuffer)
				continue;

			if (FAILED(pGameInstance->Add_Prototype(level, tag, pBuffer)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CJsonLoader::Load_Level(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device, const _wstring& jsonFilePath, LEVEL eLevel)
{
	ifstream file(jsonFilePath);
	if (!file.is_open())
	{
		MSG_BOX("Json 파일이 존재하지 않습니다.");
		return E_FAIL;
	}

	json j;
	file >> j;

	for (const auto& item : j["layers"].items())
	{
		const auto& layerName = item.key(); //first;    // 키 (문자열)
		const auto& layerObjects = item.value(); // 값 (JSON 객체)
		_wstring wLayerName = ISerializable::Utf8ToWide(layerName);

		for (const auto& objData : layerObjects)
		{
			// OBJECT_DESC 구조체 생성
			CGameObject::OBJECT_DESC tObjDesc{};

			// JSON에서 OBJECT_DESC 정보 추출
			if (objData.contains("OBJECT_DESC"))
			{
				const json& objectDesc = objData["OBJECT_DESC"];
				if (objectDesc.contains("ProtTag"))
					tObjDesc.stProtTag = ISerializable::Utf8ToWide(objectDesc["ProtTag"].get<string>());
				if (objectDesc.contains("ProtTextureTag"))
					tObjDesc.stProtTextureTag = ISerializable::Utf8ToWide(objectDesc["ProtTextureTag"].get<string>());
				if (objectDesc.contains("BufferTag"))
					tObjDesc.stBufferTag = ISerializable::Utf8ToWide(objectDesc["BufferTag"].get<string>());
				if (objectDesc.contains("Level"))
					tObjDesc.iLevel = objectDesc["Level"];
				if (objectDesc.contains("ProtoLevel"))
					tObjDesc.iProtoLevel = objectDesc["ProtoLevel"];
			}

			// 레이어 태그 가져오기
			_wstring layerTag;

			if (objData.contains("LayerTag"))
				layerTag = ISerializable::Utf8ToWide(objData["LayerTag"].get<string>());
			else
				layerTag = wLayerName;

			bool bIsItem = layerTag == L"Layer_Item";

			// FromPool 확인
			bool bFromPool = false;
			if (objData.contains("FromPool"))
				bFromPool = objData["FromPool"];

			// 필요한 프로토타입이 존재하는지 확인
			if (FAILED(pGameInstance->Find_Prototype(tObjDesc.stProtTag)))
			{
				continue;
			}
			else if (tObjDesc.stProtTag.empty())
			{
				continue;
			}

			// 객체 생성 또는 활성화 (OBJECT_DESC 전달)
			if (bFromPool)
			{
				if (!pGameInstance->Acquire_Object(tObjDesc.iProtoLevel,
					layerTag, &tObjDesc))
					continue;
			}
			else
			{
				if (bIsItem)
				{
					CItem::ITEM_DESC tItemDesc;
		
					tItemDesc.stProtTag = tObjDesc.stProtTag;
					tItemDesc.stProtTextureTag = tObjDesc.stProtTextureTag;
					tItemDesc.stBufferTag = tObjDesc.stBufferTag;
					tItemDesc.iLevel = tObjDesc.iLevel;
					tItemDesc.iProtoLevel = tObjDesc.iProtoLevel;
					if (objData.contains("Item_Name"))
					{
						tItemDesc.strItemName = ISerializable::Utf8ToWide(objData["Item_Name"].get<string>());
					}
					if (objData.contains("Type"))
					{
						tItemDesc.eType = static_cast<CItem::ITEM_TYPE>(objData["Type"].get<_int>());
					}
					if (FAILED(pGameInstance->Add_GameObject(tObjDesc.iProtoLevel, tObjDesc.stProtTag,
						tObjDesc.iLevel, layerTag, &tItemDesc)))
						continue;
				}
				else
				{
					if (FAILED(pGameInstance->Add_GameObject(tObjDesc.iProtoLevel, tObjDesc.stProtTag,
						tObjDesc.iLevel, layerTag, &tObjDesc)))
						continue;
				}

				// 생성된 객체를 찾아 나머지 데이터 역직렬화
				CGameObject* pGameObject = pGameInstance->Find_Last_Object(tObjDesc.iLevel, layerTag);
				if (pGameObject)
					pGameObject->Deserialize(objData);
			}
		}
	}

		return S_OK;
}

	CBase* CJsonLoader::Create_Object_ByClassName(const string & className, LPDIRECT3DDEVICE9 pGraphic_Device)
	{
		// ê¸°ë³¸ ê²Œìž„ ì˜¤ë¸Œì íŠ¸
		if (className == "CPlayer")
			return CPlayer::Create(pGraphic_Device);
		else if (className == "CTerrain")
			return CTerrain::Create(pGraphic_Device);
		else if (className == "CStructure")
			return CStructure::Create(pGraphic_Device);
		// ì¹´ë©”ë¼
		else if (className == "CCamera_Free")
			return CCamera_Free::Create(pGraphic_Device);
		else if (className == "CCamera_FirstPerson")
			return CCamera_FirstPerson::Create(pGraphic_Device);
		// UI ì»´í¬ë„ŒíŠ¸
		else if (className == "CUI_Default_Panel")
			return CUI_Default_Panel::Create(pGraphic_Device);
		else if (className == "CUI_Left_Display")
			return CUI_Left_Display::Create(pGraphic_Device);
		else if (className == "CUI_Player_Icon")
			return CUI_Player_Icon::Create(pGraphic_Device);
		else if (className == "CUI_HP_Bar")
			return CUI_HP_Bar::Create(pGraphic_Device);
		else if (className == "CUI_MP_Bar")
			return CUI_MP_Bar::Create(pGraphic_Device);
		else if (className == "CUI_Mid_Display")
			return CUI_Mid_Display::Create(pGraphic_Device);
		else if (className == "CUI_Right_Display")
			return CUI_Right_Display::Create(pGraphic_Device);
		else if (className == "CUI_Bullet_Bar")
			return CUI_Bullet_Bar::Create(pGraphic_Device);
		else if (className == "CUI_Menu")
			return CUI_Menu::Create(pGraphic_Device);
		else if (className == "CGamePlay_Button")
			return CGamePlay_Button::Create(pGraphic_Device);
		else if (className == "CUI_Spell_Shop")
			return CUI_Spell_Shop::Create(pGraphic_Device);
		else if (className == "CUI_Spell_Shop")
			return CUI_Spell_Shop::Create(pGraphic_Device);
		else if (className == "CUI_Spell_Shop")
			return CUI_Spell_Shop::Create(pGraphic_Device);
		else if (className == "CAxe")
			return CAxe::Create(pGraphic_Device);
		else if (className == "CClaymore")
			return CClaymore::Create(pGraphic_Device);
		else if (className == "CMagnum")
			return CMagnum::Create(pGraphic_Device);
		else if (className == "CStaff")
			return CStaff::Create(pGraphic_Device);
		else if (className == "CShotGun")
			return CShotGun::Create(pGraphic_Device);
		else if (className == "CAnubis")
			return CAnubis::Create(pGraphic_Device);
		else if (className == "CCrocman")
			return CCrocman::Create(pGraphic_Device);
		else if (className == "CHarpoonguy")
			return CHarpoonguy::Create(pGraphic_Device);
		else if (className == "CHarpoon")
			return CHarpoon::Create(pGraphic_Device);
		else if (className == "CItem")
			return CItem::Create(pGraphic_Device);


		wstring wClassName = ISerializable::Utf8ToWide(className);
		wstring errorMsg = L"ì•Œ ìˆ˜ ì—†ëŠ” í´ëž˜ìŠ¤ ì´ë¦„: " + wClassName;
		OutputDebugString(errorMsg.c_str());

		return nullptr;
	}

	HRESULT CJsonLoader::LoadClassNamesFromJson(const string & filePath, vector<string>&outClassNames)
	{
		try
		{

			ifstream file(filePath);
			if (!file.is_open())
			{
				MSG_BOX("클래스 이름 JSON 파일을 찾을 수 없습니다.");
				return E_FAIL;
			}

			json jsonData;
			file >> jsonData;
			file.close();

			outClassNames.clear();

			if (jsonData.contains("classes") && jsonData["classes"].is_array())
			{
				for (const auto& className : jsonData["classes"])
				{
					if (className.is_string())
					{
						outClassNames.push_back(className.get<string>());
					}
				}

				return S_OK;
			}
			else
			{
				MSG_BOX("JSON 파일에 'classes' 배열이 없거나 형식이 잘못되었습니다.");
				return E_FAIL;
			}
		}
		catch (const json::exception&)
		{
			MSG_BOX("JSON 파싱 오류가 발생했습니다.");
			return E_FAIL;
		}
		catch (const exception&)
		{

			MSG_BOX("예외가 발생했습니다.");
			return E_FAIL;
		}
	}

	_wstring CJsonLoader::Get_Prototype_For_Layer(const _wstring & layerName)
	{
		if (layerName == L"Layer_Player")
			return L"Prototype_GameObject_Player";
		else if (layerName == L"Layer_Monster_Crocman")
			return L"Prototype_GameObject_Crocman";
		else if (layerName == L"Layer_Monster_Harpoonguy")
			return L"Prototype_GameObject_Harpoonguy";
		else if (layerName == L"Layer_Monster_Projectile_Harpoon")
			return L"Prototype_GameObject_Harpoon";
		else if (layerName == L"Layer_BackGround")
			return L"Prototype_GameObject_Terrain";
		else if (layerName == L"Layer_Camera")
			return L"Prototype_GameObject_Camera_FirstPerson";
		else if (layerName == L"Layer_Default_PlayerUI")
			return L"Prototype_GameObject_Default_PlayerUI";

		return L"";
	}
