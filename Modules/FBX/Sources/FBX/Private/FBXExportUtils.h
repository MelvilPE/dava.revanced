#pragma once

#include "Base/BaseMath.h"
#include "Base/BaseTypes.h"
#include "Base/FastName.h"
#include "Math/Transform.h"

#define FBXSDK_SHARED //requested only for dynamic linking
#include <fbxsdk.h>

namespace DAVA
{
namespace FBXExporterDetails
{
// ── Type converters (inverse of FBXUtils) ──────────────────────────────────

FbxAMatrix ToFbxAMatrix(const Matrix4& matrix);
FbxAMatrix ToFbxAMatrix(const Transform& transform); // for TransformComponent::GetLocalTransform()
FbxVector4 ToFbxVector4(const Vector3& v);
FbxVector2 ToFbxVector2(const Vector2& v);

// ── Scene helpers ──────────────────────────────────────────────────────────

FbxManager* CreateFbxManager();
void        ConfigureScene(FbxScene* fbxScene);

// ── Node cache ─────────────────────────────────────────────────────────────

void     RegisterExportedNode(const void* key, FbxNode* fbxNode);
FbxNode* FindExportedNode(const void* key);
void     ClearExportedNodeCache();

}; //ns FBXExporterDetails
}; //ns DAVA
