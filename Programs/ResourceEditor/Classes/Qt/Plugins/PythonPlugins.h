#pragma once

#include "Classes/Qt/Plugins/PythonPluginsSingleton.h"

#include <TArc/Core/ControllerModule.h>
#include <TArc/Core/FieldBinder.h>
#include <TArc/Models/RecentMenuItems.h>
#include <TArc/Utils/QtConnections.h>

#include <Base/BaseTypes.h>

namespace DAVA
{
class FilePath;
class SceneEditor2;
class SceneData;
} // namespace DAVA
class FileSystemCache;

class PythonPlugins : public DAVA::ClientModule
{
public:
    PythonPlugins();
    ~PythonPlugins() override;

protected:
    void OnContextCreated(DAVA::DataContext* context) override;
    void OnContextDeleted(DAVA::DataContext* context) override;
    void OnContextWillBeChanged(DAVA::DataContext* current, DAVA::DataContext* newOne) override;
    void OnContextWasChanged(DAVA::DataContext* current, DAVA::DataContext* oldOne) override;
    void OnWindowClosed(const DAVA::WindowKey& key) override;

    void PostInit() override;

private:
    void CreateModuleControls(DAVA::UI* ui);
    void CreateModuleActions(DAVA::UI* ui);

    void RunPluginAsync(DAVA::String pluginName, DAVA::FilePath scriptPath);
    void RunPlugin(DAVA::String pluginName, DAVA::FilePath scriptPath);

private:
    DAVA::QtConnections connections;

    DAVA_VIRTUAL_REFLECTION_IN_PLACE(PythonPlugins, DAVA::ClientModule)
    {
        DAVA::ReflectionRegistrator<PythonPlugins>::Begin()
        .ConstructorByPointer()
        .End();
    }
};
