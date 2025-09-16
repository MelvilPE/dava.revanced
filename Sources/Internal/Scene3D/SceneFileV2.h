#ifndef __DAVAENGINE_SCENEFILEV2_H__
#define __DAVAENGINE_SCENEFILEV2_H__

#include "Base/BaseMath.h"
#include "Base/BaseObject.h"
#include "FileSystem/File.h"
#include "Render/3D/PolygonGroup.h"
#include "Render/3D/StaticMesh.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Scene3D/SceneFile/VersionInfo.h"
#include "Utils/Utils.h"

namespace DAVA
{
/*
     Scene serialization thoughts:

     There are 2 types of nodes:
     1. Data Object
     - Texture - WILL BE REMOVED
     - Material - textures is instanced by names
     - Polygroup - group of polygons
     - Static Mesh - several polygroups combined
     - Animated Mesh - mesh with weights and bones heirarhy information
     - Animations - animation for the specific mesh / bones
     - Tiled Textures - in FUTURE for dynamic loading of landscapes when traveling over the world

     2. Hierarchy Node

     * Object nodes
     - Light - light in the scene
     - Camera - camera in the scene
     - MeshInstance - instance of the specific mesh in the scene
     - AnimatedMeshInstance - instance of the specific animated mesh in the scene / animated mesh instances should store data inside.
     - Landscape - terrain object
     - Skeleton - root of the skeletal animated mesh
     - Bone - node for skeletal animation
     - Dummy - dummy object that can be used for additional client logic
     - ParticleEmitter - emitter node
     - Occluder - for occlusion culling

     * Clusterization nodes
     - BSPTree - tbd later
     - QuadTree - tbd later
     - OctTree - tbd later
     - Portal - tbd later

     General thoughts
     - Format should support uploading of data nodes on the fly
     - Format should support versioning for nodes
     - Keyed archive


    3. Required types
    -   int32, float32,
    -   Vector2, Vector3, Vector4
    -   Matrix3, Matrix4
    -   Quaternion


     Scene * scene = ...;
     scene->Load("filename
*/

class NMaterial;
class Scene;

class SceneArchive : public BaseObject
{
public:
    struct SceneArchiveHierarchyNode : public BaseObject
    {
        KeyedArchive* archive;
        Vector<SceneArchiveHierarchyNode*> children;
        SceneArchiveHierarchyNode();
        bool LoadHierarchy(File* file);

    protected:
        ~SceneArchiveHierarchyNode();
    };

    Vector<SceneArchiveHierarchyNode*> children;
    Vector<KeyedArchive*> dataNodes;

protected:
    ~SceneArchive();
};

class SceneFileV2 : public BaseObject
{
private:
    struct Header
    {
        Header()
            : version(0)
            , nodeCount(0) {};

        char signature[4];
        int32 version;
        int32 nodeCount;
    };

protected:
    virtual ~SceneFileV2();

public:
    enum eError
    {
        ERROR_NO_ERROR = 0,
        ERROR_VERSION_IS_TOO_OLD,
        ERROR_FAILED_TO_CREATE_FILE,
        ERROR_FILE_WRITE_ERROR,
        ERROR_FILE_READ_ERROR,
        ERROR_VERSION_TAGS_INVALID,
    };

    enum eFileType
    {
        SceneFile = 0,
        ModelFile = 1
    };

    /**
     * @brief Default constructor for the SceneFileV2 class.
     *
     * Creates a new instance of SceneFileV2 with default initialization.
     * This class is responsible for handling scene file operations in version 2 format.
     */
    SceneFileV2();

    /**
     * @brief Saves scene data to a file
     * @param[in] filename Path to file where scene should be saved
     * @param[in] _scene Scene object to save
     * @param[in] fileType Type of scene file to save (defaults to SceneFile)
     * @return Error code indicating success or failure of save operation
     */
    eError SaveScene(const FilePath& filename, Scene* _scene, SceneFileV2::eFileType fileType = SceneFileV2::SceneFile);
    /**
     * @brief Loads scene data from a file into the specified Scene object
     * @param filename Path to the scene file to be loaded
     * @param _scene Pointer to the Scene object where the loaded data will be stored
     * @return eError Error code indicating success or failure of the loading operation
     * @note Scene must be properly initialized before calling this function
     */
    eError LoadScene(const FilePath& filename, Scene* _scene);

