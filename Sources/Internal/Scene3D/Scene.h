#pragma once

#include "Base/BaseMath.h"
#include "Base/BaseTypes.h"
#include "Base/Observer.h"
#include "Entity/SceneSystem.h"
#include "Entity/SingletonComponent.h"
#include "Render/Highlevel/Camera.h"
#include "Render/Highlevel/Light.h"
#include "Reflection/Reflection.h"
#include "Render/RenderBase.h"
#include "Scene3D/Entity.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Scene3D/SceneFile/VersionInfo.h"
#include "Scene3D/SceneFileV2.h"

namespace DAVA
{
/**
    \defgroup scene3d 3D Engine
  */

class Texture;
class StaticMesh;
class DataNode;
class ShadowVolumeNode;
class Light;
class ShadowRect;
class QuadTree;
class Component;
class RenderSystem;
class RenderUpdateSystem;
class TransformSystem;
class DebugRenderSystem;
class EventSystem;
class ParticleEffectSystem;
class UpdateSystem;
class LightUpdateSystem;
class SwitchSystem;
class SoundUpdateSystem;
class ActionUpdateSystem;
class StaticOcclusionSystem;
class StaticOcclusionDebugDrawSystem;
class SpeedTreeUpdateSystem;
class FoliageSystem;
class WindSystem;
class WaveSystem;
class SkeletonSystem;
class MotionSystem;
class AnimationSystem;
class LandscapeSystem;
class LodSystem;
class ParticleEffectDebugDrawSystem;
class GeoDecalSystem;
class SlotSystem;
class TransformSingleComponent;
class MotionSingleComponent;
class PhysicsSystem;
class CollisionSingleComponent;

class UIEvent;
class RenderPass;

/**
    \ingroup scene3d
    \brief This class is a code of our 3D Engine scene graph. 
    To visualize any 3d scene you'll need to create Scene object. 
    Scene have visible hierarchy and invisible root nodes. You can add as many root nodes as you want, and do not visualize them.
    For example you can have multiple scenes, load them to one scene, and show each scene when it will be required. 
 */
class EntityCache
{
public:
    ~EntityCache();

    /**
     * @brief Preloads scene assets from the specified file path
     * 
     * This method initiates the preloading process for scene-related resources
     * located at the given path. Preloading helps optimize resource management
     * and improve loading times during actual scene initialization.
     * 
     * @param[in] path The file path where the scene assets are located
     */
    void Preload(const FilePath& path);
    /**
     * @brief Clears the scene associated with the specified file path
     * 
     * @param path The file path associated with the scene to be cleared
     * 
     * @details This method removes all resources, entities, and components 
     * associated with the scene at the specified path, effectively resetting 
     * it to an empty state.
     */
    void Clear(const FilePath& path);
    /**
     * Clears all resources and components from the scene.
     * This method removes all entities, components, lights, cameras,
     * and resets the scene to its initial empty state.
     */
    void ClearAll();

    /**
     * @brief Retrieves the original entity associated with the specified file path.
     * 
     * @param path The file path to the entity resource
     * @return Entity* Pointer to the original entity if found, nullptr otherwise
     */
    Entity* GetOriginal(const FilePath& path);
    /**
     * @brief Creates a clone of an entity from a given file path.
     * @param[in] path Path to the original entity file
     * @return Pointer to the newly created clone entity, or nullptr if creation failed
     */
    Entity* GetClone(const FilePath& path);

protected:
    Map<FilePath, Entity*> cachedEntities;
};

class Scene : public Entity, Observer
{
protected:
    virtual ~Scene();

public:
    enum : uint32
    {
        SCENE_SYSTEM_TRANSFORM_FLAG = 1 << 0,
        SCENE_SYSTEM_RENDER_UPDATE_FLAG = 1 << 1,
        SCENE_SYSTEM_LOD_FLAG = 1 << 2,
        SCENE_SYSTEM_DEBUG_RENDER_FLAG = 1 << 3,
        SCENE_SYSTEM_PARTICLE_EFFECT_FLAG = 1 << 4,
        SCENE_SYSTEM_UPDATEBLE_FLAG = 1 << 5,
        SCENE_SYSTEM_LIGHT_UPDATE_FLAG = 1 << 6,
        SCENE_SYSTEM_SWITCH_FLAG = 1 << 7,
        SCENE_SYSTEM_SOUND_UPDATE_FLAG = 1 << 8,
        SCENE_SYSTEM_ACTION_UPDATE_FLAG = 1 << 9,
        SCENE_SYSTEM_STATIC_OCCLUSION_FLAG = 1 << 11,
        SCENE_SYSTEM_LANDSCAPE_FLAG = 1 << 12,
        SCENE_SYSTEM_FOLIAGE_FLAG = 1 << 13,
        SCENE_SYSTEM_SPEEDTREE_UPDATE_FLAG = 1 << 14,
        SCENE_SYSTEM_WIND_UPDATE_FLAG = 1 << 15,
        SCENE_SYSTEM_WAVE_UPDATE_FLAG = 1 << 16,
        SCENE_SYSTEM_SKELETON_FLAG = 1 << 17,
        SCENE_SYSTEM_ANIMATION_FLAG = 1 << 18,
        SCENE_SYSTEM_SLOT_FLAG = 1 << 19,
        SCENE_SYSTEM_MOTION_FLAG = 1 << 20,
        SCENE_SYSTEM_GEO_DECAL_FLAG = 1 << 21,

#if defined(__DAVAENGINE_PHYSICS_ENABLED__)
        SCENE_SYSTEM_PHYSICS_FLAG = 1 << 19,
#endif
        SCENE_SYSTEM_ALL_MASK = 0xFFFFFFFF
    };

