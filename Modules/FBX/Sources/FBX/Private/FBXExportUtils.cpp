#include "FBXExportUtils.h"

namespace DAVA
{
namespace FBXExporterDetails
{
namespace FBXExportUtilsDetails
{
Map<const void*, FbxNode*> exportedNodeCache;
}

// ── Type converters ────────────────────────────────────────────────────────

// FbxAMatrix has no Set(row,col,val). We go through FbxMatrix (which does),
// then convert — both share the same memory layout.
FbxAMatrix ToFbxAMatrix(const Matrix4& matrix)
{
    FbxAMatrix mx;
    for (int r = 0; r < 4; ++r)
        mx.SetRow(r, FbxVector4(double(matrix._data[r][0]), double(matrix._data[r][1]), double(matrix._data[r][2]), double(matrix._data[r][3])));
    return mx;
}

// Overload for DAVA Transform (used by TransformComponent::GetLocalTransform()).
// We decompose T/R/S directly instead of going through Matrix4.
FbxAMatrix ToFbxAMatrix(const Transform& transform)
{
    FbxAMatrix mx;
    const Vector3& t = transform.GetTranslation();
    const Quaternion& r = transform.GetRotation();
    const Vector3& s = transform.GetScale();

    mx.SetT(FbxVector4(double(t.x), double(t.y), double(t.z)));
    mx.SetQ(FbxQuaternion(double(r.x), double(r.y), double(r.z), double(r.w)));
    mx.SetS(FbxVector4(double(s.x), double(s.y), double(s.z)));
    return mx;
}

FbxVector4 ToFbxVector4(const Vector3& v)
{
    return FbxVector4(double(v.x), double(v.y), double(v.z));
}

FbxVector2 ToFbxVector2(const Vector2& v)
{
    return FbxVector2(double(v.x), double(v.y));
}

// ── Scene helpers ──────────────────────────────────────────────────────────

FbxManager* CreateFbxManager()
{
    FbxManager* manager = FbxManager::Create();
    FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(ios);
    return manager;
}

void ConfigureScene(FbxScene* fbxScene)
{
    FbxAxisSystem::MayaZUp.ConvertScene(fbxScene);
    FbxSystemUnit::m.ConvertScene(fbxScene);
}

// ── Node cache ─────────────────────────────────────────────────────────────

void RegisterExportedNode(const void* key, FbxNode* fbxNode)
{
    FBXExportUtilsDetails::exportedNodeCache[key] = fbxNode;
}

FbxNode* FindExportedNode(const void* key)
{
    auto it = FBXExportUtilsDetails::exportedNodeCache.find(key);
    return (it != FBXExportUtilsDetails::exportedNodeCache.end()) ? it->second : nullptr;
}

void ClearExportedNodeCache()
{
    FBXExportUtilsDetails::exportedNodeCache.clear();
}

}; // namespace FBXExporterDetails
}; // namespace DAVA