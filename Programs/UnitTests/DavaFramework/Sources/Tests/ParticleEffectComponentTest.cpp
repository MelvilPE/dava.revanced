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
        KeyedArchive* archive = CreateNewEffectLegacyArchive();
        SerializationContext* serializationContext = new SerializationContext();

        Scene* scene = new Scene();
        serializationContext->SetScene(scene);

        PEC->Deserialize(archive, serializationContext);

        TEST_VERIFY(PEC->IsNestedEmitters() == false);
        TEST_VERIFY(PEC->IsUpdatedFromGame() == false);

        TEST_VERIFY(PEC->GetEmittersCount() == 1);
        TEST_VERIFY(PEC->GetEmitterInstance(0)->GetEmitter() != nullptr);

        String emitterFilename = PEC->GetOriginalConfigPath(0).GetStringValue();
        TEST_VERIFY(HasYamlExtension(emitterFilename) == true);

        SafeRelease(archive);
        SafeRelease(scene);
    }

    DAVA_TEST (TestDeserializeNested)
    {
        ParticleEffectComponent* PEC = new ParticleEffectComponent();
        KeyedArchive* archive = CreateNewEffectNestedArchive();
        SerializationContext* serializationContext = new SerializationContext();

        Scene* scene = new Scene();
        serializationContext->SetScene(scene);

        PEC->Deserialize(archive, serializationContext);

        TEST_VERIFY(PEC->IsNestedEmitters() == true);
        TEST_VERIFY(PEC->IsUpdatedFromGame() == false);

        // We don't even try to load any emitters
        TEST_VERIFY(PEC->GetEmittersCount() == 0);

        SafeRelease(archive);
        SafeRelease(scene);
    }

    DAVA_TEST (TestDeserializeNestedParticlesGod)
    {
        ParticleEffectComponent* PEC = new ParticleEffectComponent();
        KeyedArchive* archive = CreateNewEffectNestedParticlesGodArchive();
        SerializationContext* serializationContext = new SerializationContext();

        Scene* scene = new Scene();
        serializationContext->SetScene(scene);

        PEC->Deserialize(archive, serializationContext);
        
        TEST_VERIFY(PEC->IsNestedEmitters() == true);
        TEST_VERIFY(PEC->IsUpdatedFromGame() == true);

        TEST_VERIFY(PEC->GetEmittersCount() == 1);
        TEST_VERIFY(PEC->GetEmitterInstance(0)->GetEmitter() != nullptr);

        String emitterFilename = PEC->GetOriginalConfigPath(0).GetStringValue();
        TEST_VERIFY(HasYamlExtension(emitterFilename) == true);

        SafeRelease(archive);
        SafeRelease(scene);
    }

    KeyedArchive* CreateNewEffectLegacyArchive()
    {
        String archiveMock = R"(
keyedArchive:
    comp.typename:
        string: "ParticleEffectComponent"
    pe.emitters:
        keyedArchive:
            0000:
                keyedArchive:
                    emitter.filename:
                        string: "LeopardTest/welding.yaml"
                    emitter.position:
                        Vector3: [0.000000, 0.000000, 0.000000]
    pe.clearOnRestart:
        bool: true
    pe.repeatsCount:
        uint32: 4294967295
    pe.effectDuration:
        float: 10000.000000
    pe.stopWhenEmpty:
        bool: false
    pe.emittersCount:
        uint32: 1
    pe.startFromTime:
        float: 0.000000
    pe.version:
        uint32: 1
    ro.flags:
        uint32: 1
)";
        KeyedArchive* newArchive = new KeyedArchive();
        newArchive->LoadFromYamlString(archiveMock);
        return newArchive;
    }

        KeyedArchive* CreateNewEffectNestedArchive()
    {
        String archiveMock = R"(
keyedArchive:
    comp.typename:
        string: "ParticleEffectComponent"
    pe.emitters:
        variantVector:
        -   keyedArchive:
                '##name':
                    string: "<Unknown class>"
                emitter.data:
                    variantVector:
                    -   keyedArchive:
                            emitter.id:
                                uint64: 282
                emitter.position:
                    Vector3: [0.000000, 0.000000, 0.000000]
                emitter.rotation:
                    Vector4: [0.000000, 0.000000, 0.000000, 1.000000]
                emitter.scale:
                    Vector3: [1.000000, 1.000000, 1.000000]
    pe.clearOnRestart:
        bool: true
    pe.repeatsCount:
        uint32: 4294967295
    pe.effectDuration:
        float: 10000.000000
    pe.stopWhenEmpty:
        bool: false
    pe.emittersCount:
        uint32: 1
    pe.startFromTime:
        float: 0.000000
    pe.version:
        uint32: 1
    ro.flags:
        uint32: 1
    pe.nestedEmitters:
        bool: true
)";
        KeyedArchive* newArchive = new KeyedArchive();
        newArchive->LoadFromYamlString(archiveMock);
        return newArchive;
    }

    KeyedArchive* CreateNewEffectNestedParticlesGodArchive()
    {
        String archiveMock = R"(
keyedArchive:
    comp.typename:
        string: "ParticleEffectComponent"
    pe.emitters:
        variantVector:
        -   keyedArchive:
                '##name':
                    string: "<Unknown class>"
                emitter.data:
                    variantVector:
                    -   keyedArchive:
                            emitter.id:
                                uint64: 282
                emitter.position:
                    Vector3: [0.000000, 0.000000, 0.000000]
                emitter.rotation:
                    Vector4: [0.000000, 0.000000, 0.000000, 1.000000]
                emitter.scale:
                    Vector3: [1.000000, 1.000000, 1.000000]
                emitter.filename:
                    string: "LeopardTest/welding.yaml"
    pe.clearOnRestart:
        bool: true
    pe.repeatsCount:
        uint32: 4294967295
    pe.updatedFromGame:
        bool: true
    pe.effectDuration:
        float: 10000.000000
    pe.stopWhenEmpty:
        bool: false
    pe.emittersCount:
        uint32: 1
    pe.startFromTime:
        float: 0.000000
    pe.version:
        uint32: 1
    ro.flags:
        uint32: 1
    pe.nestedEmitters:
        bool: true
)";
        KeyedArchive* newArchive = new KeyedArchive();
        newArchive->LoadFromYamlString(archiveMock);
        return newArchive;
    }

    bool HasYamlExtension(String value)
    {
        bool isYamlFilename = false;
        if (value.find(".yaml") != String::npos)
        {
            isYamlFilename = true;
        }

        return isYamlFilename;
    }
};