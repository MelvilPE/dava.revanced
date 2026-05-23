#include "FBXSkeletonExport.h"

#include "Logger/Logger.h"
#include "Scene3D/Components/SkeletonComponent.h"

namespace DAVA
{
namespace FBXExporterDetails
{
namespace FBXSkeletonExportDetails
{
// Recursively create FbxSkeleton nodes for every joint whose parentIndex
// equals parentIdx (INVALID_JOINT_INDEX for root joints).
void BuildSkeletonHierarchyRecursive(
    const SkeletonComponent* skeletonComponent,
    uint32 parentIdx,
    FbxNode* parentFbxNode,
    FbxScene* fbxScene,
    Vector<FbxNode*>& outJointNodes)
{
    uint32 jointCount = uint32(skeletonComponent->GetJointsCount());
    for (uint32 j = 0; j < jointCount; ++j)
    {
        const SkeletonComponent::Joint& joint = skeletonComponent->GetJoint(j);
        if (joint.parentIndex != parentIdx)
            continue;

        // ── Create the FbxSkeleton attribute ────────────────────────────
        FbxSkeleton* fbxSkelAttr = FbxSkeleton::Create(fbxScene, joint.name.c_str());
        fbxSkelAttr->SetSkeletonType(parentIdx == SkeletonComponent::INVALID_JOINT_INDEX
                                         ? FbxSkeleton::eRoot
                                         : FbxSkeleton::eLimb);

        FbxNode* jointNode = FbxNode::Create(fbxScene, joint.name.c_str());
        jointNode->SetNodeAttribute(fbxSkelAttr);

        // ── Apply bind-pose local transform ─────────────────────────────
        // joint.bindTransform is stored as global for the root, local for
        // children (matching EvaluateNodeTransform behaviour in FBXUtils).
        FbxAMatrix fbxBindTransform = ToFbxAMatrix(joint.bindTransform);
        jointNode->LclTranslation.Set(fbxBindTransform.GetT());
        jointNode->LclRotation.Set(fbxBindTransform.GetR());
        jointNode->LclScaling.Set(fbxBindTransform.GetS());

        parentFbxNode->AddChild(jointNode);

        // Register so mesh export can look this node up by joint index.
        // We use the joint's UID string as the cache key.
        RegisterExportedNode(reinterpret_cast<const void*>(static_cast<uintptr_t>(j)), jointNode);
        outJointNodes[j] = jointNode;

        // Recurse for children.
        BuildSkeletonHierarchyRecursive(skeletonComponent, j, jointNode, fbxScene, outJointNodes);
    }
}
} //ns FBXSkeletonExportDetails

FbxNode* ExportSkeleton(const SkeletonComponent* skeletonComponent,
                        FbxNode* parentNode,
                        FbxScene* fbxScene)
{
    using namespace FBXSkeletonExportDetails;

    if (skeletonComponent == nullptr)
        return nullptr;

    uint32 jointCount = uint32(skeletonComponent->GetJointsCount());
    if (jointCount == 0)
        return nullptr;

    Vector<FbxNode*> jointNodes(jointCount, nullptr);
    BuildSkeletonHierarchyRecursive(skeletonComponent, SkeletonComponent::INVALID_JOINT_INDEX,
                                    parentNode, fbxScene, jointNodes);

    // Return the first root joint node.
    for (FbxNode* node : jointNodes)
    {
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

}; //ns FBXExporterDetails
}; //ns DAVA
