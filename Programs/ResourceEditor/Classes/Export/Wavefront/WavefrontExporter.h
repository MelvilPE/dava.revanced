#pragma once

#include "TArc/Core/ClientModule.h"
#include "TArc/Utils/QtConnections.h"
#include "Reflection/ReflectionRegistrator.h"

class WavefrontExporter : public DAVA::ClientModule
{
protected:
    void PostInit() override;
    void Export();

private:
    DAVA::QtConnections connections;

    DAVA_VIRTUAL_REFLECTION_IN_PLACE(WavefrontExporter, DAVA::ClientModule)
    {
        DAVA::ReflectionRegistrator<WavefrontExporter>::Begin()
        .ConstructorByPointer()
        .End();
    }
};
