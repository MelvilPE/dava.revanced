#pragma once

#include "Base/BaseTypes.h"
#include "Reflection/Reflection.h"
#include "Entity/Component.h"
#include "Math/Color.h"
#include "Math/AABBox2.h"

namespace DAVA
{
class BaseObject;

class MapBorderComponent : public Component
{
public:
    struct VisualParameters : public InspBase
    {
        float height;
        float zOffset;
        float minZ;
        float maxZ;
        float zFadeWidth;
        bool distanceFadeEnabled;
        float distanceFadeNear;
        float distanceFadeFar;
        Color color;

        bool operator==(const VisualParameters& other) const;

        DAVA_VIRTUAL_REFLECTION(VisualParameters, InspBase);
    };

public:
    MapBorderComponent();
    ~MapBorderComponent();

    void Serialize(KeyedArchive* archive, SerializationContext* serializationContext) override;
    void Deserialize(KeyedArchive* archive, SerializationContext* serializationContext) override;

    Component* Clone(Entity* toEntity) override;

    DAVA_VIRTUAL_REFLECTION(MapBorderComponent, Component);

private:
    AABBox2 rect;
    VisualParameters* visualParams;
    bool isDirty;
};
}
