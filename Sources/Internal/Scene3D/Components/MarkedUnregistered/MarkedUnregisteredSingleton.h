#pragma once

#include "FileSystem/KeyedArchive.h"
#include "Base/BaseTypes.h"

namespace DAVA
{
class MarkedUnregisteredSingleton
{
private:
    DAVA::KeyedArchive* object;

    MarkedUnregisteredSingleton();
    ~MarkedUnregisteredSingleton();

public:
    MarkedUnregisteredSingleton(const MarkedUnregisteredSingleton&) = delete;
    MarkedUnregisteredSingleton& operator=(const MarkedUnregisteredSingleton&) = delete;

    static MarkedUnregisteredSingleton* GetInstance();

    Vector<String> GetAllMarkedUnregistered();

    bool TryUpdateList(KeyedArchive* archive);

    //void SetList(Vector<String> );
};
}; // namespace DAVA