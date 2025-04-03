#include <StructureManager.h>

IMPLEMENT_SINGLETON(CStructureManager);

CStructureManager::CStructureManager()
{
}

void CStructureManager::Add_Structure(CStructure* pStrucutre)
{
	if (!pStrucutre) return;
	m_StructureList.push_back(pStrucutre);
}

void CStructureManager::Add_Door(CDoor* pDoor)
{
	if (!pDoor) return;
	m_DoorList.push_back(pDoor);
}

void CStructureManager::Free()
{
	__super::Free();
	m_StructureList.clear();
	m_DoorList.clear();
}
