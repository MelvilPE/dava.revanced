#include "FBXSceneExport.h"
#include "FBXMeshExport.h"
#include "FBXSkeletonExport.h"

#include "Scene3D/Entity.h"
#include "Scene3D/Components/TransformComponent.h"
#include "Scene3D/Components/RenderComponent.h"
#include "Scene3D/Components/SkeletonComponent.h"

namespace DAVA
{
namespace FBXExporterDetails
{
void ExportSceneHierarchyRecursive(const Entity* entity,
                                   FbxNode* parentFbxNode,
                                   FbxScene* fbxScene)
{
    DVASSERT(entity != nullptr);

    // ── Create the FbxNode for this entity ────────────────────────────────
    FbxNode* fbxNode = FbxNode::Create(fbxScene, entity->GetName().c_str());
    parentFbxNode->AddChild(fbxNode);

    RegisterExportedNode(static_cast<const void*>(entity), fbxNode);

    // ── Transform ─────────────────────────────────────────────────────────
    // GetComponent is non-const on Entity; cast away scene-graph const here.
    // The cast is safe: we only read from the component.
    Entity* mutableEntity = const_cast<Entity*>(entity);

    RenderComponent* renderComp   = mutableEntity->GetComponent<RenderComponent>();
    SkeletonComponent* skeletonComp = mutableEntity->GetComponent<SkeletonComponent>();

    if (renderComp == nullptr)
    {
        TransformComponent* tc = mutableEntity->GetComponent<TransformComponent>();
        if (tc != nullptr)
        {
            // GetLocalTransform() returns a const Transform&
            FbxAMatrix fbxTransform = ToFbxAMatrix(tc->GetLocalTransform());
            fbxNode->LclTranslation.Set(fbxTransform.GetT());
            fbxNode->LclRotation.Set(fbxTransform.GetR());
            fbxNode->LclScaling.Set(fbxTransform.GetS());
        }
    }

    // ── Skeleton (before mesh so joint nodes are in cache) ────────────────
    if (skeletonComp != nullptr)
        ExportSkeleton(skeletonComp, fbxNode, fbxScene);

    // ── Mesh ──────────────────────────────────────────────────────────────
    if (renderComp != nullptr)
        ExportMeshFromEntity(entity, fbxNode, fbxScene);

    // ── Recurse ───────────────────────────────────────────────────────────
    int32 childCount = entity->GetChildrenCount();
    for (int32 c = 0; c < childCount; ++c)
        ExportSceneHierarchyRecursive(entity->GetChild(c), fbxNode, fbxScene);
}

}; //ns FBXExporterDetails
}; //ns DAVA
