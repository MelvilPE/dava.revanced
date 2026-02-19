#pragma once
#include "Includes.h"
#include "Config.h"

class InternalFunctionsCaller
{
public:
    void Init(Config& config);
    /**
    * DAVA::ParticleEmitter *__cdecl DAVA::ParticleEmitter::LoadEmitterFromFile(
    *   DAVA::FilePath *filename,
    *   bool preserveInheritPosition)
    */
    void* DAVA__ParticleEmitter__LoadEmitterFromFile(void* filename, bool preserveInheritPosition);

    /**
    * void __cdecl DAVA::ParticleSerializer::SaveEmitter(const DAVA::ParticleEmitter *emitter, DAVA::KeyedArchive *archive)
    */
    void DAVA__ParticleSerializer__SaveEmitter(void* emitter, void* archive);

    /**
     * void __thiscall DAVA::KeyedArchive::KeyedArchive(DAVA::KeyedArchive *this)
     */
    void DAVA__KeyedArchive__KeyedArchive(void* param_this);

    /**
     * bool __thiscall DAVA::KeyedArchive::Save(DAVA::KeyedArchive *this, const DAVA::FilePath *pathName)
     */
    bool DAVA__KeyedArchive__Save(void* param_this, void* pathName);

    /**
    * void __thiscall DAVA::FilePath::FilePath(DAVA::FilePath *this, const char *pathname)
    */
    void DAVA__FilePath__FilePath(void* param_this, const char* pathname);

private:
    uintptr_t ADRESS_DAVA__ParticleEmitter__LoadEmitterFromFile = NULL;
    uintptr_t ADRESS_DAVA__ParticleSerializer__SaveEmitter = NULL;
    uintptr_t ADRESS_DAVA__KeyedArchive__KeyedArchive = NULL;
    uintptr_t ADRESS_DAVA__KeyedArchive__Save = NULL;
    uintptr_t ADRESS_DAVA__FilePath__FilePath = NULL;
};