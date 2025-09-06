#ifndef __DAVAENGINE_SCENE3D_ACTIONUPDATESYSTEM_H__
#define __DAVAENGINE_SCENE3D_ACTIONUPDATESYSTEM_H__

#include "Base/BaseTypes.h"
#include "Entity/SceneSystem.h"
#include "Scene3D/Components/ActionComponent.h"

namespace DAVA
{
class ActionComponent;
class ActionUpdateSystem : public SceneSystem
{
public:
    /**
     * @brief System for managing and updating actions in the 3D scene.
     * @param scene Pointer to the Scene object this system will operate on.
     * 
     * A system responsible for handling the lifecycle and execution of actions
     * within a 3D scene. Actions can be animations, transformations, or any
     * time-based operations that need to be updated each frame.
     */
    ActionUpdateSystem(Scene* scene);
    /**
     * @brief Processes system logic for the specified time period
     * @param timeElapsed Time elapsed since the last frame in seconds
     * 
     * This method is called every frame to update the action system state.
     * Inherits and overrides Process method from the base system class.
     */
    void Process(float32 timeElapsed) override;

    /**
     * @brief Adds an entity to the action update system
     * @param entity A pointer to the Entity to be added to the system
     * @note This method overrides the base class implementation
     */
    void AddEntity(Entity* entity) override;
    /**
     * @brief Removes an entity from the system.
     * @details The entity will be removed from action update system processing.
     * @param entity Pointer to the entity to be removed.
     */
    void RemoveEntity(Entity* entity) override;
    /**
     * @brief Prepares the system for removal from the scene
     * 
     * This method is called before the system is removed from the scene to perform any necessary cleanup.
     * Override of the base class virtual method.
     */
    void PrepareForRemove() override;

    /**
     * @brief Adds the specified ActionComponent to the watch list for updates
     * 
     * @param component Pointer to the ActionComponent that needs to be monitored
     * @throws none
     * 
     * @details This method registers an ActionComponent to be watched by the ActionUpdateSystem.
     * Once watched, the component will receive updates during the system's update cycle.
     */
    void Watch(ActionComponent* component);
    /**
     * Stops watching/tracking the specified action component.
     * Removes the component from the system's observation list.
     * 
     * @param[in] component Pointer to the ActionComponent to stop watching
     */
    void UnWatch(ActionComponent* component);

    /**
     * Sets the blocking state for a specific action event type.
     * @param eventType The type of action event to be blocked/unblocked
     * @param block If true, blocks the specified event type; if false, unblocks it
     */
    void SetBlockEvent(ActionComponent::Event::eType eventType, bool block);
    /**
     * @brief Checks if the specified event type is a blocking event
     * @param eventType The type of event to check
     * @return true if the event is blocking, false otherwise
     */
    bool IsBlockEvent(ActionComponent::Event::eType eventType);
    /**
     * @brief Unblocks all events in the system that were previously blocked
     * 
     * This method removes any blocking state from all events, allowing them to be processed normally.
     * Use this method to resume normal event processing after events have been blocked.
     */
    void UnblockAllEvents();

protected:
    bool eventBlocked[ActionComponent::Event::eType::Count];
    Vector<ActionComponent*> activeActions;

    /**
     * @brief Handles the deferred removal of actions that are marked for deletion.
     * 
     * This function processes and removes actions that have been queued for deletion,
     * ensuring thread-safe cleanup of action objects from the system.
     */
    void DelayedDeleteActions();
    Vector<ActionComponent*> deleteActions;
};
}

#endif //__DAVAENGINE_SCENE3D_ACTIONUPDATESYSTEM_H__