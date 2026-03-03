#pragma once
#ifndef __DAVAENGINE_SCENERENDERCONFIG_H__
#define __DAVAENGINE_SCENERENDERCONFIG_H__

#include "Scene3D/ArchiveDataNode.h"

#include "MemoryManager/MemoryProfiler.h"

namespace DAVA
{
class Entity;
class Scene;
class DataNode;
class SerializationContext;

class SceneRenderConfig : public ArchiveDataNode
{
    DAVA_ENABLE_CLASS_ALLOCATION_TRACKING(ALLOC_POOL_SCENERENDERCONFIG)

public:
    DAVA_VIRTUAL_REFLECTION(SceneRenderConfig, ArchiveDataNode);
};
}; // namespace DAVA

#endif