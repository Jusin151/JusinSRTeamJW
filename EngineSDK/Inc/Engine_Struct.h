#pragma once

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		unsigned int	iNumLevels;
	}ENGINE_DESC;

	/*D3DDECLUSAGE*/

	typedef struct tagVertexPositionTexcoord
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR2		vTexcoord;
	}VTXPOSTEX;


	typedef struct tagVector3 : public D3DXVECTOR3
	{
		tagVector3() : D3DXVECTOR3(0.0f, 0.0f, 0.0f) {}
		tagVector3(float _x, float _y, float _z) : D3DXVECTOR3(_x, _y, _z) {}
		tagVector3(const D3DXVECTOR3& v) : D3DXVECTOR3(v) {}

		float Length() const { return D3DXVec3Length(this); }
		float LengthSq() const { return D3DXVec3LengthSq(this); }

		/// <summary>
		/// 자체를 정규화 시킴
		/// </summary>
		void Normalize() {
			float len = Length();
			if (len > 0.0001f) {
				x /= len;
				y /= len;
				z /= len;
			}
		}

		/// <summary>
		/// 자신과 다른 벡터 사이의 각도
		/// </summary>
		/// <param name="to"></param>
		/// <returns>라디안 단위 각도</returns>

		float Angle(const tagVector3& to)
		{
			float dot = this->Dot(to);
			float lenMul = this->Length() * to.Length();

			// 0으로 나누기 방지
			if (lenMul < 0.0001f) return 0.0f;

			// dot / (|a|*|b|) 값은 -1에서 1 사이여야 함
			float cosAngle = dot / lenMul;

			// 수치 오류로 인한 범위 초과 방지
			cosAngle = min(1.0f, max(-1.0f, cosAngle));

			return acosf(cosAngle);
		}

		/// <summary>
		/// 정규화 시킨 일부 값 반환하는 함수
		/// </summary>
		/// <returns></returns>
		tagVector3 GetNormalized() const
		{
			tagVector3 result = *this;
			result.Normalize();
			return result;
		}

		/// <summary>
	   /// 외적 계산
	   /// </summary>
	   /// <param name="other">외적을 계산할 다른 벡터</param>
	  /// <returns>두 벡터에 수직인 새 벡터</returns>
		tagVector3 Cross(const tagVector3& other)
		{
			tagVector3 tmpVector;
			D3DXVec3Cross(&tmpVector, this, &other);
			return tmpVector;
		}

		/// <summary>
		/// 내적 할 벡터
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		float Dot(const tagVector3& other)
		{
			return D3DXVec3Dot(this, &other);
		}

		/// <summary>
	/// this 위치에서 other 위치까지의 거리를 계산합니다
	/// </summary>
	/// <param name="other">목표 위치 벡터</param>
	/// <returns>두 점 사이의 거리</returns>
		float Distance(const tagVector3& other)
		{
			tagVector3 tmpVector = other - *this;
			return D3DXVec3Length(&tmpVector);
		}

		static tagVector3 Zero() { return tagVector3(0.0f, 0.0f, 0.0f); }
		static tagVector3 One() { return tagVector3(1.0f, 1.0f, 1.0f); }
		static tagVector3 Up() { return tagVector3(0.0f, 1.0f, 0.0f); }
		static tagVector3 Right() { return tagVector3(1.0f, 0.0f, 0.0f); }
		static tagVector3 Left() { return tagVector3(-1.0f, 0.0f, 0.0f); }
		static tagVector3 Forward() { return tagVector3(0.0f, 0.0f, 1.0f); }
		static tagVector3 Down() { return tagVector3(0.0f, -1.0f, 0.0f); }
		static tagVector3 Back() { return tagVector3(0.0f, 0.0f, -1.0f); }

	}VECTOR3;
}