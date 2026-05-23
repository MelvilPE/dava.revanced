#pragma once

namespace DAVA
{
class FilePath;
class Scene;

class FBXExporter
{
public:
    // Export a full DAVA Scene to an FBX file.
    // Exports meshes (static & skinned), skeletons, materials and textures.
    static bool ExportSceneToFBX(const Scene* scene, const FilePath& fbxPath);
};
}; //ns DAVA
