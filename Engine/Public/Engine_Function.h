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

	//Helper Functions
	inline void GetRandomVector(D3DXVECTOR3* pOut, D3DXVECTOR3* pCenter, float radius)
	{
		// 반지름이 0 이하면 중심점 반환
		if (radius <= 0.f) {
			pOut->x = pCenter->x;
			pOut->y = pCenter->y;
			pOut->z = pCenter->z;
		}

		// 1. 균일 분포를 위한 세제곱근 스케일링된 반지름 생성
		//    u를 [0, 1] 범위에서 균일하게 선택
		float u = GetRandomFloat(0.f, 1.f);
		//    반지름 r = R * cbrt(u) 계산 (pow(u, 1/3))
		float r = radius * powf(u, 1.0f / 3.0f);

		// 2. 구 표면에서의 균일한 방향 벡터 생성
		//    cos(theta) 값을 [-1, 1] 범위에서 균일하게 선택 (z축 좌표)
		float cosTheta = GetRandomFloat(-1.f, 1.f);
		//    phi 값을 [0, 2*PI] 범위에서 균일하게 선택 (xy 평면 각도)
		float phi = GetRandomFloat(0.f, 2.0f * D3DX_PI);

		// 3. 구면 좌표계를 직교 좌표계로 변환
		float sinTheta = sqrtf(1.0f - cosTheta * cosTheta); // sin(theta) 계산
		float x = r * sinTheta * cosf(phi);
		float y = r * sinTheta * sinf(phi);
		float z = r * cosTheta;

		// 4. 구의 중심점을 기준으로 오프셋 적용
		pOut->x = pCenter->x + x;	
		pOut->y = pCenter->y + y;
		pOut->z = pCenter->z + z;
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


	inline float RoundToDecimalPlaces(float value, int places) 
	{
		ostringstream oss;
		oss << fixed << std::setprecision(places) << value;
		return stof(oss.str());
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

	inline float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	inline D3DXVECTOR3 VectorLerp(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float t)
	{
		return {
			Lerp(a.x, b.x, t),
			Lerp(a.y, b.y, t),
			Lerp(a.z, b.z, t) // z축도 보간
		};
	}

	inline D3DXVECTOR4 Vector4Lerp(const D3DXVECTOR4& a, const D3DXVECTOR4& b, float t)
	{
		return {
			Lerp(a.x, b.x, t),
			Lerp(a.y, b.y, t),
			Lerp(a.z, b.z, t), // z축도 보간
			Lerp(a.w, b.w, t) // z축도 보간
		};
	}

	inline D3DCOLOR ColorLerp(const D3DCOLOR& a, const D3DCOLOR& b, float t)
	{
		// 1. 각 색상 요소(A, R, G, B)를 DWORD에서 분리 (언패킹)
		//    float으로 변환하여 정밀도 확보
		float startA = static_cast<float>((a >> 24) & 0xFF);
		float startR = static_cast<float>((a >> 16) & 0xFF);
		float startG = static_cast<float>((a >> 8) & 0xFF);
		float startB = static_cast<float>((a >> 0) & 0xFF);

		float endA = static_cast<float>((b >> 24) & 0xFF);
		float endR = static_cast<float>((b >> 16) & 0xFF);
		float endG = static_cast<float>((b >> 8) & 0xFF);
		float endB = static_cast<float>((b >> 0) & 0xFF);

		// 2. 각 요소를 개별적으로 선형 보간
		float interpA = Lerp(startA, endA, t);
		float interpR = Lerp(startR, endR, t);
		float interpG = Lerp(startG, endG, t);
		float interpB = Lerp(startB, endB, t);

		// 3. 보간된 float 값을 다시 BYTE(0-255) 범위로 변환하고 DWORD로 합치기 (리패킹)
		//    반올림 처리를 위해 + 0.5f 후 캐스팅 또는 std::round 사용
		BYTE byteA = static_cast<BYTE>(interpA + 0.5f);
		BYTE byteR = static_cast<BYTE>(interpR + 0.5f);
		BYTE byteG = static_cast<BYTE>(interpG + 0.5f);
		BYTE byteB = static_cast<BYTE>(interpB + 0.5f);

		// BYTE 값들이 0-255 범위를 벗어나지 않도록 한 번 더 클램핑 (선택 사항이지만 안전함)
		// byteA = std::min(std::max(byteA, (BYTE)0), (BYTE)255);
		// ... (R, G, B에 대해서도 동일하게)

		// 각 BYTE 요소를 비트 시프트하여 DWORD로 조합
		DWORD resultColor = (static_cast<DWORD>(byteA) << 24) |
			(static_cast<DWORD>(byteR) << 16) |
			(static_cast<DWORD>(byteG) << 8) |
			(static_cast<DWORD>(byteB) << 0);

		return resultColor;
	}
}

	