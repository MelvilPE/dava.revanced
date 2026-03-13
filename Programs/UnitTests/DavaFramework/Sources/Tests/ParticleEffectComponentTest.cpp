#include "DAVAEngine.h"
#include "UnitTests/UnitTests.h"

#include "Base/BaseTypes.h"
#include "FileSystem/KeyedArchive.h"
#include "FileSystem/VariantType.h"
#include "Math/Vector.h"
#include "Scene3D/Components/ParticleEffectComponent.h"
#include "Scene3D/SceneFile/SerializationContext.h"

using namespace DAVA;

DAVA_TESTCLASS (ParticleEffectComponentTest)
{
    DAVA_TEST (TestDeserializeLegacy)
    {
        ParticleEffectComponent* PEC = new ParticleEffectComponent();
        KeyedArchive* archive = new KeyedArchive();
        SerializationContext* serializationContext = new SerializationContext();

        Scene* scene = new Scene();
        serializationContext->SetScene(scene);

        archive->SetBool("pe.nestedEmitters", false);
        archive->SetUInt32("pe.version", 42);
        archive->SetBool("pe.stopWhenEmpty", true);
        archive->SetFloat("pe.effectDuration", 10.5f);
        archive->SetUInt32("pe.repeatsCount", 3);
        archive->SetBool("pe.clearOnRestart", true);
        archive->SetFloat("pe.startFromTime", 1.2f);

        KeyedArchive* emittersArch = new KeyedArchive();
        KeyedArchive* emitter0 = new KeyedArchive();
        emitter0->SetString("emitter.filename", "emitter.yaml");
        emitter0->SetVector3("emitter.position", Vector3(1.0f, 1.0f, 1.0f));
        emittersArch->SetArchive(KeyedArchive::GenKeyFromIndex(0), emitter0);
        archive->SetArchive("pe.emitters", emittersArch);
        archive->SetUInt32("pe.emittersCount", 1);

        PEC->Deserialize(archive, serializationContext);

        TEST_VERIFY(PEC->GetEmittersCount() == 1);
        TEST_VERIFY(PEC->GetEmitterInstance(0)->GetEmitter() != nullptr);

        //SafeRelease(PEC);
        SafeRelease(archive);
        //SafeRelease(serializationContext);
        SafeRelease(emittersArch);
        SafeRelease(emitter0);
        SafeRelease(scene);
    }

    DAVA_TEST (TestDeserializeNestedSkip)
    {
        ParticleEffectComponent* PEC = new ParticleEffectComponent();
        KeyedArchive* archive = new KeyedArchive();
        SerializationContext* serializationContext = new SerializationContext();

        Scene* scene = new Scene();
        serializationContext->SetScene(scene);

        archive->SetBool("pe.nestedEmitters", true);

        PEC->Deserialize(archive, serializationContext);

        TEST_VERIFY(PEC->GetEmittersCount() == 0);

        //SafeRelease(PEC);
        SafeRelease(archive);
        //SafeRelease(serializationContext);
        SafeRelease(scene);
    }

    DAVA_TEST (TestDeserializeNestedParticlesGod)
    {
        ParticleEffectComponent* PEC = new ParticleEffectComponent();
        KeyedArchive* archive = new KeyedArchive();
        SerializationContext* serializationContext = new SerializationContext();

        Scene* scene = new Scene();
        serializationContext->SetScene(scene);

        archive->SetBool("pe.nestedEmitters", true);
        archive->SetBool("pe.updatedFromGame", true);

        Vector<VariantType> emitters;
        KeyedArchive* emitter0 = new KeyedArchive();
        emitter0->SetString("emitter.filename", "emitter.yaml");
        emitter0->SetVector3("emitter.position", Vector3(1.0f, 1.0f, 1.0f));
        VariantType var;
        var.SetKeyedArchive(emitter0);
        emitters.push_back(var);
        archive->SetVariantVector("pe.emitters", emitters);

        PEC->Deserialize(archive, serializationContext);

        TEST_VERIFY(PEC->IsUpdatedFromGame() == true);
        TEST_VERIFY(PEC->GetEmittersCount() == 1);
        TEST_VERIFY(PEC->GetEmitterInstance(0)->GetEmitter() != nullptr);

        //SafeRelease(PEC);
        SafeRelease(archive);
        //SafeRelease(serializationContext);
        SafeRelease(emitter0);
        SafeRelease(scene);
    }
};