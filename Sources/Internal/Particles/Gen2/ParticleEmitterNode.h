#pragma once
#ifndef __DAVAENGINE_PARTICLEEMITTERNODE_H__
#define __DAVAENGINE_PARTICLEEMITTERNODE_H__

#include "Scene3D/Entity.h"
#include "Scene3D/Scene.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Scene3D/ArchiveDataNode.h"
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

class ParticleEmitterNode : public ArchiveDataNode
{
    DAVA_ENABLE_CLASS_ALLOCATION_TRACKING(ALLOC_POOL_PARTICLEEMITTERNODE)

public:
    void GetMeshGeometryIds(Vector<uint64>& output);
    void UpdateMeshGeometryIds(SerializationContext* serializationContext);

    bool IsClone();

    uint64 GetReferenceId();
    void SetReferenceId(uint64 id);

    DAVA_VIRTUAL_REFLECTION(ParticleEmitterNode, ArchiveDataNode);

private:
    void GetMeshGeometryIdsRecursively(Vector<uint64>& output, KeyedArchive* archive);
    void UpdateMeshGeometryIdsRecursively(KeyedArchive* archive, SerializationContext* serializationContext);
};
}; // namespace DAVA

#endif