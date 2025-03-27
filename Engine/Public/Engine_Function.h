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

	//Helper Functions
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

	inline string WideToUtf8(const wchar_t* wstr)
	{
		if (!wstr) return "";

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		std::string strUtf8(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strUtf8[0], size_needed, NULL, NULL);
		strUtf8.resize(strUtf8.length() - 1); // 널 종료자 제거

		return strUtf8;
	}

	// UTF-8을 와이드 문자열로 변환 (반대 방향 변환도 필요함)
	inline wstring Utf8ToWide(const string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
		wstrTo.resize(wstrTo.length() - 1); // 널 종료자 제거

		return wstrTo;
	}
}

	