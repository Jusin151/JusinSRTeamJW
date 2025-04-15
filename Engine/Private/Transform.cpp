#include "Transform.h"

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent(Prototype)
	, m_WorldMatrix{ Prototype.m_WorldMatrix }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_WorldMatrix);

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{

	if (nullptr != pArg)
	{
		TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);
		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;
		Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	}

	return S_OK;
}

void CTransform::ImGuiDisplay()
{
	_float3		vPosition = Get_State(STATE_POSITION);
	static float x = vPosition.x;
	static float y = vPosition.y;
	static float z = vPosition.z;
	ImGui::DragFloat("X", &x, -100000.0f, 100000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::DragFloat("Y", &y, -100000.0f, 100000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::DragFloat("Z", &z, -100000.0f, 100000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

	_float3 newPosition = { x, y, z };
	Set_State(STATE_POSITION, newPosition);
}

HRESULT CTransform::Bind_Resource()
{
	return m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
}

_float3 CTransform::Compute_Scaled() const
{
	_float3		vRight = Get_State(STATE_RIGHT);
	_float3		vUp = Get_State(STATE_UP);
	_float3		vLook = Get_State(STATE_LOOK);

	return _float3(D3DXVec3Length(&vRight),
		D3DXVec3Length(&vUp),
		D3DXVec3Length(&vLook));
}

void CTransform::Set_Scale(_float fX, _float fY, _float fZ)
{
	_float3		vRight = Get_State(STATE_RIGHT).GetNormalized();
	_float3		vUp = Get_State(STATE_UP).GetNormalized();
	_float3		vLook = Get_State(STATE_LOOK).GetNormalized();

	Set_State(STATE_RIGHT,vRight * fX);
	Set_State(STATE_UP, vUp * fY);
	Set_State(STATE_LOOK, vLook * fZ);
}

void CTransform::Go(_float3 dir, _float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	vPosition += dir.GetNormalized() * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}


void CTransform::Go_Straight(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);
	vPosition += vLook.GetNormalized() * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vLook = Get_State(STATE_LOOK);

	vPosition -= vLook.GetNormalized() * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);
	vPosition -= vRight.GetNormalized() * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vRight = Get_State(STATE_RIGHT);

	vPosition += vRight.GetNormalized() * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vUp = Get_State(STATE_UP);
	vPosition += vUp.GetNormalized() * m_fSpeedPerSec * fTimeDelta;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_float3		vPosition = Get_State(STATE_POSITION);
	_float3		vUp = Get_State(STATE_UP);
	vPosition -= *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeedPerSec * fTimeDelta;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::LookAt(const _float3& vTargetPos)
{
	_float3		vScaled = Compute_Scaled(); //스케일값 저장해놓고

	_float3		vPosition = Get_State(STATE_POSITION); // 위치벡터 저장해놓고

	_float3		vLook = vTargetPos - vPosition; // 방향벡터만들어서 룩벡터에 싸악만들고

	_float3		vRight = {};

	_float3		vUpDir{ 0.f, 1.f, 0.f };	// 임의의 업벡터
	D3DXVec3Cross(&vRight, &vUpDir, &vLook); // 라이트 구하고

	_float3		vUp = {};
	D3DXVec3Cross(&vUp, &vLook, &vRight); // 룩이랑 라이트로 외적 때려서 Up 구하고

	Set_State(STATE_RIGHT, vRight.GetNormalized() * vScaled.x);
	Set_State(STATE_UP, vUp.GetNormalized() * vScaled.y);
	Set_State(STATE_LOOK, vLook.GetNormalized() * vScaled.z);
}

void CTransform::Chase(const _float3& vTargetPos, _float fTimeDelta, _float fMinDistance)
{
	_float3		vPosition = Get_State(STATE_POSITION); // 스케일값 소실할 수도 있으니 저장

	_float3		vMoveDir = _float3(vTargetPos - vPosition).GetNormalized(); // 방햑벡터 만들고

	if (fMinDistance <= _float3::Distance(vTargetPos, vPosition)) // 이제 대충 따라오게 하는거 뭔말인지 알지
	{
		vPosition +=vMoveDir * m_fSpeedPerSec * fTimeDelta;
	}

	Set_State(STATE_POSITION, vPosition);
}


void CTransform::Turn(const _float3& vAxis, _float fTimeDelta)
{
	_float3			vRight = Get_State(STATE_RIGHT); // 라이트 저장
	_float3			vUp = Get_State(STATE_UP); // 업 저장
	_float3			vLook = Get_State(STATE_LOOK); // 룩 저장

	_float4x4		RotationMatrix; // 회전 행렬 싸악 만들어주고


	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, m_fRotationPerSec * fTimeDelta);

	// D3DXVec4Transform();
	// D3DXVec3TransformCoord();
	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotation(const _float3& vAxis, _float fRadian)
{
	_float3		vScaled = Compute_Scaled();

	_float3			vRight = _float3(1.f, 0.f, 0.f) * vScaled.x;
	_float3			vUp = _float3(0.f, 1.f, 0.f) * vScaled.y;
	_float3			vLook = _float3(0.f, 0.f, 1.f) * vScaled.z;

	_float4x4		RotationMatrix;
	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, fRadian);

	// D3DXVec4Transform();
	// D3DXVec3TransformCoord();
	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotate_EulerAngles(const _float3& vEulerAngles)
{
	m_vEulerAngles = vEulerAngles;
	D3DXMATRIX matRotation;
	D3DXMatrixRotationYawPitchRoll(&matRotation,
		D3DXToRadian(vEulerAngles.y),  // Yaw (Y축 회전)
		D3DXToRadian(vEulerAngles.x),  // Pitch (X축 회전)
		D3DXToRadian(vEulerAngles.z)); // Roll (Z축 회전)

	_float3 vScale = Compute_Scaled();

	// 새로운 right, up, look 벡터 계산
	_float3 vRight = _float3(matRotation._11, matRotation._12, matRotation._13) * vScale.x;
	_float3 vUp = _float3(matRotation._21, matRotation._22, matRotation._23) * vScale.y;
	_float3 vLook = _float3(matRotation._31, matRotation._32, matRotation._33) * vScale.z;

	// 상태 벡터 업데이트
	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotation_Axis(const _float3& vAxis, float fAngle)
{
	D3DXMATRIX matRot;
	D3DXMatrixRotationAxis(&matRot, &vAxis, fAngle);

	// Right/Up/Look 벡터에 회전 행렬 적용
	_float3 vRight = Get_State(STATE_RIGHT);
	_float3 vUp = Get_State(STATE_UP);
	_float3 vLook = Get_State(STATE_LOOK);

	D3DXVec3TransformCoord(&vRight, &vRight, &matRot);
	D3DXVec3TransformCoord(&vUp, &vUp, &matRot);
	D3DXVec3TransformCoord(&vLook, &vLook, &matRot);


	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}
void CTransform::Go_Pos(const _float3& vMove)
{
	_float3 vPos = Get_State(STATE_POSITION);
	Set_State(STATE_POSITION, vPos + vMove);
}


CTransform* CTransform::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTransform* pInstance = new CTransform(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
 	__super::Free();


}
