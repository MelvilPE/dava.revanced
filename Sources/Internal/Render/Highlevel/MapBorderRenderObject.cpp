#include "Render/Highlevel/MapBorderRenderObject.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Render/Material/NMaterial.h"
#include "Reflection/ReflectedMeta.h"

#include "Render/Highlevel/RenderBatch.h"
#include "Base/BaseTypes.h"
#include "Render/Highlevel/RenderObject.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Render/Highlevel/RenderPassNames.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(MapBorderRenderObject)
{
    ReflectionRegistrator<MapBorderRenderObject>::Begin()
    .Field("fadeEnabled", &MapBorderRenderObject::GetFadeEnabled, &MapBorderRenderObject::SetFadeEnabled)[M::DisplayName("Fade enabled")]
    .Field("fadeNearFarSq", &MapBorderRenderObject::GetFadeNearFarSq, &MapBorderRenderObject::SetFadeNearFarSq)[M::DisplayName("Fade near far sq")]
    .End();
}

MapBorderRenderObject::~MapBorderRenderObject()
{
    DAVA_MEMORY_PROFILER_CLASS_ALLOC_SCOPE();

    SafeRelease(borderMaterial);
    Renderer::GetSignals().needRestoreResources.Disconnect(this);
}

MapBorderRenderObject::MapBorderRenderObject()
{
    DAVA_MEMORY_PROFILER_CLASS_ALLOC_SCOPE();
    fadeParams.fadeEnabled = false;
    fadeParams.fadeNearFarSq.x = 90000.0f;
    fadeParams.fadeNearFarSq.y = 160000.0f;

    type = RenderObject::eType::TYPE_MAP_BORDER;

    borderMaterial = new NMaterial();
    if (borderMaterial)
    {
        borderMaterial->SetFXName(NMaterialName::VERTEXCOLOR);
        borderMaterial->InvalidateBufferBindings();
    }

    borderMaterial->AddPreset(NMaterialPresetName::ALPHA_BLEND, true);
    borderMaterial->AddPreset(NMaterialPresetName::FORCE_AFTER_OPAQUE_LAYER, true);

    borderMaterial->SetCustomCullMode(rhi::CullMode::CULL_NONE);
    borderMaterial->InvalidateBufferBindings();

    borderMaterial->SetRuntime(true);

    flags |= RenderObject::eFlags::CUSTOM_PREPARE_TO_RENDER;
}

void MapBorderRenderObject::Load(KeyedArchive* archive, SerializationContext* serializationContext)
{
    DAVA_MEMORY_PROFILER_CLASS_ALLOC_SCOPE();

    RenderObject::Load(archive, serializationContext);

    if (archive->IsKeyExists("mro.distanceFadeEnabled"))
    {
        fadeParams.fadeEnabled = archive->GetBool("mro.distanceFadeEnabled");
    }

    if (archive->IsKeyExists("mro.distanceNearFarSq"))
    {
        fadeParams.fadeNearFarSq = archive->GetVector2("mro.distanceNearFarSq");
    }

    SetDistanceFadeParams(&fadeParams);

    for (uint32 i = 0; i < GetRenderBatchCount(); ++i)
        GetRenderBatch(i)->SetMaterial(borderMaterial);

    borderMaterial->PreBuildMaterial(PASS_FORWARD);
}

void MapBorderRenderObject::Save(KeyedArchive* archive, SerializationContext* serializationContext)
{
    DAVA_MEMORY_PROFILER_CLASS_ALLOC_SCOPE();

    RenderObject::Save(archive, serializationContext);

    archive->SetBool("mro.distanceFadeEnabled", fadeParams.fadeEnabled);

    archive->SetVector2("mro.distanceNearFarSq", fadeParams.fadeNearFarSq);
}

void MapBorderRenderObject::SetDistanceFadeParams(FadeParams* params)
{
    DAVA_MEMORY_PROFILER_CLASS_ALLOC_SCOPE();

    fadeParams = *params;

    if (fadeParams.fadeEnabled)
    {
        borderMaterial->AddFlag(NMaterialFlagName::FLAG_DISTANCE_FADE_OUT, true);

        bool hasDistance = borderMaterial->HasLocalProperty(NMaterialParamName::DISTANCE_FADE_NEAR_FAR_SQ);
        if (hasDistance)
        {
            borderMaterial->SetPropertyValue(NMaterialParamName::DISTANCE_FADE_NEAR_FAR_SQ, &fadeParams.fadeNearFarSq.x);
        }
        else
        {
            borderMaterial->AddProperty(NMaterialParamName::DISTANCE_FADE_NEAR_FAR_SQ, &fadeParams.fadeNearFarSq.x, rhi::ShaderProp::TYPE_FLOAT2, 1u); // rhi::ShaderProp::RepresentationType::RPT_UNSPECIFIED            
        }
    }
    else
    {
        bool hasFlag = borderMaterial->HasLocalFlag(NMaterialFlagName::FLAG_DISTANCE_FADE_OUT);
        if (hasFlag)
        {
            borderMaterial->RemoveFlag(NMaterialFlagName::FLAG_DISTANCE_FADE_OUT);
        }

        bool hasDistance = borderMaterial->HasLocalProperty(NMaterialParamName::DISTANCE_FADE_NEAR_FAR_SQ);
        if (hasDistance)
        {
            borderMaterial->RemoveProperty(NMaterialParamName::DISTANCE_FADE_NEAR_FAR_SQ);
        }
    }
}

bool MapBorderRenderObject::GetFadeEnabled() const
{
    return fadeParams.fadeEnabled;
}

void MapBorderRenderObject::SetFadeEnabled(bool value)
{
    DAVA_MEMORY_PROFILER_CLASS_ALLOC_SCOPE();

    fadeParams.fadeEnabled = value;
}

Vector2 MapBorderRenderObject::GetFadeNearFarSq() const
{
    return fadeParams.fadeNearFarSq;
}

void MapBorderRenderObject::SetFadeNearFarSq(Vector2 value)
{
    DAVA_MEMORY_PROFILER_CLASS_ALLOC_SCOPE();

    fadeParams.fadeNearFarSq = value;
}
}
