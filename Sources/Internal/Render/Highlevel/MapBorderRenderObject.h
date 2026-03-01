#pragma once

#include "Base/BaseTypes.h"
#include "Render/Highlevel/RenderObject.h"
#include "Render/Material/NMaterial.h"
#include "Reflection/Reflection.h"

namespace DAVA
{
class MapBorderRenderObject : public RenderObject
{
public:
    struct  MapBorderPoint
    {
        Vector3 position;
        unsigned int color;
    };

    struct FadeParams
    {
        bool fadeEnabled;
        Vector2 fadeNearFarSq;
    };

public:
    MapBorderRenderObject();
    ~MapBorderRenderObject();

    void Load(KeyedArchive* archive, SerializationContext* serializationContext) override;
    void Save(KeyedArchive* archive, SerializationContext* serializationContext) override;
    // RenderObject* Clone(RenderObject* newObject) override;

    void SetDistanceFadeParams(FadeParams* params);

    bool GetFadeEnabled() const;
    void SetFadeEnabled(bool value);

    Vector2 GetFadeNearFarSq() const;
    void SetFadeNearFarSq(Vector2 value);

private:
    FadeParams fadeParams;
    NMaterial* borderMaterial;

    DAVA_VIRTUAL_REFLECTION(MapBorderRenderObject, RenderObject);
};
}
