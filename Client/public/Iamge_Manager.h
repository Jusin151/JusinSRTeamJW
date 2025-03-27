#include "Client_Defines.h"
#include "GameObject.h"
#include "Image.h"

class CImage_Manager
{
public:
    static CImage_Manager* GetInstance()
    {
        static CImage_Manager instance;
        return &instance;
    }

private:
    CImage_Manager() = default;


    CImage_Manager(const CImage_Manager&) = delete;
    CImage_Manager& operator=(const CImage_Manager&) = delete;
    CImage_Manager(CImage_Manager&&) = delete;
    CImage_Manager& operator=(CImage_Manager&&) = delete;
public:

    void Add_Image(const wstring& tag, CImage* pUI)
    {
        if (m_mapImage.find(tag) != m_mapImage.end())
        {
            return;
        } 
        m_mapImage[tag] = pUI;
    }
    CImage* Get_Image(const wstring& tag)
    {
        auto it = m_mapImage.find(tag);
        if (it != m_mapImage.end())
        {
            return it->second;
        }
        return nullptr;
    }

private:
    unordered_map<wstring, CImage*> m_mapImage;
 

};


