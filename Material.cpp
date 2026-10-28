#include "Material.h"
#include "Graphics.h"

Material::Material(const char* name, DirectX::XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, 
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, DirectX::XMFLOAT2 uvScale, DirectX::XMFLOAT2 uvOffset)
{
    this->name = name;
    this->colorTint = colorTint;
    this->vertexShader = vertexShader;
    this->pixelShader = pixelShader;
    this->uvScale = uvScale;
    this->uvOffset = uvOffset;
}

Material::~Material()
{
}

const char* Material::GetName() const
{
    return name;
}

DirectX::XMFLOAT4 Material::GetColorTint() const
{
    return colorTint;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>* Material::GetTextureSRVs()
{
    return textureSRVs;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState>* Material::GetSamplers()
{
    return samplers;
}

DirectX::XMFLOAT2 Material::GetUVScale() const
{
    return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset() const
{
    return uvOffset;
}

void Material::SetColorTint(DirectX::XMFLOAT4 tint)
{
    colorTint = tint;
}

void Material::SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader)
{
    vertexShader = shader;
}

void Material::SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> shader)
{
    pixelShader = shader;
}

void Material::SetUVScale(DirectX::XMFLOAT2 scale)
{
    uvScale = scale;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 offset)
{
    uvOffset = offset;
}

void Material::AddTextureSRV(unsigned int index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs[index] = srv;
}

void Material::AddSampler(unsigned int index, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState)
{
    samplers[index] = samplerState;
}

void Material::BindTexturesAndSamplers()
{
    //Bind the textures
    for (int i = 0; i < 128; i++) {
        Graphics::Context->PSSetShaderResources(i, 1, textureSRVs[i].GetAddressOf());
    }
    
    // Set the samplers
    for (int i = 0; i < 16; i++) {
        Graphics::Context->PSSetSamplers(i, 1, samplers[i].GetAddressOf());
    }
}
