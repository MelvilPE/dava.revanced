#include "ParticleEmitterNode.h"

#include "Reflection/ReflectionRegistrator.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(ParticleEmitterNode)
{
    ReflectionRegistrator<ParticleEmitterNode>::Begin()
    .End();
}

void ParticleEmitterNode::GetMeshGeometryIds(Vector<uint64>& output)
{
    GetMeshGeometryIdsRecursively(output, archiveNode);
}

void ParticleEmitterNode::GetMeshGeometryIdsRecursively(Vector<uint64>& output, KeyedArchive* archive)
{
    Vector<VariantType> layers;
    if (archive->IsKeyExists("layers"))
    {
        layers = archive->GetVariantVector("layers");
    }

    for (const auto& layer : layers)
    {
        KeyedArchive* layerArchive = layer.AsKeyedArchive();
        if (layerArchive->IsKeyExists("meshGeometryId"))
        {
            uint64 id = layerArchive->GetUInt64("meshGeometryId");
            output.push_back(id);
        }

        if (layerArchive->IsKeyExists("innerEmitter"))
        {
            KeyedArchive* innerArch = layerArchive->GetArchive("innerEmitter");
            GetMeshGeometryIdsRecursively(output, innerArch);
        }
    }
}

void ParticleEmitterNode::UpdateMeshGeometryIds(SerializationContext* serializationContext)
{
    UpdateMeshGeometryIdsRecursively(archiveNode, serializationContext);
}

void ParticleEmitterNode::UpdateMeshGeometryIdsRecursively(KeyedArchive* archive, SerializationContext* serializationContext)
{
    Vector<VariantType> layers;
    if (archive->IsKeyExists("layers"))
    {
        layers = archive->GetVariantVector("layers");
    }

    for (auto& layer : layers)
    {
        KeyedArchive* layerArchive = layer.AsKeyedArchive();
        if (layerArchive->IsKeyExists("meshGeometryId"))
        {
            uint64 oldId = layerArchive->GetUInt64("meshGeometryId");
            uint64 newId = serializationContext->GetUpdatedNodeId(oldId);
            layerArchive->SetUInt64("meshGeometryId", newId);
        }

        if (layerArchive->IsKeyExists("innerEmitter"))
        {
            KeyedArchive* innerArch = layerArchive->GetArchive("innerEmitter");
            UpdateMeshGeometryIdsRecursively(innerArch, serializationContext);
        }
    }
}

bool ParticleEmitterNode::IsClone()
{
    return archiveNode->IsKeyExists("clone") && archiveNode->GetBool("clone");
}

uint64 ParticleEmitterNode::GetReferenceId()
{
    return archiveNode->GetUInt64("reference.id");
}

void ParticleEmitterNode::SetReferenceId(uint64 id)
{
    return archiveNode->SetUInt64("reference.id", id);
}
}; // namespace DAVA
