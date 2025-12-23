#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <unordered_map>

class ImageManager
{
public:
    static ImageManager* GetInstance();
    static void Release();

    bool LoadImage(const std::string& key, const std::wstring& filename, LPDIRECT3DDEVICE9 device);
    void Render(const std::string& key, float x, float y);

private:
    ImageManager();
    ~ImageManager();

    static ImageManager* instance;

    struct ImageInfo
    {
        LPDIRECT3DTEXTURE9 texture;
        int width;
        int height;
    };

    std::unordered_map<std::string, ImageInfo> images;

    LPD3DXSPRITE sprite;
    LPDIRECT3DDEVICE9 d3dDevice;

public:
    void Init(LPDIRECT3DDEVICE9 device);
    void Cleanup();
};