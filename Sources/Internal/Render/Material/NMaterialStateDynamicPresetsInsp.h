#ifndef __DAVAENGINE_NMATERIALSTATEDYNAMICPRESETSINSP_NAMES_H__
#define __DAVAENGINE_NMATERIALSTATEDYNAMICPRESETSINSP_NAMES_H__

#include "Base/BaseTypes.h"
#include "Base/Introspection.h"

#include "NMaterialStateDynamicSingleton.h"

namespace DAVA
{
class NMaterial;

class NMaterialStateDynamicPresetsInsp : public InspInfoDynamic
{
public:
    DynamicData Prepare(void* object, int filter) const override;
    Vector<FastName> MembersList(const DynamicData& ddata) const override;
    InspDesc MemberDesc(const DynamicData& ddata, const FastName& member) const override;
    int MemberFlags(const DynamicData& ddata, const FastName& member) const override;
    VariantType MemberValueGet(const DynamicData& ddata, const FastName& member) const override;
    void MemberValueSet(const DynamicData& ddata, const FastName& member, const VariantType& value) override;
};
};

#endif /* defined(__DAVAENGINE_NMATERIALSTATEDYNAMICPRESETSINSP_NAMES_H__) */
