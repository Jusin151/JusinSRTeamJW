#include "PlayerCollider.h"

CPlayerCollider::CPlayerCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollider(pGraphic_Device)
{
}

int CPlayerCollider::On_Collision()
{
	return 0;
}

CPlayerCollider* CPlayerCollider::Create()
{
	return nullptr;
}

CComponent* CPlayerCollider::Clone(void* pArg)
{
	return nullptr;
}

void CPlayerCollider::Free()
{
	__super::Free();
}
