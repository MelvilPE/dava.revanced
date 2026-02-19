#pragma once
#include "Includes.h"
#include "AOB.h"

class Config
{
public:
    bool Parse(rapidjson::Document& doc);

    uintptr_t TYPESIZE_KEYED_ARCHIVE = 0x2C;
    uintptr_t TYPESIZE_FILE_PATH = 0x1C;

    std::vector<std::string> EMITTER_PATHS = {};

    std::string AOB_DAVA__ParticleEmitter__LoadEmitterFromFile = "";
    std::string AOB_DAVA__ParticleSerializer__SaveEmitter = "";
    std::string AOB_DAVA__KeyedArchive__KeyedArchive = "";
    std::string AOB_DAVA__KeyedArchive__Save = "";
    std::string AOB_DAVA__FilePath__FilePath = "";

    uintptr_t ADRESS_DAVA__ParticleEmitter__LoadEmitterFromFile = NULL;
    uintptr_t ADRESS_DAVA__ParticleSerializer__SaveEmitter = NULL;
    uintptr_t ADRESS_DAVA__KeyedArchive__KeyedArchive = NULL;
    uintptr_t ADRESS_DAVA__KeyedArchive__Save = NULL;
    uintptr_t ADRESS_DAVA__FilePath__FilePath = NULL;
};