    enum eSceneProcessFlags : uint32
    {
        SCENE_SYSTEM_REQUIRE_PROCESS = 1 << 0,
        SCENE_SYSTEM_REQUIRE_INPUT = 1 << 1,
        SCENE_SYSTEM_REQUIRE_FIXED_PROCESS = 1 << 2
    };

    Scene(uint32 systemsMask = SCENE_SYSTEM_ALL_MASK);

    /**
        \brief Function to register entity in scene. This function is called when you add entity to scene.
     */
    void RegisterEntity(Entity* entity);
    /**
        \brief Function to unregister entity from scene. This function is called when you remove entity from scene.
     */
    void UnregisterEntity(Entity* entity);

    /**
        \brief Function to register component in scene. This function is called when you add any component to any entity in scene.
     */
    void RegisterComponent(Entity* entity, Component* component);
    /**
        \brief Function to unregister component from scene. This function is called when you remove any component from any entity in scene.
     */
    void UnregisterComponent(Entity* entity, Component* component);

    /**
     * @brief Adds a scene system to the scene with specified component mask and processing flags
     * 
     * @param sceneSystem The scene system to be added
     * @param componentMask Mask specifying which components this system should process
     * @param processFlags Flags defining when the system should be processed (default: 0)
     * @param insertBeforeSceneForProcess System before which this system should be processed (nullptr for end of list)
     * @param insertBeforeSceneForInput System before which this system should process input (nullptr for end of list)
     * @param insertBeforeSceneForFixedProcess System before which this system should be processed in fixed update (nullptr for end of list)
     * 
     * @details This method allows to insert a scene system into the scene's processing pipeline.
     * The system will only process entities that match the provided component mask.
     * The order of processing can be controlled by specifying which existing systems the new system should be inserted before.
     */
    virtual void AddSystem(SceneSystem* sceneSystem, const ComponentMask& componentMask, uint32 processFlags = 0, SceneSystem* insertBeforeSceneForProcess = nullptr, SceneSystem* insertBeforeSceneForInput = nullptr, SceneSystem* insertBeforeSceneForFixedProcess = nullptr);
    /**
     * @brief Removes specified system from the scene.
     * @param[in] sceneSystem Pointer to the scene system to be removed.
     * @note The ownership of the system is not transferred, caller is responsible for memory management of the removed system.
     */
    virtual void RemoveSystem(SceneSystem* sceneSystem);
    template <class T>
    /**
     * @brief Gets the system of specified type from the Scene.
     * @tparam T Type of the system to retrieve
     * @return Pointer to the system of type T, or nullptr if not found
     */
    T* GetSystem();

    Vector<SceneSystem*> systems;
    Vector<SceneSystem*> systemsToProcess;
    Vector<SceneSystem*> systemsToInput;
    Vector<SceneSystem*> systemsToFixedProcess;

