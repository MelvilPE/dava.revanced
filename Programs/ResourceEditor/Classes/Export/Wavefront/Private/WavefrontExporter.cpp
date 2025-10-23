#include "Classes/Export/Wavefront/WavefrontExporter.h"
#include "Classes/Export/Wavefront/Private/WavefrontExporterTools.h"
#include "Classes/Qt/Plugins/PythonPluginsSingleton.h"

#include <REPlatform/DataNodes/SceneData.h>
#include <REPlatform/Scene/SceneEditor2.h>

#include <TArc/Utils/ModuleCollection.h>
#include <TArc/WindowSubSystem/ActionUtils.h>
#include <TArc/WindowSubSystem/QtAction.h>
#include <TArc/WindowSubSystem/UI.h>

#include <Base/RefPtr.h>
#include <Base/String.h>
#include <FileSystem/FileSystem.h>
#include <Functional/Function.h>
#include <Render/Highlevel/Landscape.h>
#include <Render/Highlevel/LandscapeThumbnails.h>
#include <Render/Highlevel/Light.h>
#include <Render/Highlevel/RenderBatch.h>
#include <Render/Highlevel/RenderObject.h>
#include <Render/Image/Image.h>
#include <Render/Image/ImageSystem.h>
#include <Render/Material/NMaterial.h>
#include <Render/Texture.h>
#include <Render/TextureDescriptor.h>
#include <Scene3D/Components/ComponentHelpers.h>
#include <Scene3D/Components/LightComponent.h>
#include <Scene3D/Components/TransformComponent.h>
#include <Scene3D/Lod/LodComponent.h>

namespace WavefrontExporterDetail
{
struct RenderBatchExport
{
    DAVA::RenderBatch* rb = nullptr;
    DAVA::Matrix4 transform;
    DAVA::String material;
};

class Exporter
{
public:
    static const DAVA::uint32 MeshExporterVersion = 1;

    DAVA::Vector<std::pair<DAVA::String, WavefrontExporterDetail::RenderBatchExport>> renderBatchesToExport;

    void CollectExportObjects(const DAVA::Entity* entity);
    bool Export(DAVA::Scene*, const DAVA::FilePath&);

    void ExportBatch(const DAVA::String& name, const WavefrontExporterDetail::RenderBatchExport& rb, std::ofstream& fOut, DAVA::int32 vertexOffset);

    bool RenderObjectsIsValidForExport(DAVA::RenderObject* object);
    bool RenderBatchIsValidForExport(DAVA::RenderBatch* batch, DAVA::int32 lodIndex, DAVA::int32 swIndex);
};
}

void WavefrontExporter::PostInit()
{
    using namespace DAVA;

    QtAction* exportAction = new QtAction(GetAccessor(), "Export to Wavefront...");
    FieldDescriptor fieldDescriptor(DAVA::ReflectedTypeDB::Get<SceneData>(), DAVA::FastName(SceneData::scenePropertyName));
    exportAction->SetStateUpdationFunction(QtAction::Enabled, fieldDescriptor, [](const DAVA::Any& value) -> DAVA::Any
                                           {
                                               return value.CanCast<SceneData::TSceneType>();
                                           });

    GetUI()->AddAction(DAVA::mainWindowKey, CreateMenuPoint("DebugFunctions"), exportAction);
    connections.AddConnection(exportAction, &QAction::triggered, DAVA::MakeFunction(this, &WavefrontExporter::Export));
}

void WavefrontExporter::Export()
{
    using namespace DAVA;

    FileDialogParams parameters;
    parameters.title = "Export to Wavefront";
    parameters.filters = "Wavefront file (*.obj)";
    QString exportFolder = GetUI()->GetSaveFileName(DAVA::mainWindowKey, parameters);
    if (!exportFolder.isEmpty())
    {
        DAVA::FilePath exportFile(exportFolder.toUtf8().toStdString());
        if (exportFile.GetExtension() != ".obj")
        {
            exportFile.ReplaceExtension(".obj");
        }

        WavefrontExporterDetail::Exporter exporter;
        SceneData* sceneData = GetAccessor()->GetActiveContext()->GetData<SceneData>();
        exporter.Export(sceneData->GetScene().Get(), exportFile);
    }
}

bool WavefrontExporterDetail::Exporter::Export(DAVA::Scene* scene, const DAVA::FilePath& fileExport)
{
    CollectExportObjects(scene);

    if (renderBatchesToExport.empty())
        return false;

    DAVA::String outFile = fileExport.GetAbsolutePathname();
    std::ofstream fOut(outFile.c_str(), std::ios::out);
    if (!fOut.is_open())
    {
        DAVA::Logger::Error("Cannot open file %s for writing.", outFile.c_str());
        return false;
    }

    DAVA::int32 vertexOffset = 0;

    for (const auto& rb : renderBatchesToExport)
    {
        ExportBatch(rb.first, rb.second, fOut, vertexOffset);

        // Incrément du vertexOffset pour le batch suivant
        if (rb.second.rb->GetPolygonGroup())
        {
            vertexOffset += rb.second.rb->GetPolygonGroup()->GetVertexCount();
        }
    }

    fOut.flush();
    fOut.close();

    wavefront_exporter::currentOutput = nullptr;
    return true;
}


