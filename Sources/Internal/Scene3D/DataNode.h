#pragma once

#include "Base/BaseObject.h"
#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Reflection/Reflection.h"
#include "Render/RenderBase.h"
#include "Scene3D/Entity.h"
#include "Scene3D/SceneFile/SerializationContext.h"

namespace DAVA
{
/**
    
 */
class DataNode : public BaseObject
{
public:
    static const uint64 INVALID_ID = 0;

public:
    /**
     * @brief Default constructor
     */
    DataNode();

    /**
     * @brief Set the scene this node belongs to
     * @param _scene Pointer to Scene
     */
    void SetScene(Scene* _scene);

    /**
     * @brief Get the scene this node belongs to
     * @return Pointer to Scene
     */
    Scene* GetScene() const;

    /**
     * @brief Find child node by name
     * @param searchName Name to search for
     * @return Pointer to found DataNode or nullptr
     */
    DataNode* FindByName(const String& searchName);

    /**
     * @brief Get node identifier
     * @return Node identifier
     */
    virtual uint64 GetNodeID() const;

    /**
     * @brief Set node identifier
     * @param id Identifier to set
     */
    virtual void SetNodeID(uint64 id);

    /**
     * @brief Set runtime flag
     * @param isRuntime Runtime flag value
     */
    void SetRuntime(bool isRuntime);

    /**
     * @brief Check if node is runtime
     * @return true if runtime node
     */
    bool IsRuntime() const;

    /**
     * @brief Save node to archive
     * @param archive Archive to save to
     * @param serializationContext Serialization context
     */
    virtual void Save(KeyedArchive* archive, SerializationContext* serializationContext);

    /**
     * @brief Load node from archive
     * @param archive Archive to load from
     * @param serializationContext Serialization context
     */
    virtual void Load(KeyedArchive* archive, SerializationContext* serializationContext);

protected:
    virtual ~DataNode();

    uint64 id;
    bool isRuntime;
    Scene* scene;

    DAVA_VIRTUAL_REFLECTION(DataNode, BaseObject);
};
}