    TransformSystem* transformSystem = nullptr;
    RenderUpdateSystem* renderUpdateSystem = nullptr;
    LodSystem* lodSystem = nullptr;
    DebugRenderSystem* debugRenderSystem = nullptr;
    EventSystem* eventSystem = nullptr;
    ParticleEffectSystem* particleEffectSystem = nullptr;
    UpdateSystem* updatableSystem = nullptr;
    LightUpdateSystem* lightUpdateSystem = nullptr;
    SwitchSystem* switchSystem = nullptr;
    RenderSystem* renderSystem = nullptr;
    SoundUpdateSystem* soundSystem = nullptr;
    ActionUpdateSystem* actionSystem = nullptr;
    StaticOcclusionSystem* staticOcclusionSystem = nullptr;
    SpeedTreeUpdateSystem* speedTreeUpdateSystem = nullptr;
    FoliageSystem* foliageSystem = nullptr;
    VersionInfo::SceneVersion version;
    WindSystem* windSystem = nullptr;
    WaveSystem* waveSystem = nullptr;
    AnimationSystem* animationSystem = nullptr;
    StaticOcclusionDebugDrawSystem* staticOcclusionDebugDrawSystem = nullptr;
    SkeletonSystem* skeletonSystem = nullptr;
    MotionSystem* motionSystem = nullptr;
    LandscapeSystem* landscapeSystem = nullptr;
    ParticleEffectDebugDrawSystem* particleEffectDebugDrawSystem = nullptr;
    SlotSystem* slotSystem = nullptr;
    GeoDecalSystem* geoDecalSystem = nullptr;
    PhysicsSystem* physicsSystem = nullptr;

    CollisionSingleComponent* collisionSingleComponent = nullptr;
    TransformSingleComponent* transformSingleComponent = nullptr;
    MotionSingleComponent* motionSingleComponent = nullptr;

    /**
     * @brief Adds a singleton component to the scene
     * 
     * @param[in] component Pointer to the singleton component to be added
     * 
     * @details A singleton component is a special type of component that can only have one instance
     * in the scene. If a component of the same type already exists, it should be handled according
     * to the singleton pattern implementation.
     */
    void AddSingletonComponent(SingletonComponent* component);
    template <class T>
    /**
     * @brief Gets the singleton component of type T in the scene
     * @tparam T The type of component to retrieve
     * @return Pointer to the singleton component of type T, or nullptr if not found
     * @note Only one component of type T can exist in the scene at a time
     */
    T* GetSingletonComponent();
    /**
     * @brief Removes a singleton component from the scene.
     * 
     * @param component Pointer to the singleton component to be removed.
     * @note The component pointer must be valid and the component must be already added to the scene.
     */
    void RemoveSingletonComponent(SingletonComponent* component);
    Vector<SingletonComponent*> singletonComponents;

    /**
        \brief Overloaded GetScene returns this, instead of normal functionality.
     */
    Scene* GetScene() override;

    /**
     * @brief Handles RenderOptions event from an observable object
     * @param observable Pointer to the Observable object that triggered the event
     * @override Overrides base class event handler
     */
    void HandleEvent(Observable* observable) override; //Handle RenderOptions

    //virtual void StopAllAnimations(bool recursive = true);

    /**
     * @brief Updates the scene with the given time elapsed since last frame.
     * 
     * This virtual method is called every frame to update the scene state, including all scene objects,
     * components, animations, and other time-dependent elements.
     *
     * @param[in] timeElapsed Time elapsed since the last frame in seconds
     */
    virtual void Update(float32 timeElapsed);
    /**
     * @brief Renders the current scene.
     *
     * This virtual function is responsible for drawing all visible objects in the scene.
     * It handles the rendering pipeline including all registered cameras, lights, and renderable entities.
     * Derived classes can override this method to implement custom rendering behavior.
     */
    virtual void Draw();
    /**
     * @brief Called when scene loading is complete
     * 
     * This callback method is triggered after the scene and all its resources have been fully loaded
     * into memory. It provides a hook for performing any necessary post-loading initialization
     * or setup operations.
     */
    void SceneDidLoaded() override;

    /**
     * @brief Gets the camera at the specified index from the scene
     * @param n The index of the camera to retrieve
     * @return Pointer to the Camera object at the specified index, or nullptr if index is invalid
     */
    Camera* GetCamera(int32 n);
    /**
     * Adds a camera to the scene.
     * @param[in] c Camera object to be added to the scene.
     * @note The scene does not take ownership of the camera. The caller is responsible for managing the camera's lifetime.
     */
    void AddCamera(Camera* c);
    /**
     * Removes specified camera from the scene
     * @param[in] c Camera to be removed from the scene
     * @return true if camera was successfully removed, false otherwise
     */
    bool RemoveCamera(Camera* c);
    /**
    * @brief Returns the total number of cameras in the scene
    * @return Number of cameras as a signed 32-bit integer
    */
    inline int32 GetCameraCount();

