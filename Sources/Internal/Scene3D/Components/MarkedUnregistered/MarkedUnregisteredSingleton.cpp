#include "MarkedUnregisteredSingleton.h"

namespace DAVA
{
MarkedUnregisteredSingleton* MarkedUnregisteredSingleton::GetInstance()
{
    static MarkedUnregisteredSingleton instance;
    return &instance;
}

MarkedUnregisteredSingleton::MarkedUnregisteredSingleton()
{
    object = new DAVA::KeyedArchive();
    Vector<VariantType> emptyArray;
    object->SetVariantVector("list", emptyArray);
}

MarkedUnregisteredSingleton::~MarkedUnregisteredSingleton()
{
    DAVA::SafeRelease(object);
}

Vector<String> MarkedUnregisteredSingleton::GetAllMarkedUnregistered()
{
    Vector<String> result;

    if (!object->IsKeyExists("list"))
    {
        return result;
    }

    Vector<VariantType> unformat = object->GetVariantVector("list");
    for (auto variant : unformat)
    {
        String appended = variant.AsString();
        result.push_back(appended);
    }

    return result;
}

bool MarkedUnregisteredSingleton::TryUpdateList(KeyedArchive* archive)
{
    if (!archive->IsKeyExists("list"))
    {
        return false;
    }

    Vector<VariantType> ready = archive->GetVariantVector("list");
    object->SetVariantVector("list", ready);
    return true;
}

} // namespace DAVA