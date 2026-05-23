#include "FBXExporter.h"

#include "Private/FBXExportUtils.h"
#include "Private/FBXMaterialExport.h"
#include "Private/FBXMeshExport.h"
#include "Private/FBXSceneExport.h"
#include "Private/FBXSkeletonExport.h"

#include "Logger/Logger.h"
#include "Scene3D/Scene.h"

namespace DAVA
{
namespace
{
void ClearAllExportCaches()
{
    FBXExporterDetails::ClearExportedNodeCache();
    FBXExporterDetails::ClearMaterialExportCache();
}

bool SaveFbxScene(FbxManager* fbxManager, FbxScene* fbxScene, const FilePath& fbxPath)
{
    FbxExporter* fbxExporter = FbxExporter::Create(fbxManager, "fbxExporter");

    bool initSuccess = fbxExporter->Initialize(
        fbxPath.GetAbsolutePathname().c_str(),
        -1, // default format (binary FBX)
        fbxManager->GetIOSettings());

    if (!initSuccess)
    {
        Logger::Error("[FBXExporter] Initialization error: %s",
                      fbxExporter->GetStatus().GetErrorString());
        fbxExporter->Destroy();
        return false;
    }

    bool exportSuccess = fbxExporter->Export(fbxScene);
    if (!exportSuccess)
    {
        Logger::Error("[FBXExporter] Export error: %s",
                      fbxExporter->GetStatus().GetErrorString());
    }

    fbxExporter->Destroy();
    return exportSuccess;
}
} //anonymous ns

bool FBXExporter::ExportSceneToFBX(const Scene* scene, const FilePath& fbxPath)
{
    if (scene == nullptr)
    {
        Logger::Error("[FBXExporter] ExportSceneToFBX: scene is null");
        return false;
    }

    FbxManager* fbxManager = FBXExporterDetails::CreateFbxManager();
    FbxScene*   fbxScene   = FbxScene::Create(fbxManager, "exportedScene");
    FBXExporterDetails::ConfigureScene(fbxScene);

    FbxNode* rootNode  = fbxScene->GetRootNode();
    int32    childCount = scene->GetChildrenCount();
    for (int32 c = 0; c < childCount; ++c)
        FBXExporterDetails::ExportSceneHierarchyRecursive(scene->GetChild(c), rootNode, fbxScene);

    bool success = SaveFbxScene(fbxManager, fbxScene, fbxPath);

    fbxScene->Destroy();
    fbxManager->Destroy();
    ClearAllExportCaches();

    return success;
}

}; //ns DAVA
