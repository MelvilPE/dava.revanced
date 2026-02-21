#pragma once

namespace DAVA
{
class EnableMaterialsMigrationSingleton
{
private:
    bool value;

    EnableMaterialsMigrationSingleton();
    // ~EnableMaterialsMigrationSingleton();

public:
    EnableMaterialsMigrationSingleton(const EnableMaterialsMigrationSingleton&) = delete;
    EnableMaterialsMigrationSingleton& operator=(const EnableMaterialsMigrationSingleton&) = delete;

    static EnableMaterialsMigrationSingleton* GetInstance();
    bool GetValue();
    void SetValue(bool init);
};
}; // namespace DAVA