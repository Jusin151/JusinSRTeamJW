#include "Bounding_Box.h"

CBounding_Box::CBounding_Box()
{
	m_vMin = { INFINITY, INFINITY, INFINITY };
	m_vMax = { -INFINITY, -INFINITY, -INFINITY };
}

bool CBounding_Box::Is_Point_Inside(D3DXVECTOR3& p)
{
	if (
		p.x <= m_vMin.x &&
		p.y <= m_vMin.y &&
		p.z <= m_vMin.z &&
		p.x >= m_vMax.x &&
		p.y >= m_vMax.y &&
		p.z >= m_vMax.z)
		return true;
	return false;
}

void CBounding_Box::Free()
{
	__super::Free();
}
