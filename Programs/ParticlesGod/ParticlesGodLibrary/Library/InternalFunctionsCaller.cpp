#include "InternalFunctionsCaller.h"

void InternalFunctionsCaller::Init(Config& config)
{
    ADRESS_DAVA__ParticleEmitter__LoadEmitterFromFile = config.ADRESS_DAVA__ParticleEmitter__LoadEmitterFromFile;
    ADRESS_DAVA__ParticleSerializer__SaveEmitter = config.ADRESS_DAVA__ParticleSerializer__SaveEmitter;
    ADRESS_DAVA__KeyedArchive__KeyedArchive = config.ADRESS_DAVA__KeyedArchive__KeyedArchive;
    ADRESS_DAVA__KeyedArchive__Save = config.ADRESS_DAVA__KeyedArchive__Save;
    ADRESS_DAVA__FilePath__FilePath = config.ADRESS_DAVA__FilePath__FilePath;
}

void* InternalFunctionsCaller::DAVA__ParticleEmitter__LoadEmitterFromFile(void* filename, bool preserveInheritPosition)
{
    typedef void*(__cdecl* _Function)(void* param_1, bool param_2);
    _Function function = (_Function)((uintptr_t)ADRESS_DAVA__ParticleEmitter__LoadEmitterFromFile);
    return function(filename, preserveInheritPosition);
}

void InternalFunctionsCaller::DAVA__ParticleSerializer__SaveEmitter(void* emitter, void* archive)
{
    typedef void(__cdecl * _Function)(void* param_1, void* param_2);
    _Function function = (_Function)((uintptr_t)ADRESS_DAVA__ParticleSerializer__SaveEmitter);
    function(emitter, archive);
}

void InternalFunctionsCaller::DAVA__KeyedArchive__KeyedArchive(void* param_this)
{
    typedef void(__thiscall * _Function)(void* param_1);
    _Function function = (_Function)((uintptr_t)ADRESS_DAVA__KeyedArchive__KeyedArchive);
    function(param_this);
}

bool InternalFunctionsCaller::DAVA__KeyedArchive__Save(void* param_this, void* pathName)
{
    typedef bool(__thiscall * _Function)(void* param_1, void* param_2);
    _Function function = (_Function)((uintptr_t)ADRESS_DAVA__KeyedArchive__Save);
    return function(param_this, pathName);
}

void InternalFunctionsCaller::DAVA__FilePath__FilePath(void* param_this, const char* pathname)
{
    typedef void(__thiscall * _Function)(void* param_1, const char* param_2);
    _Function function = (_Function)((uintptr_t)ADRESS_DAVA__FilePath__FilePath);
    function(param_this, pathname);
}