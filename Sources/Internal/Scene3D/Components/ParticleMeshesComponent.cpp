#include "Scene3D/Components/ParticleMeshesComponent.h"
#include "Scene3D/Systems/UpdateSystem.h"
#include "Scene3D/Entity.h"
#include "Scene3D/Scene.h"
#include "Particles/Gen2/ParticleEmitterNode.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Reflection/ReflectedMeta.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(ParticleMeshesComponent)
{
    ReflectionRegistrator<ParticleMeshesComponent>::Begin()[M::CantBeCreatedManualyComponent(), M::NonSerializableComponent(), M::NonExportableComponent()]
    .ConstructorByPointer()
    .End();
}

ParticleMeshesComponent::~ParticleMeshesComponent()
{
    for (auto& storedMesh : storedMeshes)
    {
        SafeRelease(storedMesh);
    }

    storedMeshes.clear();
}

ParticleMeshesComponent::ParticleMeshesComponent()
{}

Component* ParticleMeshesComponent::Clone(Entity* toEntity)
{
    DVASSERT(false && "ParticleMeshesComponent::Clone I would rather avoid cloning this component");
    return nullptr;
}

void ParticleMeshesComponent::GetDataNodes(Set<DAVA::DataNode*>& dataNodes)
{
    for (const auto& storedMesh : storedMeshes)
    {
        dataNodes.insert(storedMesh);
    }
}

void ParticleMeshesComponent::AddMesh(PolygonGroup* mesh)
{
    auto it = std::find(storedMeshes.begin(), storedMeshes.end(), mesh);
    if (it == storedMeshes.end())
    {
        PolygonGroup* node = SafeRetain(mesh);
        storedMeshes.push_back(node);
    }
}

void ParticleMeshesComponent::SetupParticleMeshesComponent(Entity* entity, KeyedArchive* archive, SerializationContext* serializationContext)
{
    Scene* scene = serializationContext->GetScene();
    Vector<ParticleEmitterNode*> nodes = scene->GetParticleEmitterNodes();

    // Build a map from node ID -> node pointer
    std::unordered_map<uint64, ParticleEmitterNode*> nodeMap;
    for (auto& node : nodes)
    {
        nodeMap[node->GetNodeID()] = node;
    }

    Vector<VariantType> emitters = archive->GetVariantVector("pe.emitters");
    Vector<uint64> meshGeometryIds;

    for (auto& emitterVar : emitters)
    {
        KeyedArchive* emitterArch = emitterVar.AsKeyedArchive();
        Vector<VariantType> emitterDatas = emitterArch->GetVariantVector("emitter.data");

        for (auto& emitterDataVar : emitterDatas)
        {
            KeyedArchive* emitterDataArch = emitterDataVar.AsKeyedArchive();
            uint64 emitterId = emitterDataArch->GetUInt64("emitter.id");

            auto it = nodeMap.find(emitterId);
            if (it != nodeMap.end())
            {
                it->second->GetMeshGeometryIds(meshGeometryIds);
            }
        }
    }

    if (meshGeometryIds.size() > 0)
    {
        ParticleMeshesComponent* comp = entity->GetOrCreateComponent<ParticleMeshesComponent>();
        for (const auto& meshGeometryId : meshGeometryIds)
        {
            PolygonGroup* mesh = static_cast<PolygonGroup*>(serializationContext->GetDataBlock(meshGeometryId));
            comp->AddMesh(mesh);
        }
    }
}
}