#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)
class ENGINE_DLL ISerializable
{	public:
	virtual json Serialize() = 0;
	virtual void Deserialize(const json& j) = 0;

    // 와이드 문자열을 UTF-8로 변환 (std::string 반환해야 함)
    static string WideToUtf8(const wchar_t* wstr)
    {
        if (!wstr) return "";

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        std::string strUtf8(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strUtf8[0], size_needed, NULL, NULL);
        strUtf8.resize(strUtf8.length() - 1); // 널 종료자 제거

        return strUtf8;
    }

    // UTF-8을 와이드 문자열로 변환 (반대 방향 변환도 필요함)
    static wstring Utf8ToWide(const string& str)
    {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
        wstrTo.resize(wstrTo.length() - 1); // 널 종료자 제거

        return wstrTo;
    }

    static string WideToUtf8(const wstring& wstr)
    {
        if (wstr.empty()) return "";

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        string strUtf8(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &strUtf8[0], size_needed, NULL, NULL);
        strUtf8.resize(strUtf8.length() - 1); // �� ������ ����

        return strUtf8;
    }
};
END
