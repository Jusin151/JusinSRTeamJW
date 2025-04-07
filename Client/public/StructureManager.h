#pragma once
#include <Base.h>
#include <Client_Defines.h>
#include <Structure.h>
#include <Door.h>
BEGIN(Client)
class CStructureManager final : public CBase
{
	DECLARE_SINGLETON(CStructureManager)

private:
	CStructureManager();
	virtual ~CStructureManager() = default;

public:
	void Add_Structure(CStructure* pStrucutre);
	void Add_Door(CDoor* pDoor);

	const auto& Get_Structures() const { 
		return m_StructureList; }
	const auto& Get_Doors() const { return m_DoorList; }
	void Clear();

private:
	list<CDoor*> m_DoorList;
	list<CStructure*> m_StructureList;

public:
	virtual void Free() override;

};
END
