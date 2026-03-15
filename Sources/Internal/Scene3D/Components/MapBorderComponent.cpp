#include "Scene3D/Components/MapBorderComponent.h"
#include "Scene3D/Entity.h"
#include "Scene3D/Scene.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Reflection/ReflectedMeta.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(MapBorderComponent::VisualParameters)
{
    ReflectionRegistrator<MapBorderComponent::VisualParameters>::Begin()
    .ConstructorByPointer()
    .Field("height", &MapBorderComponent::VisualParameters::height)[M::DisplayName("Height")]
    .Field("zOffset", &MapBorderComponent::VisualParameters::zOffset)[M::DisplayName("Z Offset")]
    .Field("minZ", &MapBorderComponent::VisualParameters::minZ)[M::DisplayName("Min Z")]
    .Field("maxZ", &MapBorderComponent::VisualParameters::maxZ)[M::DisplayName("Max Z")]
    .Field("zFadeWidth", &MapBorderComponent::VisualParameters::zFadeWidth)[M::DisplayName("Z Fade Width")]
    .Field("distanceFadeEnabled", &MapBorderComponent::VisualParameters::distanceFadeEnabled)[M::DisplayName("Distance Fade Enabled")]
    .Field("distanceFadeNear", &MapBorderComponent::VisualParameters::distanceFadeNear)[M::DisplayName("Distance Fade Near")]
    .Field("distanceFadeFar", &MapBorderComponent::VisualParameters::distanceFadeFar)[M::DisplayName("Distance Fade Far")]
    .Field("color", &MapBorderComponent::VisualParameters::color)[M::DisplayName("Color")]
    .End();
}

bool MapBorderComponent::VisualParameters::operator==(const VisualParameters& other) const
{
    return height == other.height &&
        zOffset == other.zOffset &&
        minZ == other.minZ &&
        maxZ == other.maxZ &&
        zFadeWidth == other.zFadeWidth &&
        distanceFadeEnabled == other.distanceFadeEnabled &&
        distanceFadeNear == other.distanceFadeNear &&
        distanceFadeFar == other.distanceFadeFar &&
        color == other.color;
}

DAVA_VIRTUAL_REFLECTION_IMPL(MapBorderComponent)
{
    ReflectionRegistrator<MapBorderComponent>::Begin()[M::CantBeCreatedManualyComponent(), M::CantBeDeletedManualyComponent()]
    .ConstructorByPointer()
    .Field("rect", &MapBorderComponent::rect)[M::DisplayName("Rect")]
    .Field("visualParams", &MapBorderComponent::visualParams)[M::DisplayName("Visual Parameters")]
    .Field("isDirty", &MapBorderComponent::isDirty)[M::DisplayName("Is Dirty")]
    .End();
}

MapBorderComponent::MapBorderComponent()
{
    rect = AABBox2(Vector2(-250.f, -250.f), Vector2(250.f, 250.f));

    visualParams = new VisualParameters();
    visualParams->height = 1.f;
    visualParams->zOffset = -0.2f;
    visualParams->minZ = -100.f;
    visualParams->maxZ = 100.f;
    visualParams->zFadeWidth = 2.f;
    visualParams->distanceFadeEnabled = false;
    visualParams->distanceFadeNear = 300.f;
    visualParams->distanceFadeFar = 400.f;
    visualParams->color = Color::Red;

    isDirty = false;
}

MapBorderComponent::~MapBorderComponent()
{
    if (visualParams)
    {
        delete visualParams;
    }
}

void MapBorderComponent::Deserialize(KeyedArchive* archive, SerializationContext* serializationContext)
{
    if (archive->IsKeyExists("mbc.rect"))
    {
        rect = archive->GetByteArrayAsType<AABBox2>("mbc.rect", rect);
    }
    if (archive->IsKeyExists("mbc.visualHeight"))
    {
        visualParams->height = archive->GetFloat("mbc.visualHeight");
    }
    if (archive->IsKeyExists("mbc.visualZOffset"))
    {
        visualParams->zOffset = archive->GetFloat("mbc.visualZOffset");
    }
    if (archive->IsKeyExists("mbc.visualMinZ"))
    {
        visualParams->minZ = archive->GetFloat("mbc.visualMinZ");
    }
    if (archive->IsKeyExists("mbc.visualMaxZ"))
    {
        visualParams->maxZ = archive->GetFloat("mbc.visualMaxZ");
    }
    if (archive->IsKeyExists("mbc.visualZFadeWidth"))
    {
        visualParams->zFadeWidth = archive->GetFloat("mbc.visualZFadeWidth");
    }

    if (archive->IsKeyExists("mbc.visualDistanceFadeEnabled"))
    {
        visualParams->distanceFadeEnabled = archive->GetBool("mbc.visualDistanceFadeEnabled");
    }

    if (archive->IsKeyExists("mbc.visualDistanceFadeNear"))
    {
        visualParams->distanceFadeNear = archive->GetFloat("mbc.visualDistanceFadeNear");
    }
    if (archive->IsKeyExists("mbc.visualDistanceFadeFar"))
    {
        visualParams->distanceFadeFar = archive->GetFloat("mbc.visualDistanceFadeFar");
    }

    if (archive->IsKeyExists("mbc.visualColor"))
    {
        visualParams->color = archive->GetColor("mbc.visualColor");
    }

    Component::Deserialize(archive, serializationContext);
}

void MapBorderComponent::Serialize(KeyedArchive* archive, SerializationContext* serializationContext)
{
    Component::Serialize(archive, serializationContext);

    archive->SetByteArray("mbc.rect", reinterpret_cast<const uint8*>(&rect), sizeof(AABBox2));

    archive->SetFloat("mbc.visualHeight", visualParams->height);
    archive->SetFloat("mbc.visualZOffset", visualParams->zOffset);
    archive->SetFloat("mbc.visualMinZ", visualParams->minZ);
    archive->SetFloat("mbc.visualMaxZ", visualParams->maxZ);
    archive->SetFloat("mbc.visualZFadeWidth", visualParams->zFadeWidth);

    archive->SetBool("mbc.visualDistanceFadeEnabled", visualParams->distanceFadeEnabled);

    archive->SetFloat("mbc.visualDistanceFadeNear", visualParams->distanceFadeNear);
    archive->SetFloat("mbc.visualDistanceFadeFar", visualParams->distanceFadeFar);

    archive->SetColor("mbc.visualColor", visualParams->color);
}

Component* MapBorderComponent::Clone(Entity* toEntity)
{
    DVASSERT(false && "MapBorderComponent::Clone I would rather avoid cloning this component");
    return nullptr;
}
}