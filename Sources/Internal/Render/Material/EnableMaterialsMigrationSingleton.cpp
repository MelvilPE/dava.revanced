#include "EnableMaterialsMigrationSingleton.h"

namespace DAVA
{
EnableMaterialsMigrationSingleton* EnableMaterialsMigrationSingleton::GetInstance()
{
    static EnableMaterialsMigrationSingleton instance;
    return &instance;
}

EnableMaterialsMigrationSingleton::EnableMaterialsMigrationSingleton()
{
    value = false;
}

bool EnableMaterialsMigrationSingleton::GetValue()
{
    return value;
}

void EnableMaterialsMigrationSingleton::SetValue(bool init)
{
    value = init;
}

} // namespace DAVA