    /**
     * @brief Export scene data for world of tanks blitz (wargaming)
     * @param[in] filename Path of the main file where scene should be saved
     * @param[in] _scene Scene object to save
     * @param[in] fileType Type of scene file to save (defaults to SceneFile)
     * @return Error code indicating success or failure of export operation
     */
    eError ExportSceneForWorldOfTanksBlitz(const FilePath& filename, Scene* _scene, SceneFileV2::eFileType fileType = SceneFileV2::SceneFile);

    /**
     * @brief Loads scene version information from a file
     * @param[in] filename Path to the scene file
     * @return Scene version information
     * @details This static function reads and returns the version information of a scene file
     * without loading the entire scene. This is useful for version compatibility checks.
     */
    static VersionInfo::SceneVersion LoadSceneVersion(const FilePath& filename);

    /**
     * @brief Enables or disables debug logging for scene file operations
     * @param[in] _isDebugLogEnabled flag to control debug log state (true to enable, false to disable)
     */
    void EnableDebugLog(bool _isDebugLogEnabled);
    /**
     * @brief Returns the current debug logging state.
     * @return true if debug logging is enabled, false otherwise.
     */
    bool DebugLogEnabled();
    /**
     * @brief Sets whether scene should be saved for game
     * @param[in] _isSaveForGame If true, scene will be saved with game-specific settings
     *
     * Controls the scene saving mode to optimize for game-specific requirements.
     * When enabled, certain scene data might be preprocessed or optimized for runtime performance.
     */
    void EnableSaveForGame(bool _isSaveForGame);

    // Material * GetMaterial(int32 index);
    // StaticMesh * GetStaticMesh(int32 index);

    // DataNode * GetNodeByPointer(uint64 pointer);

    /**
     * @brief Sets the error state for the SceneFileV2
     * @param[in] error The error state to set, defined by eError enumeration
     */
    void SetError(eError error);
    /**
     * Gets the current error state.
     * @return Error code indicating the current error state of the scene file.
     */
    eError GetError() const;

    /**
     * @brief Performs optimization on the scene hierarchy starting from the given root node
     * @details This function optimizes the scene by performing various operations such as
     *          mesh combining, redundant node removal, and other performance improvements
     * @param[in] rootNode Pointer to the root entity of the scene to be optimized
     */
    void OptimizeScene(Entity* rootNode);
    /**
     * @brief Removes empty hierarchy nodes from the scene graph starting from the specified node
     *
     * @param currentNode The entity node to start checking for empty hierarchy from
     * @return true if the current node was removed (had no children and no components)
     * @return false if the current node was preserved (had children or components)
     */
    /**
     * @brief Removes empty hierarchies (nodes without any visible content) from the scene tree
     * @param currentNode The root node to start checking for empty hierarchies
     * @return true if the current node is empty and can be removed, false otherwise
     *
     * This method recursively traverses the scene tree starting from the given node and removes
     * nodes that don't contain any meaningful content (empty hierarchies). A node is considered
     * empty if it has no components, no children, or if all its children are empty.
     */
    bool RemoveEmptyHierarchy(Entity* currentNode);
    /**
     * @brief Rebuilds tangent space for the given entity
     *
     * Recalculates tangent space vectors (tangents and binormals) for the mesh data
     * of the specified entity. This is typically used when mesh data has been modified
     * and normal mapping requires updated tangent space information.
     *
     * @param[in,out] entity Pointer to the Entity whose tangent space needs to be rebuilt
     */
    void RebuildTangentSpace(Entity* entity);
    /**
     * @brief Converts shadow volumes for entities in the scene.
     *
     * This function processes the scene hierarchy starting from the root node and converts
     * shadow volumes according to the provided shadow material parent configuration.
     *
     * @param[in] rootNode Pointer to the root entity node where shadow volume conversion should start
     * @param[in] shadowMaterialParent Pointer to the parent material used for shadow rendering
     */
    void ConvertShadowVolumes(Entity* rootNode, NMaterial* shadowMaterialParent);
    /**
     * @brief Removes deprecated material flags from the entity and its children.
     *
     * Traverses through the entity hierarchy starting from the given root node and removes
     * any deprecated material flags that may exist in the materials of the entities.
     *
     * @param[in] rootNode Pointer to the root entity from which to start the removal process.
     *                     The process will affect this entity and all its descendants.
     */
    void RemoveDeprecatedMaterialFlags(Entity* rootNode);
    /**
     * @brief Converts alphatest value materials for all materials in the entity hierarchy
     *
     * @details This function traverses through the entity tree starting from the root node
     * and processes materials that use alphatest. It converts legacy alphatest value
     * materials to ensure compatibility with current material system.
     *
     * @param[in] rootNode Pointer to the root entity from which to start the conversion
     */
    void ConvertAlphatestValueMaterials(Entity* rootNode);
    int32 removedNodeCount = 0;