    /**
     * @brief Sets the currently active camera for the scene
     * @param camera Pointer to the Camera object to be set as current. Can be nullptr
     * @note Setting camera to nullptr will result in no camera being active in the scene
     */
    void SetCurrentCamera(Camera* camera);
    /**
     * @brief Returns the currently active camera in the scene
     * @return Pointer to the current Camera object, or nullptr if no camera is active
     */
    Camera* GetCurrentCamera() const;

    /* 
        This camera is used for visualization setup only. Most system functions use mainCamere, draw camera is used to setup matrices for render. If you do not call this function GetDrawCamera returns currentCamera. 
        You can use SetCustomDrawCamera function if you want to test frustum clipping, and view the scene from different angles.
     */
    void SetCustomDrawCamera(Camera* camera);
    /**
     * @return Current camera used for rendering the scene. Can be nullptr if no draw camera is set.
     */
    Camera* GetDrawCamera() const;

    /**
     * @brief Initializes and creates core components for the scene
     * 
     * Responsible for creating and setting up essential scene components 
     * required for proper scene functionality during initialization.
     * 
     * This method is called during scene construction to establish 
     * the basic infrastructure needed for scene operations.
     */
    void CreateComponents();
    /**
     * @brief Creates and initializes core systems required by the 3D scene.
     * 
     * This function is responsible for setting up essential systems needed for
     * scene functionality, such as rendering, physics, and other core components.
     * It should be called during scene initialization.
     */
    void CreateSystems();

    /**
     * @brief Returns a pointer to the Scene's event system.
     * @return A pointer to the EventSystem instance associated with this scene.
     */
    EventSystem* GetEventSystem() const;
    /**
    * @brief Returns the current RenderSystem instance associated with the Scene
    * @return Pointer to the RenderSystem used for rendering the Scene
    */
    RenderSystem* GetRenderSystem() const;
    /**
     * @brief Returns the animation system of the scene.
     * @return Pointer to the AnimationSystem associated with this scene. Can be nullptr if no animation system exists.
     */
    AnimationSystem* GetAnimationSystem() const;
    /**
     * @brief Returns the system responsible for debug drawing of particle effects.
     * @return Pointer to the ParticleEffectDebugDrawSystem instance.
     *         Returns nullptr if the system is not initialized.
     */
    ParticleEffectDebugDrawSystem* GetParticleEffectDebugDrawSystem() const;

    /**
     * @brief Loads a scene from the specified file path.
     * @param[in] pathname The file path to the scene file to be loaded.
     * @return SceneFileV2::eError Returns an error code indicating the result of the loading operation:
     *         - SUCCESS if the scene was loaded successfully
     *         - Other error codes depending on what went wrong during loading
     * @note This is a virtual function that can be overridden by derived classes.
     */
    virtual SceneFileV2::eError LoadScene(const DAVA::FilePath& pathname);
    /**
     * @brief Saves the current scene to a file.
     * 
     * @param pathname Path where the scene will be saved
     * @param saveForGame Optional flag indicating whether to save the scene in game-ready format (default: false)
     * 
     * @return SceneFileV2::eError Error code indicating the result of the save operation
     * 
     * @details This method serializes the current scene state into a file at the specified location.
     * When saveForGame is true, the scene is optimized for game runtime usage.
     */
    virtual SceneFileV2::eError SaveScene(const DAVA::FilePath& pathname, bool saveForGame = false);

    virtual SceneFileV2::eError ExportSceneForWorldOfTanksBlitz(const DAVA::FilePath& pathname, bool saveForGame = false);

    /**
     * @brief Performs optimization of the scene before exporting
     *
     * This virtual method prepares the scene for export by performing necessary optimizations.
     * Classes inheriting from this one can implement their specific optimization logic.
     */
    virtual void OptimizeBeforeExport();

    /**
     * @brief Returns globally applied material in current scene
     * @return Pointer to global material if exists, nullptr otherwise
     */
    DAVA::NMaterial* GetGlobalMaterial() const;
    /**
     * @brief Sets the global material for the scene
     * @param[in] globalMaterial Pointer to the material that will be used as global material.
     *                          Can be nullptr to reset global material.
     */
    void SetGlobalMaterial(DAVA::NMaterial* globalMaterial);

    /**
     * @brief Called when the scene is fully initialized and ready to use
     * 
     * This method is invoked when all scene initialization procedures are complete,
     * including resource loading and entity hierarchy setup.
     * 
     * @param rootNode Pointer to the root entity of the scene hierarchy
     */
    void OnSceneReady(Entity* rootNode);

