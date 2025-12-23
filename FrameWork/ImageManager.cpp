#include "ImageManager.h"

ImageManager* ImageManager::instance = nullptr;

ImageManager* ImageManager::GetInstance()
{
    if (!instance)
        instance = new ImageManager();
    return instance;
}

void ImageManager::Release()
{
    if (instance)
    {
        instance->Cleanup();
        delete instance;
        instance = nullptr;
    }
}

ImageManager::ImageManager()
    : sprite(nullptr), d3dDevice(nullptr)
{
}

ImageManager::~ImageManager()
{
    Cleanup();
}

void ImageManager::Init(LPDIRECT3DDEVICE9 device)
{
    d3dDevice = device;
    D3DXCreateSprite(d3dDevice, &sprite);
}

bool ImageManager::LoadImage(const std::string& key, const std::wstring& filename, LPDIRECT3DDEVICE9 device)
{
    if (images.find(key) != images.end())
        return true; // 이미 로드된 이미지

    LPDIRECT3DTEXTURE9 tex = nullptr;
    HRESULT hr = D3DXCreateTextureFromFileW(device, filename.c_str(), &tex);
    if (FAILED(hr))
        return false;

    // 텍스처 크기 얻기
    D3DSURFACE_DESC desc;
    tex->GetLevelDesc(0, &desc);

    ImageInfo info;
    info.texture = tex;
    info.width = desc.Width;
    info.height = desc.Height;

    images[key] = info;
    return true;
}

void ImageManager::Render(const std::string& key, float x, float y)
{
    auto it = images.find(key);
    if (it == images.end() || !sprite)
        return;

    sprite->Begin(D3DXSPRITE_ALPHABLEND);
    RECT rc = { 0, 0, it->second.width, it->second.height };
    D3DXVECTOR3 pos(x, y, 0);
    sprite->Draw(it->second.texture, &rc, nullptr, &pos, D3DCOLOR_XRGB(255, 255, 255));
    sprite->End();
}

void ImageManager::Cleanup()
{
    for (auto& pair : images)
    {
        if (pair.second.texture)
        {
            pair.second.texture->Release();
            pair.second.texture = nullptr;
        }
    }
    images.clear();

    if (sprite)
    {
        sprite->Release();
        sprite = nullptr;
    }
    d3dDevice = nullptr;
}