    /**
     * @brief Recursively updates the requested format of polygon groups for an entity and its children
     * @param entity The root entity from which to start the recursive format update
     *
     * This function traverses through the entity hierarchy starting from the given entity
     * and updates the polygon group requested format for each entity encountered in the process.
     * The update is applied to the current entity and propagates down to all its child entities.
     */
    void UpdatePolygonGroupRequestedFormatRecursively(Entity* entity);
    /**
     * @brief Loads scene archive data from a file
     * @param filename Path to the scene archive file
     * @return Pointer to the loaded SceneArchive object, or nullptr if loading fails
     * @details This function only loads raw data from the scene archive without any processing
     */
    SceneArchive* LoadSceneArchive(const FilePath& filename); // purely load data

    String GetSceneComponents();
    String GetSceneComponentSets();
    String GetSceneRenderConfig();

private:
    /**
     * @brief Reads the header information from a scene file
     * @param[out] header Reference to a Header structure where the read data will be stored
     * @param[in] file Pointer to the File object to read from
     * @return true if header was successfully read, false otherwise
     */
    static bool ReadHeader(Header& header, File* file);
    /**
     * @brief Reads the header information from a geometry file
     * @param[out] header Reference to a Header structure where the read data will be stored
     * @param[in] file Pointer to the File object to read from
     * @return true if header was successfully read, false otherwise
     */
    static bool ReadGeometryFileHeader(Header& header, File* file);
    /**
     * @brief Reads version tags from a scene file
     * @param[out] version Reference to SceneVersion structure where version info will be stored
     * @param[in] file Pointer to the file to read from
     * @return true if version tags were successfully read, false otherwise
     */
    static bool ReadVersionTags(VersionInfo::SceneVersion& version, File* file);
    /**
     * @brief Adds the specified data node to the node mapping.
     *
     * @param[in] node Pointer to the DataNode to be added to the map.
     *                 The node must be a valid pointer and must not be null.
     */
    void AddToNodeMap(DataNode* node);

    Header header;

    struct Descriptor
    {
        uint32 size = 0;
        uint32 fileType = 0;
        uint64 geometryIdHash = 0;
        uint64 geometryDataHash = 0;
    };
    Descriptor descriptor;

    // Vector<StaticMesh*> staticMeshes;

    /**
     * @brief Saves the data hierarchy for a node to a file
     *
     * @param node The data node to save the hierarchy from
     * @param file The file to save the hierarchy to
     * @param level The current level in the hierarchy
     * @return true if the save operation was successful
     * @return false if there was an error during the save operation
     */
    bool SaveDataHierarchy(DataNode* node, File* file, int32 level);
    /**
     * @brief Loads hierarchical data structure from a file into the scene
     * @param scene Pointer to the Scene object where data will be loaded
     * @param node Pointer to the DataNode representing current node in hierarchy
     * @param file Pointer to the File object containing scene data
     * @param level Current depth level in the hierarchy
     *
     * This function recursively loads and processes hierarchical data structure
     * from the specified file into the scene. The data is organized in a tree-like
     * structure where each node can have multiple child nodes.
     */
    void LoadDataHierarchy(Scene* scene, DataNode* node, File* file, int32 level);
    /**
     * @brief Saves a data node to a file.
     *
     * @param[in] node Pointer to the DataNode to be saved
     * @param[in] file Pointer to the File where the data will be saved
     * @return true if the save operation was successful, false otherwise
     */
    bool SaveDataNode(DataNode* node, File* file);
    /**
     * @brief Loads a data node from a file and adds it to the scene hierarchy
     * @param scene Pointer to the Scene where the data node will be loaded
     * @param file Pointer to the File containing the node data to be loaded
     * @return true if the data node was successfully loaded, false otherwise
     */
    bool LoadDataNode(Scene* scene, File* file);

    bool LoadDataNodeInternal(Scene* scene, KeyedArchive* archive, uint32 currFilePos = 0);

    bool LoadHierarchyFromArchive(Scene* scene, Entity* parent, KeyedArchive* archive);

    /**
     * @brief Checks if a DataNode can be serialized
     *
     * Determines whether a DataNode is serializable by checking if it's not a runtime node.
     * Runtime nodes (such as ShadowVolume materials) are not serializable.
     *
     * @param[in] node Pointer to the DataNode to check
     * @return true if the node is serializable, false otherwise
     */
    inline bool IsDataNodeSerializable(DataNode* node)
    {
        // VI: runtime nodes (such as ShadowVolume materials are not serializable)
        return (!node->IsRuntime());
    }

