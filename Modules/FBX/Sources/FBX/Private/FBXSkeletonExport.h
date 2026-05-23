#pragma once

#include "FBXExportUtils.h"

namespace DAVA
{
class Entity;
class SkeletonComponent;

namespace FBXExporterDetails
{
// Build the FbxSkeleton node hierarchy from a SkeletonComponent and attach it
// to parentNode. The created joint nodes are registered in the export cache so
// that FBXMeshExport can reference them when building skin clusters.
// Returns the root skeleton FbxNode (or nullptr on failure).
FbxNode* ExportSkeleton(const SkeletonComponent* skeletonComponent,
                        FbxNode* parentNode,
                        FbxScene* fbxScene);

}; //ns FBXExporterDetails
}; //ns DAVA
