#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_AddRef(T& Instance)
	{
		unsigned int iRefCnt = { 0 };

		if (nullptr != Instance)
			iRefCnt = Instance->AddRef();

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_Release(T& Instance)
	{
		unsigned int iRefCnt = { 0 };

		if (nullptr != Instance)
		{
			iRefCnt = Instance->Release();
			if (0 == iRefCnt)
				Instance = nullptr;
		}
		return iRefCnt;
	}

	//Helper Functions
	inline float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound)
			return lowBound;
		float f = (rand() % 10000) * 0.0001f;

		return (f * (highBound - lowBound)) + lowBound;
	}

	inline void GetRandomVector(D3DXVECTOR3* pOut, D3DXVECTOR3* pMin, D3DXVECTOR3* pMax)
	{
		pOut->x = GetRandomFloat(pMin->x, pMax->x);
		pOut->y = GetRandomFloat(pMin->y, pMax->y);
		pOut->z = GetRandomFloat(pMin->z, pMax->z);
	}

	inline DWORD FtoDW(float f)
	{
		return *((DWORD*)&f);
	}

	inline D3DXVECTOR4 DWORDToFloat4_Color(DWORD color) {
		D3DXVECTOR4 result;

		// Extract the individual color components (8 bits each)
		result.w = ((color >> 24) & 0xFF) / 255.0f; // Alpha (most significant byte)
		result.x = ((color >> 16) & 0xFF) / 255.0f; // Red
		result.y = ((color >> 8) & 0xFF) / 255.0f;  // Green
		result.z = (color & 0xFF) / 255.0f;         // Blue

		return result;
	}
}

	