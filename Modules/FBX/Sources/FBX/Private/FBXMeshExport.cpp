#include "FBXMeshExport.h"
#include "FBXMaterialExport.h"

#include "Logger/Logger.h"
#include "Scene3D/Entity.h"
#include "Scene3D/Components/RenderComponent.h"
#include "Scene3D/Components/SkeletonComponent.h"
#include "Render/Highlevel/Mesh.h"
#include "Render/Highlevel/SkinnedMesh.h"
#include "Render/Highlevel/RenderBatch.h"
#include "Render/3D/PolygonGroup.h"
#include "Render/Material/NMaterial.h"

namespace DAVA
{
namespace FBXExporterDetails
{
namespace FBXMeshExportDetails
{
// All PolygonGroup::Get* methods are non-const (inline accessors use
// non-const internal pointers). We receive non-const pointers throughout.

FbxNode* ExportPolygonGroup(
    PolygonGroup* polygonGroup,
    NMaterial*    material,
    SkeletonComponent* skeletonComponent,
    const char*   meshNodeName,
    FbxNode*      parentFbxNode,
    FbxScene*     fbxScene)
{
    DVASSERT(polygonGroup != nullptr);

    int32 vertexFormat  = polygonGroup->GetFormat();
    int32 vertexCount   = polygonGroup->GetVertexCount();
    int32 indexCount    = polygonGroup->GetIndexCount();
    int32 triangleCount = indexCount / 3;

    // ── FbxMesh ───────────────────────────────────────────────────────────
    FbxMesh* fbxMesh = FbxMesh::Create(fbxScene, meshNodeName);

    fbxMesh->InitControlPoints(vertexCount);
    FbxVector4* controlPoints = fbxMesh->GetControlPoints();
    for (int32 v = 0; v < vertexCount; ++v)
    {
        Vector3 pos;
        polygonGroup->GetCoord(v, pos);
        controlPoints[v] = ToFbxVector4(pos);
    }

    // ── UV sets ───────────────────────────────────────────────────────────
    const int32 uvSlots[] = { EVF_TEXCOORD0, EVF_TEXCOORD1, EVF_TEXCOORD2, EVF_TEXCOORD3 };
    const char* uvNames[] = { "UVMap0", "UVMap1", "UVMap2", "UVMap3" };

    for (int32 t = 0; t < 4; ++t)
    {
        if (!(vertexFormat & uvSlots[t]))
            continue;

        FbxGeometryElementUV* uvElement = fbxMesh->CreateElementUV(uvNames[t]);
        uvElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        uvElement->SetReferenceMode(FbxGeometryElement::eDirect);

        for (int32 v = 0; v < vertexCount; ++v)
        {
            Vector2 uv;
            polygonGroup->GetTexcoord(t, v, uv);
            uv.y = -uv.y; // Flip back (importer flipped Y at load time)
            uvElement->GetDirectArray().Add(ToFbxVector2(uv));
        }
    }

    // ── Normals ───────────────────────────────────────────────────────────
    if (vertexFormat & EVF_NORMAL)
    {
        FbxGeometryElementNormal* normalElement = fbxMesh->CreateElementNormal();
        normalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        normalElement->SetReferenceMode(FbxGeometryElement::eDirect);

        for (int32 v = 0; v < vertexCount; ++v)
        {
            Vector3 normal;
            polygonGroup->GetNormal(v, normal);
            normalElement->GetDirectArray().Add(ToFbxVector4(normal));
        }
    }

    // ── Tangents ──────────────────────────────────────────────────────────
    if (vertexFormat & EVF_TANGENT)
    {
        FbxGeometryElementTangent* tangentElement = fbxMesh->CreateElementTangent();
        tangentElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        tangentElement->SetReferenceMode(FbxGeometryElement::eDirect);

        for (int32 v = 0; v < vertexCount; ++v)
        {
            Vector3 tangent;
            polygonGroup->GetTangent(v, tangent);
            tangentElement->GetDirectArray().Add(ToFbxVector4(tangent));
        }
    }

    // ── Binormals ─────────────────────────────────────────────────────────
    if (vertexFormat & EVF_BINORMAL)
    {
        FbxGeometryElementBinormal* binormalElement = fbxMesh->CreateElementBinormal();
        binormalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        binormalElement->SetReferenceMode(FbxGeometryElement::eDirect);

        for (int32 v = 0; v < vertexCount; ++v)
        {
            Vector3 binormal;
            polygonGroup->GetBinormal(v, binormal);
            binormalElement->GetDirectArray().Add(ToFbxVector4(binormal));
        }
    }

    // ── Index buffer ──────────────────────────────────────────────────────
    FbxGeometryElementMaterial* matElement = fbxMesh->CreateElementMaterial();
    matElement->SetMappingMode(FbxGeometryElement::eAllSame);
    matElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
    matElement->GetIndexArray().Add(0);

    for (int32 tri = 0; tri < triangleCount; ++tri)
    {
        fbxMesh->BeginPolygon(0);
        for (int32 v = 0; v < 3; ++v)
        {
            int32 idx = 0;
            polygonGroup->GetIndex(tri * 3 + v, idx);
            fbxMesh->AddPolygon(idx);
        }
        fbxMesh->EndPolygon();
    }

    // ── FbxNode ───────────────────────────────────────────────────────────
    FbxNode* meshNode = FbxNode::Create(fbxScene, meshNodeName);
    meshNode->SetNodeAttribute(fbxMesh);

    FbxSurfaceMaterial* fbxMaterial = ExportMaterial(material, fbxScene);
    if (fbxMaterial != nullptr)
        meshNode->AddMaterial(fbxMaterial);

    parentFbxNode->AddChild(meshNode);

    // ── Skinning ──────────────────────────────────────────────────────────
    bool hasSoftSkinning = (vertexFormat & EVF_JOINTINDEX) && (vertexFormat & EVF_JOINTWEIGHT);
    bool hasHardSkinning = (vertexFormat & EVF_HARD_JOINTINDEX) != 0;

    if (skeletonComponent != nullptr && (hasSoftSkinning || hasHardSkinning))
    {
        uint32 jointCount = uint32(skeletonComponent->GetJointsCount());

        FbxSkin* fbxSkin = FbxSkin::Create(fbxScene, "");
        fbxSkin->SetSkinningType(FbxSkin::eLinear);

        Vector<FbxCluster*> clusters(jointCount, nullptr);
        for (uint32 j = 0; j < jointCount; ++j)
        {
            FbxNode* jointNode = FindExportedNode(reinterpret_cast<const void*>(static_cast<uintptr_t>(j)));
            if (jointNode == nullptr)
                continue;

            FbxCluster* cluster = FbxCluster::Create(fbxScene, "");
            cluster->SetLink(jointNode);
            cluster->SetLinkMode(FbxCluster::eNormalize);

            const SkeletonComponent::Joint& joint = skeletonComponent->GetJoint(j);
            cluster->SetTransformLinkMatrix(ToFbxAMatrix(joint.bindTransform));
            cluster->SetTransformMatrix(FbxAMatrix()); // identity (vertices already in world space)

            clusters[j] = cluster;
        }

        for (int32 v = 0; v < vertexCount; ++v)
        {
            if (hasHardSkinning)
            {
                int32 jointIndex = 0;
                polygonGroup->GetHardJointIndex(v, jointIndex);
                if (uint32(jointIndex) < jointCount && clusters[jointIndex] != nullptr)
                    clusters[jointIndex]->AddControlPointIndex(v, 1.0);
            }
            else
            {
                for (uint32 slot = 0; slot < PolygonGroup::MAX_VERTEX_JOINTS_COUNT; ++slot)
                {
                    int32   jointIndex  = 0;
                    float32 jointWeight = 0.f;
                    polygonGroup->GetJointIndex(v, int32(slot), jointIndex);
                    polygonGroup->GetJointWeight(v, int32(slot), jointWeight);

                    if (jointWeight > EPSILON && uint32(jointIndex) < jointCount && clusters[jointIndex] != nullptr)
                        clusters[jointIndex]->AddControlPointIndex(v, double(jointWeight));
                }
            }
        }

        for (FbxCluster* cluster : clusters)
        {
            if (cluster != nullptr && cluster->GetControlPointIndicesCount() > 0)
                fbxSkin->AddCluster(cluster);
        }

        fbxMesh->AddDeformer(fbxSkin);
    }

    return meshNode;
}

} //ns FBXMeshExportDetails

// ── Public API ─────────────────────────────────────────────────────────────

void ExportMeshFromEntity(const Entity* entity, FbxNode* parentFbxNode, FbxScene* fbxScene)
{
    using namespace FBXMeshExportDetails;

    DVASSERT(entity != nullptr);

    // GetComponent is non-const; cast is safe (read-only usage).
    Entity* mutableEntity = const_cast<Entity*>(entity);

    RenderComponent*   renderComponent   = mutableEntity->GetComponent<RenderComponent>();
    SkeletonComponent* skeletonComponent = mutableEntity->GetComponent<SkeletonComponent>();

    if (renderComponent == nullptr)
        return;

    RenderObject* renderObject = renderComponent->GetRenderObject();
    if (renderObject == nullptr)
        return;

    const char* entityName = entity->GetName().c_str();

    uint32 batchCount = renderObject->GetRenderBatchCount();
    for (uint32 b = 0; b < batchCount; ++b)
    {
        RenderBatch*  batch = renderObject->GetRenderBatch(b);
        PolygonGroup* pg    = batch->GetPolygonGroup();
        NMaterial*    mat   = batch->GetMaterial();

        if (pg == nullptr)
            continue;

        String batchName = Format("%s_batch%d", entityName, int32(b));
        ExportPolygonGroup(pg, mat, skeletonComponent, batchName.c_str(), parentFbxNode, fbxScene);
    }
}

}; //ns FBXExporterDetails
}; //ns DAVA
