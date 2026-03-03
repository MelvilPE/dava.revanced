#pragma once

#include "Base/BaseTypes.h"
#include "Reflection/Reflection.h"
#include "Entity/Component.h"
#include "Logger/Logger.h"

namespace DAVA
{
/**
 * @brief This component is typically used alongside a ParticleEffectComponent
 * It collects and stores PolygonGroup meshes referenced by particle emitters
 */
class ParticleMeshesComponent : public Component
{
public:
    ParticleMeshesComponent();
    ~ParticleMeshesComponent();

    Component* Clone(Entity* toEntity) override;

    void GetDataNodes(Set<DataNode*>& dataNodes) override;
    void AddMesh(PolygonGroup* mesh);

    /**
     * @brief Collect emitter.id's, resolve nodes, find meshGeometryId's, finally store meshes
     * 
     * @param entity The entity that may receive the ParticleMeshesComponent
     * @param archive The archive containing nested particle effect component
     * @param context The serialization context used to resolve mesh data blocks
     */
    static void SetupParticleMeshesComponent(Entity* entity, KeyedArchive* archive, SerializationContext* context);

private:
    Vector<PolygonGroup*> storedMeshes;

    DAVA_VIRTUAL_REFLECTION(ParticleMeshesComponent, Component);
};
}