    /**
     * @brief Checks if the DataNode is a PolygonGroup
     *
     * @param[in] node Pointer to the DataNode to check
     * @return true if the node is a PolygonGroup, false otherwise
     */
    inline bool IsPolygonGroupDataNode(DataNode* node)
    {
        return (node->GetClassName() == "PolygonGroup");
    }

    /**
     * @brief Saves the hierarchy of entities to a file
     * @param node The root entity node to save
     * @param file The file to write the hierarchy to
     * @param level The current depth level in the hierarchy
     * @return true if hierarchy was saved successfully, false otherwise
     */
    bool SaveHierarchy(Entity* node, File* file, int32 level);

    void ExportHierarchyForWorldOfTanksBlitz(Vector<VariantType>* hierarchy, Entity* node);

    bool GetNestedParticleEmitterNodes(Entity* entity, Vector<VariantType>* result);

    bool LoadHierarchy(Scene* scene, Entity* node, File* file, int32 level);

    /**
     * @brief Fixes the Level of Detail (LOD) settings for entities using LOD System 2
     * @param[in] entity Pointer to the Entity whose LOD settings need to be fixed
     * @details This function adjusts and corrects LOD (Level of Detail) configuration
     *          for the specified entity when using LOD System 2. This ensures proper
     *          LOD behavior and compatibility with the updated LOD system.
     */
    void FixLodForLodsystem2(Entity* entity);

    /**
     * @brief Loads an Entity from an archive into the specified scene
     * @param scene Pointer to the Scene where the Entity will be loaded
     * @param archive Pointer to the KeyedArchive containing serialized Entity data
     * @return Pointer to the loaded Entity, or nullptr if loading fails
     */
    Entity* LoadEntity(Scene* scene, KeyedArchive* archive);
    /**
     * @brief Loads a landscape entity from an archive into the scene
     * @param scene Pointer to the Scene object where the landscape will be loaded
     * @param archive Pointer to the KeyedArchive containing landscape data
     * @return Pointer to the loaded landscape Entity. Returns nullptr if loading fails
     */
    Entity* LoadLandscape(Scene* scene, KeyedArchive* archive);
    /**
     * @brief Loads a camera Entity from an archive into the specified scene
     * @param scene Pointer to the Scene where camera should be loaded
     * @param archive Pointer to KeyedArchive containing camera data
     * @return Pointer to created camera Entity, nullptr if loading failed
     */
    Entity* LoadCamera(Scene* scene, KeyedArchive* archive);
    /**
     * @brief Loads a light entity from an archive into the specified scene
     * @param[in] scene Pointer to the Scene where the light will be loaded
     * @param[in] archive Pointer to the KeyedArchive containing light data
     * @return Pointer to the created light Entity, or nullptr if loading failed
     */
    Entity* LoadLight(Scene* scene, KeyedArchive* archive);

    /**
     * @brief Applies fog quality settings to the specified material
     * @param material Pointer to the material instance where fog quality will be applied
     *
     * @details This function modifies the material's fog-related properties based on
     * current quality settings. The material must be a valid NMaterial instance.
     */
    void ApplyFogQuality(DAVA::NMaterial* material);

    /**
     * @brief Prepare and writes the descriptor data to a file
     * @param[in] file Pointer to the file where descriptor will be written
     * @param[in] descriptor The descriptor data to write
     * @return true if writing was successful, false otherwise
     */
    static bool PrepareAndWriteDescriptor(File* file, Descriptor& descriptor, SerializationContext* serializationContext);
    /**
     * Reads a scene descriptor from a file.
     * @param[in] file The file to read the descriptor from.
     * @param[out] descriptor The descriptor object where the read data will be stored.
     * @return Returns true if the descriptor was successfully read, false otherwise.
     */
    static bool ReadDescriptor(File* file, Descriptor& descriptor);

    bool isDebugLogEnabled;
    bool isSaveForGame;
    eError lastError;

    String sceneComponents = "";
    String sceneComponentSets = "";
    String sceneRenderConfig = "";

    SerializationContext serializationContext;
};

class SceneFileV2Key
{
public:
    static const String DATANODES_KEY;
    static const String HIERARCHY_KEY;
    static const String SCENE_COMPONENTS_KEY;
    static const String SCENE_COMPONENT_SETS_KEY;
};

}; // namespace DAVA

#endif // __DAVAENGINE_SCENEFILEV2_H__
