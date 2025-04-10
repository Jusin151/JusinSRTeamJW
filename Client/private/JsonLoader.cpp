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
#include "Hub_PointShop.h"
#include "Harpoon.h"
#include "Item.h"
#include "Trigger.h"
#include "Harpoonguy.h"
#include "Yeti.h"
#include "Snowspider.h"
#include "Glacier.h"
#include "GlacierBullet.h"
#include "Door.h"
#include "Effects.h"
#include "Cthulhu.h"
#include "GameObject_Snow.h"
#include "Prefabs.h"

#include "UI_Headers.h" // UI 헤더들 
#include "Hub_Headers.h" // 허브 헤더들
#include <Weapon_Base.h>
#include "Weapon_Effect.h"
#include "Staff_Bullet.h"
#include "Image.h"
#include "Inven_UI.h"
#include "Level_Hub.h"
#include "Harvester.h"



HRESULT CJsonLoader::Load_Prototypes(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device, const _wstring& filePath)
{

	ifstream file(filePath);
	if (!file.is_open())
	{
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
			auto pTexture = CTexture::Create(pGraphic_Device, CTexture::TYPE_2D, path.c_str(), count);
			if (FAILED(pGameInstance->Add_Prototype(level, tag,
				pTexture)))
			{
 				Safe_Release(pTexture);
				continue;
			}
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
			{
				Safe_Release(pGameObject);
				continue;
			}
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


			if (!pBuffer)
				continue;

			if (FAILED(pGameInstance->Add_Prototype(level, tag, pBuffer)))
			{
				Safe_Release(pBuffer);
				continue;
			}
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
		const auto& layerName = item.key();
		const auto& layerObjects = item.value();
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

			// 객체 타입 확인
			bool bIsItem = layerTag == L"Layer_Item";
			bool bIsDoor = layerTag.find(L"Layer_Door")!=_wstring::npos;
			bool bIsTrigger = layerTag.find(L"Trigger") != _wstring::npos;

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

			// 객체 생성 또는 활성화 (타입별 DESC 구조체 전달)
			if (bFromPool)
			{
				if (!pGameInstance->Acquire_Object(tObjDesc.iProtoLevel,
					layerTag, &tObjDesc))
					continue;
			}
			else if (bIsItem)
			{
				CItem::ITEM_DESC tItemDesc;

				// 기본 OBJECT_DESC 필드 복사
				tItemDesc.stProtTag = tObjDesc.stProtTag;
				tItemDesc.stProtTextureTag = tObjDesc.stProtTextureTag;
				tItemDesc.stBufferTag = tObjDesc.stBufferTag;
				tItemDesc.iLevel = tObjDesc.iLevel;
				tItemDesc.iProtoLevel = tObjDesc.iProtoLevel;

				// Item 특정 필드 복사
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
			else if (bIsDoor)
			{
				CDoor::DOOR_DESC tDoorDesc{};

				// 기본 OBJECT_DESC 필드 복사
				tDoorDesc.stProtTag = tObjDesc.stProtTag;
				tDoorDesc.stProtTextureTag = tObjDesc.stProtTextureTag;
				tDoorDesc.stBufferTag = tObjDesc.stBufferTag;
				tDoorDesc.iLevel = tObjDesc.iLevel;
				tDoorDesc.iProtoLevel = tObjDesc.iProtoLevel;

				// Door 특정 필드들을 JSON에서 가져오기
				if (objData.contains("door_type"))
					tDoorDesc.eType = static_cast<CDoor::DOOR_TYPE>(objData["door_type"].get<_int>());

				if (objData.contains("door_color"))
					tDoorDesc.eColor = static_cast<CDoor::DOOR_COLOR>(objData["door_color"].get<_int>());

				if (objData.contains("slide_distance"))
					tDoorDesc.fSlideDistance = objData["slide_distance"].get<_float>();

				if (objData.contains("key_item_tag"))
					tDoorDesc.stKeyItemTag = ISerializable::Utf8ToWide(objData["key_item_tag"].get<string>());

				if (objData.contains("is_active"))
					tDoorDesc.bStartsActive = objData["is_active"].get<bool>();

				// Door 객체 생성
				if (FAILED(pGameInstance->Add_GameObject(tObjDesc.iProtoLevel, tObjDesc.stProtTag,
					tObjDesc.iLevel, layerTag, &tDoorDesc)))
					continue;
			}
			else if (bIsTrigger)
			{
				CTrigger::TRIGGER_DESC tTriggerDesc{};

				// 기본 OBJECT_DESC 필드 복사
				tTriggerDesc.stProtTag = tObjDesc.stProtTag;
				tTriggerDesc.stProtTextureTag = tObjDesc.stProtTextureTag;
				tTriggerDesc.stBufferTag = tObjDesc.stBufferTag;
				tTriggerDesc.iLevel = tObjDesc.iLevel;
				tTriggerDesc.iProtoLevel = tObjDesc.iProtoLevel;

				// Trigger 특정 필드들을 JSON에서 가져오기
				if (objData.contains("trigger_type"))
					tTriggerDesc.eType = static_cast<CTrigger::TRIGGER_TYPE>(objData["trigger_type"].get<_int>());

				if (objData.contains("target_tags"))
					tTriggerDesc.stTargetTag = ISerializable::Utf8ToWide(objData["target_tags"].get<string>());

				if (objData.contains("is_active"))
					tTriggerDesc.bStartsActive = objData["is_active"].get<bool>();

				// Trigger 객체 생성
				if (FAILED(pGameInstance->Add_GameObject(tObjDesc.iProtoLevel, tObjDesc.stProtTag,
					tObjDesc.iLevel, layerTag, &tTriggerDesc)))
					continue;
			}
			else
			{
				// 일반 게임 오브젝트 생성
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

	return S_OK;
}

CBase* CJsonLoader::Create_Object_ByClassName(const string & className, LPDIRECT3DDEVICE9 pGraphic_Device)
{
#pragma region Player
	if (className == "CPlayer")
		return CPlayer::Create(pGraphic_Device);
	else if (className == "CInventory")
		return CInventory::Create(pGraphic_Device);


#pragma endregion

#pragma region Camera
	else if (className == "CCamera_Free")
		return CCamera_Free::Create(pGraphic_Device);
	else if (className == "CCamera_FirstPerson")
		return CCamera_FirstPerson::Create(pGraphic_Device);
#pragma endregion

	else if (className == "CTerrain")
		return CTerrain::Create(pGraphic_Device);
	else if (className == "CStructure")
		return CStructure::Create(pGraphic_Device);

#pragma region UI
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
	else if (className == "CUI_Exp_Bar")
		return CUI_Exp_Bar::Create(pGraphic_Device);
	else if (className == "CUI_Event")
		return CUI_Event::Create(pGraphic_Device);
	else if (className == "CGamePlay_Button")
		return CGamePlay_Button::Create(pGraphic_Device);
	else if (className == "CUI_Spell_Shop")
		return CUI_Spell_Shop::Create(pGraphic_Device);
	else if (className == "CImage")
		return CImage::Create(pGraphic_Device);
	else if (className == "CInven_UI")
		return CInven_UI::Create(pGraphic_Device);
#pragma endregion

#pragma region Weapons
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
	else if (className == "CMinigun")
		return CMinigun::Create(pGraphic_Device);
	else if (className == "CHarvester")
		return CHarvester::Create(pGraphic_Device);
	else if (className == "CSonic")
		return CSonic::Create(pGraphic_Device);
	else if (className == "CStaff_Bullet")
		return CStaff_Bullet::Create(pGraphic_Device);
#pragma endregion

#pragma region Monsters
	else if (className == "CAnubis")
		return CAnubis::Create(pGraphic_Device);
	else if (className == "CCrocman")
		return CCrocman::Create(pGraphic_Device);
	else if (className == "CHarpoonguy")
		return CHarpoonguy::Create(pGraphic_Device);
	else if (className == "CYeti")
		return CYeti::Create(pGraphic_Device);
	else if (className == "CSnowspider")
		return CSnowspider::Create(pGraphic_Device);
	else if (className == "CGlacier")
		return CGlacier::Create(pGraphic_Device);
	else if (className == "CHarpoon")
	return CHarpoon::Create(pGraphic_Device);
	else if (className == "CGlacierBullet")
		return CGlacierBullet::Create(pGraphic_Device);
	else if(className =="CCthulhu")
		return CCthulhu::Create(pGraphic_Device);
#pragma endregion

	else if (className == "CItem")
		return CItem::Create(pGraphic_Device);
	else if (className == "CDoor")
		return CDoor::Create(pGraphic_Device);
	else if (className == "CTrigger")
		return CTrigger::Create(pGraphic_Device);

#pragma region Effect
	else if (className == "CHit_Effect")
		return CHit_Effect::Create(pGraphic_Device);
	else if (className == "CGib_Effect")
		return CGib_Effect::Create(pGraphic_Device);
	else if (className == "CBlood_Effect")
		return CBlood_Effect::Create(pGraphic_Device);
	else if (className == "CWeapon_Effect")
		return CWeapon_Effect::Create(pGraphic_Device);
	else if (className == "CStains_Effect")
		return CStains_Effect::Create(pGraphic_Device);
#pragma endregion

#pragma region Particle
	else if (className == "CGameObject_Snow")
		return CGameObject_Snow::Create(pGraphic_Device);
#pragma endregion

#pragma region LightPrefabs
	else if (className == "CCandles")
		return CCandles::Create(pGraphic_Device);
	else if (className == "CGoblet")
		return CGoblet::Create(pGraphic_Device);
	else if (className == "CHub_Light")
		return CHub_Light::Create(pGraphic_Device);
	else if (className == "CLamp")
		return CLamp::Create(pGraphic_Device);
	else if (className == "CLong_Torch")
		return CLong_Torch::Create(pGraphic_Device);
	else if (className == "CTorch")
		return CTorch::Create(pGraphic_Device);
#pragma endregion


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

_int CJsonLoader::CountPrototypes(const _wstring& filePath) const
{
	ifstream ifs(filePath);
	if (!ifs.is_open()) return 0;

	nlohmann::json j;
	try { ifs >> j; }
	catch (...) { return 0; }

	int count = 0;
	if (j.contains("textures") && j["textures"].is_array())
		count += static_cast<int>(j["textures"].size());
	if (j.contains("gameObjects") && j["gameObjects"].is_array())
		count += static_cast<int>(j["gameObjects"].size());
	if (j.contains("buffers") && j["buffers"].is_array())
		count += static_cast<int>(j["buffers"].size());

	return count;
}

HRESULT CJsonLoader::Load_Prototypes(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device, const _wstring& filePath, function<void()> onEntryLoaded)
{
	ifstream file(filePath);
	if (!file.is_open())
	{
		return E_FAIL;
	}

	json j;
	file >> j;

	if (j.contains("textures"))
	{
		for (const auto& texture : j["textures"])
		{
			onEntryLoaded();
			_wstring tag = ISerializable::Utf8ToWide(texture["tag"]);
			_wstring path = ISerializable::Utf8ToWide(texture["path"]);
			LEVEL level = static_cast<LEVEL>(texture["level"].get<_uint>());
			_uint count = texture["count"];
			auto pTexture = CTexture::Create(pGraphic_Device, CTexture::TYPE_2D, path.c_str(), count);
			if (FAILED(pGameInstance->Add_Prototype(level, tag,
				pTexture)))
			{
				Safe_Release(pTexture);
				continue;
			}
		
		}
	}


	if (j.contains("gameObjects"))
	{
		for (const auto& obj : j["gameObjects"])
		{
			onEntryLoaded();
			_wstring tag = ISerializable::Utf8ToWide(obj["tag"]);
			LEVEL level = static_cast<LEVEL>(obj["level"].get<_uint>());
			string className = obj["class"];
			CBase* pGameObject = Create_Object_ByClassName(className, pGraphic_Device);
			if (!pGameObject)
				continue;

			if (FAILED(pGameInstance->Add_Prototype(level, tag, pGameObject)))
			{
				Safe_Release(pGameObject);
				continue;
			}
		
		}
	}

	if (j.contains("buffers"))
	{
		for (const auto& buffer : j["buffers"])
		{
			onEntryLoaded();
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


			if (!pBuffer)
				continue;

			if (FAILED(pGameInstance->Add_Prototype(level, tag, pBuffer)))
			{
				Safe_Release(pBuffer);
				continue;
			}
		
		}
	}

	return S_OK;
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
