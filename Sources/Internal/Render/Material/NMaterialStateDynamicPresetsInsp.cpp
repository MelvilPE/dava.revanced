#include "Render/Material/NMaterial.h"
#include "Render/Material/FXCache.h"
#include "Render/Material/NMaterialStateDynamicPresetsInsp.h"

namespace DAVA
{
///////////////////////////////////////////////////////////////////////////
///// NMaterialStateDynamicPresetsInsp implementation

InspInfoDynamic::DynamicData NMaterialStateDynamicPresetsInsp::Prepare(void* object, int filter) const
{
    InspInfoDynamic::DynamicData ddata;
    ddata.object = object;

    return ddata;
}

Vector<FastName> NMaterialStateDynamicPresetsInsp::MembersList(const DynamicData& ddata) const
{
    static Vector<FastName> ret;

    if (ret.empty())
    {
        KeyedArchive* archive = NMaterialStateDynamicSingleton::GetInstance()->GetArchive();

        Vector<VariantType> presets = archive->GetVariantVector("presets");
        for (auto preset : presets)
        {
            ret.push_back(FastName(preset.AsString()));
        }
    }

    return ret;
}

InspDesc NMaterialStateDynamicPresetsInsp::MemberDesc(const DynamicData& ddata, const FastName& member) const
{
    return InspDesc(member.c_str());
}

VariantType NMaterialStateDynamicPresetsInsp::MemberValueGet(const DynamicData& ddata, const FastName& member) const
{
    NMaterial* material = static_cast<NMaterial*>(ddata.object);
    DVASSERT(material);

    VariantType ret;
    ret.SetBool(0 != material->GetEffectivePresetValue(member));
    return ret;
}

void NMaterialStateDynamicPresetsInsp::MemberValueSet(const DynamicData& ddata, const FastName& member, const VariantType& value)
{
    NMaterial* material = static_cast<NMaterial*>(ddata.object);
    DVASSERT(material);

    // empty value is thread as preset remove
    if (value.GetType() == VariantType::TYPE_NONE)
    {
        if (material->HasLocalPreset(member))
        {
            material->RemovePreset(member);
        }
    }
    else
    {
        int32 newValue = 0;
        if ((value.GetType() == VariantType::TYPE_BOOLEAN) && value.AsBool())
        {
            newValue = 1;
        }

        if (material->HasLocalPreset(member))
        {
            material->SetPreset(member, newValue);
        }
        else
        {
            material->AddPreset(member, newValue);
        }
    }
}

int NMaterialStateDynamicPresetsInsp::MemberFlags(const DynamicData& ddata, const FastName& member) const
{
    NMaterial* material = static_cast<NMaterial*>(ddata.object);
    DVASSERT(material);

    return I_VIEW | (material->HasLocalPreset(member) ? I_EDIT : 0);
}
};
