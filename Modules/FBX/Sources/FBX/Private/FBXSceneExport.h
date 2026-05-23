#pragma once

#include "FBXExportUtils.h"

namespace DAVA
{
class Entity;
class Scene;

namespace FBXExporterDetails
{
// Recursively walk the DAVA entity hierarchy and build the equivalent
// FbxNode tree inside fbxScene, exporting meshes, skeletons and materials.
void ExportSceneHierarchyRecursive(const Entity* entity,
                                   FbxNode* parentFbxNode,
                                   FbxScene* fbxScene);

}; //ns FBXExporterDetails
}; //ns DAVA
