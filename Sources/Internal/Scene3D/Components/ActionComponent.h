#pragma once

#include "Entity/Component.h"
#include "Reflection/Reflection.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Base/Any.h"

namespace DAVA
{
class Entity;
class Scene;
class ActionUpdateSystem;
class ActionComponent : public Component
{
public:
    const static DAVA::FastName ACTION_COMPONENT_SELF_ENTITY_NAME;

    class Task
    {
    public:
        enum eType
        {
            None = 0,
            StartParticles,
            StartSound,
            TriggerWave,
            StopParticles,
            StopSound,
            MotionStart,
            MotionStop,
            SetSwitch,
            ActivateState,
            TriggerCustomEvent,
            TriggerUIEvent,
            StartHighlightAnimation,
            StartFadeIn,
            StartFadeOut,
            MotionSetFrame,
            MotionPause,
            MotionResume,
            MotionUpdateSpeed,
            Count,
        };
    };

    class Event
    {
    public:
        enum eType
        {
            SwitchChanged = 0,
            EntityAddedToScene,
            Custom,
            MotionFragmentFinished,
            MotionFragmentStarted,
            ParticlesStarted,
            ParticlesStopped,
            MotionSequenceStarted,
            MotionSequenceFinished,
            MotionInterrupted,
            Input,
            StateActivated,
            TriggerEntered,
            TriggerLeft,
            MotionSetFrame,
            MotionSetSpeed,
            MotionPaused,
            MotionResumed,
            Count
        };
    };

    struct Action : public InspBase
    {
        

        

        Task::eType type;
        Event::eType eventType;
        FastName userEventId;
        int32 switchIndex;
        float32 delay;
        float32 delayVariation;
        float32 actualDelay;
        FastName entityName;
        //VI: properties needed to control particle effect
        int32 stopAfterNRepeats;
        bool stopWhenEmpty;

        Action()
            : type(Task::eType::None)
            , eventType(Event::eType::SwitchChanged)
            , userEventId("")
            , switchIndex(-1)
            , delay(0.0f)
            , delayVariation(0.0f)
            , actualDelay(0.0f)
            , stopAfterNRepeats(-1)
            , stopWhenEmpty(false)
        {
        }

        bool operator==(const Action& other) const;

        void actualizeDelay();

        DAVA_VIRTUAL_REFLECTION(Action, InspBase);
    };

protected:
    virtual ~ActionComponent();

public:
    ActionComponent();

    void StartSwitch(int32 switchIndex = -1);
    void StartAdd();
    void StartUser(const FastName& name);
    bool IsStarted();
    void StopAll();
    void StopSwitch(int32 switchIndex = -1);
    void StopUser(const FastName& name);

    void Add(const ActionComponent::Action& action);
    void Remove(const ActionComponent::Action& action);
    void Remove(const ActionComponent::Task::eType type, const FastName& entityName, const int switchIndex);
    uint32 GetCount();
    ActionComponent::Action& Get(uint32 index);

    void Update(float32 timeElapsed);

    Component* Clone(Entity* toEntity) override;
    void Serialize(KeyedArchive* archive, SerializationContext* serializationContext) override;
    void Deserialize(KeyedArchive* archive, SerializationContext* serializationContext) override;

    static ActionComponent::Action MakeAction(ActionComponent::Task::eType type, const FastName& targetName, float32 delay);
    static ActionComponent::Action MakeAction(ActionComponent::Task::eType type, const FastName& targetName, float32 delay, int32 switchIndex);

    struct ActionContainer : public InspBase
    {
        Action action;
        float32 timer;
        bool active;
        bool markedForUpdate;

        ActionContainer()
            : timer(0.0f)
            , active(false)
            , markedForUpdate(false)
        {
        }

        ActionContainer(const Action& srcAction)
            : timer(0.0f)
            , active(false)
            , markedForUpdate(false)
        {
            action = srcAction;
        }

        bool operator==(const ActionContainer& container) const;

        DAVA_VIRTUAL_REFLECTION(ActionContainer, InspBase);
    };

private:
    void EvaluateAction(const Action& action);

    void OnActionParticleEffectStart(const Action& action);
    void OnActionParticleEffectStop(const Action& action);
    void OnActionAnimationStart(const Action& action);
    void OnActionAnimationStop(const Action& action);
    void OnActionSoundStart(const Action& action);
    void OnActionSoundStop(const Action& action);
    void OnActionWave(const Action& action);

    Entity* GetTargetEntity(const FastName& name, Entity* parent);

    Vector<ActionComponent::ActionContainer> actions;
    bool started;
    bool allActionsActive; //skip processing when all actions are active

    DAVA_VIRTUAL_REFLECTION(ActionComponent, Component);
};

template <>
bool AnyCompare<ActionComponent::ActionContainer>::IsEqual(const Any&, const Any&);
extern template struct AnyCompare<ActionComponent::ActionContainer>;

template <>
bool AnyCompare<ActionComponent::Action>::IsEqual(const Any&, const Any&);
extern template struct AnyCompare<ActionComponent::Action>;
};
