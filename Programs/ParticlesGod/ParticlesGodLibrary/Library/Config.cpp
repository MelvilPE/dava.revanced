#include "Config.h"

bool Config::Parse(rapidjson::Document& doc)
{
    if (!doc.HasMember("emitterFilePaths") ||
        !doc.HasMember("functions") ||
        !doc.HasMember("debugLibrary"))
    {
        return false;
    }

    if (!doc["emitterFilePaths"].IsArray() ||
        !doc["functions"].IsObject() ||
        !doc["debugLibrary"].IsBool())
    {
        return false;
    }

    for (const auto& emitterPath : doc["emitterFilePaths"].GetArray())
    {
        EMITTER_PATHS.push_back(emitterPath.GetString());
    }

    std::set<std::string> requiredFunctions = {
        "ParticleEmitter__LoadEmitterFromFile",
        "ParticleSerializer__SaveEmitter",
        "KeyedArchive__KeyedArchive",
        "KeyedArchive__Save",
        "FilePath__FilePath"
    };

    const auto& funcs = doc["functions"].GetObject();

    for (const auto& name : requiredFunctions)
    {
        if (!funcs.HasMember(name.c_str()) || !funcs[name.c_str()].IsString())
        {
            return false;
        }
    }

    AOB_DAVA__ParticleEmitter__LoadEmitterFromFile = funcs["ParticleEmitter__LoadEmitterFromFile"].GetString();
    AOB_DAVA__ParticleSerializer__SaveEmitter = funcs["ParticleSerializer__SaveEmitter"].GetString();
    AOB_DAVA__KeyedArchive__KeyedArchive = funcs["KeyedArchive__KeyedArchive"].GetString();
    AOB_DAVA__KeyedArchive__Save = funcs["KeyedArchive__Save"].GetString();
    AOB_DAVA__FilePath__FilePath = funcs["FilePath__FilePath"].GetString();

    ADRESS_DAVA__ParticleEmitter__LoadEmitterFromFile = AOB::Scan(AOB::SplitPattern(AOB_DAVA__ParticleEmitter__LoadEmitterFromFile));
    ADRESS_DAVA__ParticleSerializer__SaveEmitter = AOB::Scan(AOB::SplitPattern(AOB_DAVA__ParticleSerializer__SaveEmitter));
    ADRESS_DAVA__KeyedArchive__KeyedArchive = AOB::Scan(AOB::SplitPattern(AOB_DAVA__KeyedArchive__KeyedArchive));
    ADRESS_DAVA__KeyedArchive__Save = AOB::Scan(AOB::SplitPattern(AOB_DAVA__KeyedArchive__Save));
    ADRESS_DAVA__FilePath__FilePath = AOB::Scan(AOB::SplitPattern(AOB_DAVA__FilePath__FilePath));

    if (ADRESS_DAVA__ParticleEmitter__LoadEmitterFromFile == NULL ||
        ADRESS_DAVA__ParticleSerializer__SaveEmitter == NULL ||
        ADRESS_DAVA__KeyedArchive__KeyedArchive == NULL ||
        ADRESS_DAVA__KeyedArchive__Save == NULL ||
        ADRESS_DAVA__FilePath__FilePath == NULL)
    {
        return false;
    }

    return true;
}