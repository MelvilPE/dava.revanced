#include "UI/MainWindow.h"
#include <QtHelpers/LauncherListener.h>

#include <DocDirSetup/DocDirSetup.h>

#include <Engine/Engine.h>
#include <Engine/EngineContext.h>
#include <FileSystem/FileSystem.h>
#include <FileSystem/KeyedArchive.h>
#include <Logger/Logger.h>

#include <QApplication>
#include <QCryptographicHash>

int Process(DAVA::Engine& e)
{
    using namespace DAVA;

    const EngineContext* context = e.GetContext();
    context->logger->SetLogLevel(DAVA::Logger::LEVEL_FRAMEWORK);

    Vector<char*> argv = e.GetCommandLineAsArgv();
    int argc = static_cast<int>(argv.size());

    QApplication a(argc, argv.data());

    DAVA::FileSystem* fs = context->fileSystem;
    DocumentsDirectorySetup::SetApplicationDocDirectory(fs, "FormatEditor");

    std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>();
    mainWindow->OnFirstLaunch();

    QObject::connect(&a, &QApplication::aboutToQuit, [&]()
    {
        mainWindow.reset();
    });

    LauncherListener launcherListener;
    launcherListener.Init([](LauncherListener::eMessage message)
                          {
        if (message == LauncherListener::eMessage::QUIT)
        {
            Logger::Info("Got quit message from launcher listener");
            qApp->quit();
            return LauncherListener::eReply::ACCEPT;
        }
        return LauncherListener::eReply::UNKNOWN_MESSAGE; });

    return a.exec();
}

int DAVAMain(DAVA::Vector<DAVA::String> cmdLine)
{
    using namespace DAVA;

    Vector<String> modules = {};

    KeyedArchive* options = new KeyedArchive;

    Engine e;
    e.Init(eEngineRunMode::CONSOLE_MODE, modules, options);

    e.update.Connect([&e](float32)
    {
        int result = Process(e);
        e.QuitAsync(result);
    });

    return e.Run();
}
