#include "FBXMaterialExport.h"

#include "Logger/Logger.h"
#include "Render/Material/NMaterial.h"
#include "Render/Material/NMaterialNames.h"
#include "Render/Texture.h"

namespace DAVA
{
namespace FBXExporterDetails
{
namespace FBXMaterialExportDetails
{
// NMaterial* keys (non-const) because GetParent()/GetLocalTextures() are non-const.
Map<NMaterial*, FbxSurfaceMaterial*> materialCache;

void AttachTexture(FbxScene* fbxScene, FbxSurfaceMaterial* fbxMaterial,
                   const char* fbxPropertyName, const FilePath& texturePath)
{
    if (texturePath.IsEmpty())
        return;

    FbxFileTexture* fbxTexture = FbxFileTexture::Create(fbxScene, texturePath.GetBasename().c_str());
    fbxTexture->SetFileName(texturePath.GetAbsolutePathname().c_str());
    fbxTexture->SetTextureUse(FbxTexture::eStandard);
    fbxTexture->SetMappingType(FbxTexture::eUV);
    fbxTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);

    FbxProperty prop = fbxMaterial->FindProperty(fbxPropertyName);
    if (prop.IsValid())
        prop.ConnectSrcObject(fbxTexture);
}
} //ns FBXMaterialExportDetails

FbxSurfaceMaterial* ExportMaterial(NMaterial* material, FbxScene* fbxScene)
{
    using namespace FBXMaterialExportDetails;

    if (material == nullptr)
        return nullptr;

    // Walk up to the template (skip instances created at import time).
    NMaterial* baseMaterial = material;
    while (baseMaterial->GetParent() != nullptr)
        baseMaterial = baseMaterial->GetParent();

    auto found = materialCache.find(baseMaterial);
    if (found != materialCache.end())
        return found->second;

    FbxSurfaceLambert* fbxMaterial = FbxSurfaceLambert::Create(
        fbxScene, baseMaterial->GetMaterialName().c_str());

    // ── Textures ────────────────────────────────────────────────────────────
    // GetLocalTextures() returns a const ref — safe to use on non-const ptr.
    // We read the MaterialTextureInfo::path directly to avoid calling the
    // non-const GetEffectiveTexture().
    auto exportTexture = [&](const FastName& davaSlot, const char* fbxProp)
    {
        const UnorderedMap<FastName, MaterialTextureInfo*>& textures =
            baseMaterial->GetLocalTextures();

        auto it = textures.find(davaSlot);
        if (it != textures.end() && it->second != nullptr && !it->second->path.IsEmpty())
            AttachTexture(fbxScene, fbxMaterial, fbxProp, it->second->path);
    };

    exportTexture(NMaterialTextureName::TEXTURE_ALBEDO, FbxSurfaceMaterial::sDiffuse);
    exportTexture(NMaterialTextureName::TEXTURE_NORMAL, FbxSurfaceMaterial::sNormalMap);

    materialCache.emplace(baseMaterial, fbxMaterial);
    return fbxMaterial;
}

void ClearMaterialExportCache()
{
    // FbxSurfaceMaterial objects are owned by FbxScene — do not destroy here.
    FBXMaterialExportDetails::materialCache.clear();
}

}; //ns FBXExporterDetails
}; //ns DAVA