void WavefrontExporterDetail::Exporter::ExportBatch(
    const DAVA::String& name,
    const WavefrontExporterDetail::RenderBatchExport& rb,
    std::ofstream& fOut,
    DAVA::int32 vertexOffset)
{
    DAVA::PolygonGroup* poly = rb.rb->GetPolygonGroup();
    if (poly == nullptr)
    {
        DAVA::Logger::Info("Object %s does not contain polygon group.", name.c_str());
        return;
    }

    DAVA::int32 vertexCount = poly->GetVertexCount();
    DAVA::int32 vertexFormat = poly->GetFormat();

    if ((vertexFormat & DAVA::EVF_NORMAL) == 0)
    {
        DAVA::Logger::Error("%s data does not contain normals", name.c_str());
        return;
    }

    bool hasPivot = (vertexFormat & DAVA::EVF_PIVOT4) == DAVA::EVF_PIVOT4;
    if (hasPivot)
    {
        DAVA::Logger::Error("%s data contains pivot point, which are not supported", name.c_str());
    }

    bool hasTexCoords = (vertexFormat & DAVA::EVF_TEXCOORD0) == DAVA::EVF_TEXCOORD0;

    // Crée un objet dans l'OBJ pour ce batch
    fOut << std::endl
         << "o " << name << std::endl;

    // Écriture des vertices, normales et UV
    for (DAVA::int32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    {
        DAVA::Vector3 v;
        poly->GetCoord(vertexIndex, v);
        fOut << DAVA::Format("v %.7f %.7f %.7f\n", v.x, v.y, v.z);

        DAVA::Vector3 n;
        poly->GetNormal(vertexIndex, n);
        fOut << DAVA::Format("vn %.7f %.7f %.7f\n", n.x, n.y, n.z);

        if (hasTexCoords)
        {
            DAVA::Vector2 t;
            poly->GetTexcoord(0, vertexIndex, t);
            fOut << DAVA::Format("vt %.7f %.7f\n", t.x, 1.0f - t.y);
        }
    }

    // Écriture des faces avec décalage d'indice
    for (DAVA::int32 ti = 0, te = poly->GetPrimitiveCount(); ti < te; ++ti)
    {
        DAVA::int32 tri[3] = {};
        poly->GetIndex(3 * ti + 0, tri[0]);
        poly->GetIndex(3 * ti + 1, tri[1]);
        poly->GetIndex(3 * ti + 2, tri[2]);

        if (hasTexCoords)
        {
            fOut << "f " << tri[0] + 1 + vertexOffset << "/" << tri[0] + 1 + vertexOffset << "/" << tri[0] + 1 + vertexOffset << wavefront_exporter::kSpace << tri[1] + 1 + vertexOffset << "/" << tri[1] + 1 + vertexOffset << "/" << tri[1] + 1 + vertexOffset << wavefront_exporter::kSpace << tri[2] + 1 + vertexOffset << "/" << tri[2] + 1 + vertexOffset << "/" << tri[2] + 1 + vertexOffset << wavefront_exporter::kSpace << std::endl;
        }
        else
        {
            fOut << "f " << tri[0] + 1 + vertexOffset << "//" << tri[0] + 1 + vertexOffset << wavefront_exporter::kSpace << tri[1] + 1 + vertexOffset << "//" << tri[1] + 1 + vertexOffset << wavefront_exporter::kSpace << tri[2] + 1 + vertexOffset << "//" << tri[2] + 1 + vertexOffset << wavefront_exporter::kSpace << std::endl;
        }
    }
}

void WavefrontExporterDetail::Exporter::CollectExportObjects(const DAVA::Entity* entity)
{
    DAVA::RenderObject* renderObject = DAVA::GetRenderObject(entity);
    if (RenderObjectsIsValidForExport(renderObject))
    {
        if (renderObject->GetType() != DAVA::RenderObject::TYPE_LANDSCAPE)
        {
            DAVA::TransformComponent* tc = entity->GetComponent<DAVA::TransformComponent>();
            DAVA::uint32 renderBatchCount = renderObject->GetRenderBatchCount();
            for (DAVA::uint32 i = 0; i < renderBatchCount; ++i)
            {
                DAVA::int32 lodIndex = -1;
                DAVA::int32 swIndex = -1;
                DAVA::RenderBatch* batch = renderObject->GetRenderBatch(i, lodIndex, swIndex);
                if (RenderBatchIsValidForExport(batch, lodIndex, swIndex))
                {
                    DAVA::NMaterial* material = batch->GetMaterial();
                    DAVA::String materialID = DAVA::Format("mat_%p", material);

                    DAVA::String batchID = entity->GetName().c_str();
                    if (entity->GetComponent<DAVA::LodComponent>() != nullptr)
                    {
                        batchID = DAVA::Format("%s_lod%u", entity->GetName().c_str(), i);
                    }
                    
                    renderBatchesToExport.emplace_back(batchID, WavefrontExporterDetail::RenderBatchExport());
                    renderBatchesToExport.back().second.rb = batch;
                    renderBatchesToExport.back().second.material = materialID;
                    renderBatchesToExport.back().second.transform = tc->GetWorldMatrix();
                }
            }
        }
    }

    for (DAVA::int32 i = 0, e = entity->GetChildrenCount(); i < e; ++i)
    {
        CollectExportObjects(entity->GetChild(i));
    }
}

bool WavefrontExporterDetail::Exporter::RenderObjectsIsValidForExport(DAVA::RenderObject* object)
{
    static const DAVA::Set<DAVA::RenderObject::eType> validRenderObjectTypes =
    {
      DAVA::RenderObject::TYPE_RENDEROBJECT,
      DAVA::RenderObject::TYPE_MESH,
      DAVA::RenderObject::TYPE_SPEED_TREE
    };

    return (object != nullptr) && (validRenderObjectTypes.count(object->GetType()) > 0);
}

bool WavefrontExporterDetail::Exporter::RenderBatchIsValidForExport(DAVA::RenderBatch* batch, DAVA::int32 lodIndex, DAVA::int32 swIndex)
{
    return batch != nullptr;
}

DECL_TARC_MODULE(WavefrontExporter);