    /**
     * Processes input events in the scene.
     * @param[in] event Pointer to the UIEvent containing input data to be processed.
     */
    void Input(UIEvent* event);
    /**
     * @brief Handler for cancelled input events in the scene
     * @details Called when an input event is cancelled during scene processing
     * 
     * @param event Pointer to the UIEvent object containing information about the cancelled input
     */
    void InputCancelled(UIEvent* event);

    /**
        \brief This functions activate and deactivate scene systems
     */
    /**
     * @brief Activates the scene, making it the currently active scene.
     * 
     * This virtual method is called when the scene becomes active. Override this method
     * to perform any initialization or setup required when the scene is activated.
     */
    virtual void Activate();
    /**
     * @brief Deactivates the current Scene. Called when another Scene becomes active.
     *        Performs cleanup and resource management tasks during Scene deactivation.
     * @details This virtual method is called during Scene state transitions,
     *          allowing derived classes to properly handle Scene deactivation events.
     */
    virtual void Deactivate();

    EntityCache cache;

    /**
     * @brief Sets the properties for the main render pass of the scene
     * @param priority Rendering priority of the main pass
     * @param viewport Viewport rectangle defining the visible area
     * @param width Width of the render target in pixels
     * @param height Height of the render target in pixels
     * @param format Pixel format to be used for the render target
     */
    void SetMainPassProperties(uint32 priority, const Rect& viewport, uint32 width, uint32 height, PixelFormat format);
    /**
     * @brief Sets up the main render target for the scene
     * @param color Color texture handle to be used as render target
     * @param depthStencil Depth stencil texture handle for depth/stencil operations
     * @param colorLoadAction Load action to perform on color buffer at the start of rendering
     * @param clearColor Color value used when clearing the render target
     */
    void SetMainRenderTarget(rhi::HTexture color, rhi::HTexture depthStencil, rhi::LoadAction colorLoadAction, const Color& clearColor);

    String GetSceneComponents();
    String GetSceneComponentSets();
    void SetSceneComponents(String value);
    void SetSceneComponentSets(String value);

public: // deprecated methods
    DAVA_DEPRECATED(rhi::RenderPassConfig& GetMainPassConfig());

protected:
    /**
     * @brief Recursively registers the specified entity and all its children in the given scene system.
     * 
     * @param system Pointer to the scene system where entities will be registered
     * @param entity Pointer to the root entity to start registration from
     * 
     * This method traverses the entity hierarchy starting from the specified entity and registers
     * each encountered entity in the provided scene system. The registration is performed in a 
     * depth-first manner.
     */
    void RegisterEntitiesInSystemRecursively(SceneSystem* system, Entity* entity);

    /**
    * @brief Removes a specified system from the given storage vector
    * @param[in,out] storage Vector containing SceneSystem pointers to search through
    * @param[in] system Pointer to the SceneSystem to be removed
    * @return true if the system was found and removed, false otherwise
    */
    bool RemoveSystem(Vector<SceneSystem*>& storage, SceneSystem* system);

    uint32 systemsMask;
    uint32 maxEntityIDCounter;

    float32 sceneGlobalTime = 0.f;

    Vector<Camera*> cameras;

    NMaterial* sceneGlobalMaterial;

    Camera* mainCamera;
    Camera* drawCamera;

    String sceneComponents = "";
    String sceneComponentSets = "";

    struct FixedUpdate
    {
        float32 constantTime = 0.016f;
        float32 lastTime = 0.f;
    } fixedUpdate;

    friend class Entity;
    DAVA_VIRTUAL_REFLECTION(Scene, Entity);
};

template <class T>
T* Scene::GetSystem()
{
    T* res = nullptr;
    const std::type_info& type = typeid(T);
    for (SceneSystem* system : systems)
    {
        const std::type_info& currType = typeid(*system);
        if (currType == type)
        {
            res = static_cast<T*>(system);
            break;
        }
    }

    return res;
}

template <class T>
T* Scene::GetSingletonComponent()
{
    T* res = nullptr;
    const std::type_info& type = typeid(T);
    for (SingletonComponent* component : singletonComponents)
    {
        const std::type_info& currType = typeid(*component);
        if (currType == type)
        {
            res = static_cast<T*>(component);
            break;
        }
    }

    return res;
}

int32 Scene::GetCameraCount()
{
    return static_cast<int32>(cameras.size());
}
};
