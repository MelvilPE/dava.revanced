#pragma once
#ifndef __DAVAENGINE_PARTICLEEMITTERNODE_H__
#define __DAVAENGINE_PARTICLEEMITTERNODE_H__

#include "Scene3D/Entity.h"
#include "Scene3D/Scene.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Scene3D/DataNode.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Render/Shader.h"

#include <memory>

#include "MemoryManager/MemoryProfiler.h"

namespace DAVA
{
class Entity;
class Scene;
class DataNode;
class SerializationContext;

class ParticleEmitterNode : public DataNode
{
    DAVA_ENABLE_CLASS_ALLOCATION_TRACKING(ALLOC_POOL_PARTICLEEMITTERNODE)

public:
    ParticleEmitterNode();
    ~ParticleEmitterNode();

    void Load(KeyedArchive* archive, SerializationContext* serializationContext) override;
    void Save(KeyedArchive* archive, SerializationContext* serializationContext) override;

    uint64 GetParticleEmitterNodeID();

    DAVA_VIRTUAL_REFLECTION(ParticleEmitterNode, DataNode);

    String GetNodeYaml();
    void SetNodeYaml(String value);

private:
    String nodeYaml;
};
}; // namespace DAVA

#endif