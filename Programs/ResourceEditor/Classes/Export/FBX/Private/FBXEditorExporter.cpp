#include "Classes/Export/FBX/FBXEditorExporter.h"

#include <FBX/FBXExporter.h>

#include <REPlatform/DataNodes/SceneData.h>

#include <TArc/Utils/ModuleCollection.h>
#include <TArc/WindowSubSystem/ActionUtils.h>
#include <TArc/WindowSubSystem/QtAction.h>
#include <TArc/WindowSubSystem/UI.h>

#include <Base/RefPtr.h>
#include <Base/String.h>
#include <Functional/Function.h>

namespace FBXEditorExporterDetail
{
static const DAVA::String FileExtension = ".fbx";
} // namespace FBXEditorExporterDetail

void FBXEditorExporter::PostInit()
{
    using namespace DAVA;

    QtAction* exportAction = new QtAction(GetAccessor(), "Export to FBX...");
    FieldDescriptor fieldDescriptor(DAVA::ReflectedTypeDB::Get<SceneData>(), DAVA::FastName(SceneData::scenePropertyName));
    exportAction->SetStateUpdationFunction(QtAction::Enabled, fieldDescriptor, [](const DAVA::Any& value) -> DAVA::Any
                                           { return value.CanCast<SceneData::TSceneType>(); });

    GetUI()->AddAction(DAVA::mainWindowKey, CreateMenuPoint("DebugFunctions"), exportAction);
    connections.AddConnection(exportAction, &QAction::triggered, DAVA::MakeFunction(this, &FBXEditorExporter::Export));
}

void FBXEditorExporter::Export()
{
    using namespace DAVA;

    FileDialogParams parameters;
    parameters.title = "Export to";
    parameters.filters = "FBX File (*.fbx)";
    QString exportFolder = GetUI()->GetSaveFileName(DAVA::mainWindowKey, parameters);
    if (!exportFolder.isEmpty())
    {
        DAVA::FilePath exportFile(exportFolder.toUtf8().toStdString());
        if (exportFile.GetExtension() != FBXEditorExporterDetail::FileExtension)
        {
            exportFile.ReplaceExtension(FBXEditorExporterDetail::FileExtension);
        }

        SceneData* sceneData = GetAccessor()->GetActiveContext()->GetData<SceneData>();
        FBXExporter::ExportSceneToFBX(sceneData->GetScene().Get(), exportFile);
    }
}

DECL_TARC_MODULE(FBXEditorExporter);
