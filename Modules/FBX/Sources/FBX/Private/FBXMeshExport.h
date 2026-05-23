#pragma once

#include "FBXExportUtils.h"

namespace DAVA
{
class Entity;

namespace FBXExporterDetails
{
// Export the Mesh or SkinnedMesh attached to 'entity' as one or more FbxMesh
// nodes added as children of 'parentFbxNode'.
// The SkeletonComponent (if present) must have been exported first so that
// joint FbxNodes are already in the export cache.
void ExportMeshFromEntity(const Entity* entity,
                          FbxNode* parentFbxNode,
                          FbxScene* fbxScene);

}; //ns FBXExporterDetails
}; //ns DAVA
