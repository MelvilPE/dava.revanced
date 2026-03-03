#pragma once

#include "Scene3D/DataNode.h"

namespace DAVA
{
/**
    
 */
class ArchiveDataNode : public DataNode
{
public:
    ArchiveDataNode();
    ~ArchiveDataNode();

    virtual void Load(KeyedArchive* archive, SerializationContext* serializationContext) override;
    virtual void Save(KeyedArchive* archive, SerializationContext* serializationContext) override;

    virtual uint64 GetNodeID() const override;
    virtual void SetNodeID(uint64 nodeId) override;

    String GetNodeYaml();
    void SetNodeYaml(String value);

    KeyedArchive* GetArchive();

protected:
    KeyedArchive* archiveNode = nullptr;

    DAVA_VIRTUAL_REFLECTION(ArchiveDataNode, DataNode);
};
}
