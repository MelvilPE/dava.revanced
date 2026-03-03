#include "Scene3D/ArchiveDataNode.h"
#include "FileSystem/KeyedArchive.h"
#include "Scene3D/SceneFileV2.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Reflection/ReflectedMeta.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(ArchiveDataNode)
{
    ReflectionRegistrator<ArchiveDataNode>::Begin()
    .End();
}

ArchiveDataNode::ArchiveDataNode()
    : DataNode()
{
    archiveNode = nullptr;
    isRuntime = false;
}

ArchiveDataNode::~ArchiveDataNode()
{
    SafeRelease(archiveNode);
}

void ArchiveDataNode::Load(KeyedArchive* archive, SerializationContext* serializationContext)
{
    archiveNode = SafeRetain(archive);
    id = archiveNode->GetByteArrayAsType<uint64>("#id", 0);
}

void ArchiveDataNode::Save(KeyedArchive* archive, SerializationContext* serializationContext)
{
    archive->LoadFromYamlString(archiveNode->SaveToYamlString());
}

uint64 ArchiveDataNode::GetNodeID() const
{
    return archiveNode->GetByteArrayAsType<uint64>("#id", 0);
}

void ArchiveDataNode::SetNodeID(uint64 nodeId)
{
    archiveNode->SetByteArrayAsType<uint64>("#id", nodeId);
}

String ArchiveDataNode::GetNodeYaml()
{
    return archiveNode->SaveToYamlString();
}

void ArchiveDataNode::SetNodeYaml(String value)
{
    archiveNode->LoadFromYamlString(value);
}

KeyedArchive* ArchiveDataNode::GetArchive()
{
    return archiveNode;
}
}
