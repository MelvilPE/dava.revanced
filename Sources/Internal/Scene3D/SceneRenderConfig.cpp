#include "SceneRenderConfig.h"

#include "Reflection/ReflectionRegistrator.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(SceneRenderConfig)
{
    ReflectionRegistrator<SceneRenderConfig>::Begin()
    .End();
}
}; // namespace DAVA
