#pragma once

#include "FileSystem/KeyedArchive.h"
#include "Base/BaseTypes.h"
#include "Logger/Logger.h"

namespace DAVA
{
class NMaterialStateDynamicSingleton
{
private:
    DAVA::KeyedArchive* archive;

    NMaterialStateDynamicSingleton();
    ~NMaterialStateDynamicSingleton();

    struct PropertyDefinition
    {
        String name;
        uint32 type;
        uint32 size;
        VariantType defaultValue;
    };

public:
    NMaterialStateDynamicSingleton(const NMaterialStateDynamicSingleton&) = delete;
    NMaterialStateDynamicSingleton& operator=(const NMaterialStateDynamicSingleton&) = delete;

    static NMaterialStateDynamicSingleton* GetInstance();

    KeyedArchive* GetArchive();

    bool TryUpdateArchive(KeyedArchive* param);

    String GetDefaultArchiveYaml();
    void SetDefaultArchiveYaml();
    static Vector<VariantType> DefaultArchiveProperties(Vector<PropertyDefinition> defs);
};
}; // namespace DAVA