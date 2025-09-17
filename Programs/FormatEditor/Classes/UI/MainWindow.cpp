#include "MainWindow.h"
#include "ui_MainWindowWidget.h"

#include <Version/Version.h>

#include "FileSystem/KeyedArchive.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/File.h"
#include "Logger/Logger.h"
#include "Utils/StringFormat.h"
#include "Job/JobManager.h"
#include "Scene3D/SceneFileV2.h"

#if defined(__DAVAENGINE_WINDOWS__)
#include <QSettings>
#elif defined(__DAVAENGINE_MACOS__)
#include <QXmlStreamReader>
#endif

#include "FileSystem/DynamicMemoryFile.h"
#include "FileSystem/UnmanagedMemoryFile.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowWidget)
{
    setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowFullscreenButtonHint);
    ui->setupUi(this);

    InitData();
    CreateWidgets();
    SetupWidgets();

    DAVA::String title = DAVA::Version::CreateAppVersion("FormatEditor");
    setWindowTitle(QString::fromStdString(title));

    CreateTrayMenu();

    setMinimumWidth(800);
}

MainWindow::~MainWindow()
{
    if (appIcon)
    {
        appIcon->hide();
    }
}

void MainWindow::CreateTrayMenu()
{
    QIcon windowIcon(":/icon.png");
    setWindowIcon(windowIcon);

    appIcon = new QSystemTrayIcon(this);
    appIcon->setIcon(windowIcon);
    appIcon->setToolTip("FormatEditor");
    appIcon->show();

    connect(appIcon, &QSystemTrayIcon::activated, this, &MainWindow::OnTrayIconActivated);
}


void MainWindow::OnFirstLaunch()
{
    show();
}

void MainWindow::OnTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
    {
        showNormal();
        activateWindow();
        raise();
        break;
    }
    default:
        break;
    }
}

void MainWindow::InitData()
{
    m_codeSamples = {
        { "None", nullptr },
        { "C++",  LoadCode(":/cpp.xml")},
        { "GLSL", LoadCode(":/shader.glsl") },
        { "XML", LoadCode(":/xml.xml") },
        { "JSON", LoadCode(":/json.json") },
        { "LUA", LoadCode(":/lua.lua") },
        { "Python", LoadCode(":/python.py") }
    };

    m_completers = {
        {"None", nullptr},
        {"GLSL", new QGLSLCompleter(this)},
        {"LUA", new QLuaCompleter(this)},
        {"Python", new QPythonCompleter(this)},
    };

    m_highlighters = {
        {"None", nullptr},
        {"C++",  new QCXXHighlighter},
        {"GLSL", new QGLSLHighlighter},
        {"XML",  new QXMLHighlighter},
        {"JSON", new QJSONHighlighter},
        {"LUA",  new QLuaHighlighter},
        {"Python",  new QPythonHighlighter},
    };

    m_styles = {
        {"Default", QSyntaxStyle::defaultStyle()}
    };

    LoadStyle(":/drakula.xml");
}

QString MainWindow::LoadCode(QString path)
{
    QFile fl(path);

    if (!fl.open(QIODevice::ReadOnly))
    {
        return QString();
    }

    return fl.readAll();
}

void MainWindow::LoadStyle(QString path)
{
    QFile fl(path);

    if (!fl.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto style = new QSyntaxStyle(this);

    if (!style->load(fl.readAll()))
    {
        delete style;
        return;
    }

    m_styles.append({style->name(), style});
}

void MainWindow::CreateWidgets()
{
    QWidget* codeEditorWidget = new QWidget(ui->frameMain);

    m_codeEditor = new QCodeEditor(this);
    QVBoxLayout* codeEditorLayout = new QVBoxLayout(codeEditorWidget);
    codeEditorLayout->setContentsMargins(0, 0, 0, 0);
    codeEditorLayout->setSpacing(0);
    codeEditorLayout->addWidget(m_codeEditor);

    QWidget* actionsWidget = new QWidget(ui->frameMain);
    actionsWidget->setFixedHeight(40);

    QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setSpacing(5);

    QPushButton* button1 = new QPushButton("Load from KeyedArchive", actionsWidget);
    QPushButton* button2 = new QPushButton("Load from SceneFile", actionsWidget);
    QPushButton* button3 = new QPushButton("Save to KeyedArchive", actionsWidget);
    QPushButton* button4 = new QPushButton("Save to SceneFile", actionsWidget);
    QPushButton* button5 = new QPushButton("Load from Yaml", actionsWidget);
    QPushButton* button6 = new QPushButton("Save to Yaml", actionsWidget);

    connect(button1, &QPushButton::clicked, this, &MainWindow::LoadFromKeyedArchive);
    connect(button2, &QPushButton::clicked, this, &MainWindow::LoadFromSceneFile);
    connect(button3, &QPushButton::clicked, this, &MainWindow::SaveToKeyedArchive);
    connect(button4, &QPushButton::clicked, this, &MainWindow::SaveToSceneFile);
    connect(button5, &QPushButton::clicked, this, &MainWindow::LoadFromYamlFile);
    connect(button6, &QPushButton::clicked, this, &MainWindow::SaveToYamlFile);

    actionsLayout->addWidget(button1);
    actionsLayout->addWidget(button2);
    actionsLayout->addWidget(button3);
    actionsLayout->addWidget(button4);
    actionsLayout->addWidget(button5);
    actionsLayout->addWidget(button6);

    actionsWidget->setLayout(actionsLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(ui->frameMain);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(codeEditorWidget);
    mainLayout->addWidget(actionsWidget);

    ui->frameMain->setLayout(mainLayout);
}

void MainWindow::SetupWidgets()
{
    m_codeEditor->setPlainText(m_codeSamples[0].second);
    m_codeEditor->setSyntaxStyle(m_styles[1].second);
    m_codeEditor->setCompleter(m_completers[0].second);
    m_codeEditor->setHighlighter(m_highlighters[0].second);
}

void MainWindow::LoadFromKeyedArchive()
{
    using namespace DAVA;

    QString archivePath = QFileDialog::getOpenFileName(nullptr, "Select KeyedArchive file", "", "KeyedArchive (*.ka)");
    FilePath archiveFilePath(archivePath.toStdString());

    if (archiveFilePath.IsEmpty() ||
       !archiveFilePath.IsEqualToExtension(".ka") ||
       !archiveFilePath.Exists())
    {
        return;
    }

    ScopedPtr<KeyedArchive> archive(new KeyedArchive());
    if (!archive->Load(archiveFilePath))
    {
        ShowErrorMessage("Failed to load KeyedArchive!");
        return;
    }

    String content = archive->SaveToYamlString();
    if (content == "")
    {
        ShowErrorMessage("Failed to create KeyedArchive yaml representation!");
        return;
    }

    m_codeEditor->setPlainText(QString(content.c_str()));
}

void MainWindow::LoadFromSceneFile()
{
    using namespace DAVA;

    QString tmp = QFileDialog::getOpenFileName(nullptr, "Select SceneFileV2 file", "", "SceneFileV2 (*.sc2)");
    FilePath sceneFilePath(tmp.toStdString());

    if (sceneFilePath.IsEmpty() ||
        !sceneFilePath.IsEqualToExtension(".sc2") ||
        !sceneFilePath.Exists())
    {
        return;
    }

    const ScopedPtr<File> file(File::Create(sceneFilePath, File::OPEN | File::READ));
    if (!file)
    {
        ShowErrorMessage("Failed to open SceneFile!");
        return;
    }

    SceneFileV2::Header header;
    const bool headerValid = SceneFileV2::ReadHeader(header, file);
    if (!headerValid)
    {
        ShowErrorMessage("Failed to read SceneFile header!");
        return;
    }

    if (header.version < WORLD_OF_TANKS_BLITZ_7_8_VERSION)
    {
        ShowErrorMessage("Failed to read SceneFile header - version is too old for this tool (30+ required for now)!");
        return;
    }

    VersionInfo::SceneVersion dumb;
    dumb.version = header.version;

    const bool headerTagsValid = SceneFileV2::ReadVersionTags(dumb, file);
    if (!headerTagsValid)
    {
        ShowErrorMessage("Failed to read SceneFile header version tags!");
        return;
    }

    SceneFileV2::Descriptor descriptor;
    const bool descriptorValid = SceneFileV2::ReadDescriptor(file, descriptor);
    if (!descriptorValid)
    {
        ShowErrorMessage("Failed to read SceneFile descriptor!");
        return;
    }

    ScopedPtr<KeyedArchive> sceneArchive(new KeyedArchive());
    const bool loadedArchive = sceneArchive->Load(file);
    if (!loadedArchive)
    {
        ShowErrorMessage("Failed to read SceneFile scene archive!");
        return;
    }

    ScopedPtr<KeyedArchive> wholeArchive(new KeyedArchive());
    ScopedPtr<KeyedArchive> headeArchive(new KeyedArchive());
    ScopedPtr<KeyedArchive> descrArchive(new KeyedArchive());

    headeArchive->SetUInt32("version", header.version);
    headeArchive->SetUInt32("nodeCount", header.nodeCount);
    descrArchive->SetUInt32("size", descriptor.size);
    descrArchive->SetUInt32("fileType", descriptor.fileType);
    descrArchive->SetUInt64("geometryIdHash", descriptor.geometryIdHash);
    descrArchive->SetUInt64("geometryDataHash", descriptor.geometryDataHash);

    wholeArchive->SetArchive("header", headeArchive);
    wholeArchive->SetArchive("descriptor", descrArchive);
    wholeArchive->SetArchive("sceneArchive", sceneArchive);

    String content = wholeArchive->SaveToYamlString();
    if (content == "")
    {
        ShowErrorMessage("Failed to create KeyedArchive yaml representation!");
        return;
    }

    m_codeEditor->setPlainText(QString(content.c_str()));
}

void MainWindow::SaveToKeyedArchive()
{
    using namespace DAVA;

    String content = m_codeEditor->toPlainText().toStdString();

    ScopedPtr<KeyedArchive> archive(new KeyedArchive());
    if (!archive->LoadFromYamlString(content))
    {
        ShowErrorMessage("Failed to create KeyedArchive from yaml content!");
        return;
    }

    QString archivePath = QFileDialog::getSaveFileName(nullptr, "Save KeyedArchive file", "", "KeyedArchive (*.ka)");
    FilePath archiveFilePath(archivePath.toStdString());

    if (archiveFilePath.IsEmpty() ||
       !archiveFilePath.IsEqualToExtension(".ka"))
    {
        return;
    }

    if (!archive->Save(archiveFilePath))
    {
        ShowErrorMessage("Failed to save KeyedArchive!");
        return;
    }
}

void MainWindow::SaveToSceneFile()
{
    using namespace DAVA;

    String content = m_codeEditor->toPlainText().toStdString();

    ScopedPtr<KeyedArchive> wholeArchive(new KeyedArchive());
    if (!wholeArchive->LoadFromYamlString(content))
    {
        ShowErrorMessage("Failed to create KeyedArchive from yaml content!");
        return;
    }

    if (!wholeArchive->IsKeyExists("header") || !wholeArchive->IsKeyExists("descriptor"))
    {
        ShowErrorMessage("Header || VersionTags || Descriptor part is missing in KeyedArchive!");
        return;
    }

    KeyedArchive* headeArchive = wholeArchive->GetArchive("header");
    KeyedArchive* descrArchive = wholeArchive->GetArchive("descriptor");
    KeyedArchive* sceneArchive = wholeArchive->GetArchive("sceneArchive");

    SceneFileV2::Header header;
    header.signature = Array<char, 4>{ 'S', 'F', 'V', '2' };
    header.version = static_cast<int32>(headeArchive->GetUInt32("version"));
    header.nodeCount = headeArchive->GetUInt32("nodeCount", header.nodeCount);

    VersionInfo::SceneVersion version;
    version.version = header.version;

    SceneFileV2::Descriptor descriptor;
    descriptor.size = descrArchive->GetUInt32("size");
    descriptor.fileType = descrArchive->GetUInt32("fileType");
    descriptor.geometryIdHash = descrArchive->GetUInt64("geometryIdHash");
    descriptor.geometryDataHash = descrArchive->GetUInt64("geometryDataHash");

    QString tmp = QFileDialog::getSaveFileName(nullptr, "Select SceneFileV2 file", "", "SceneFileV2 (*.sc2)");
    FilePath sceneFilePath(tmp.toStdString());

    if (sceneFilePath.IsEmpty() ||
        !sceneFilePath.IsEqualToExtension(".sc2"))
    {
        return;
    }

    ScopedPtr<File> file(File::Create(sceneFilePath, File::CREATE | File::WRITE));
    if (!file)
    {
        ShowErrorMessage("Failed to create SceneFile!");
        return;
    }

    if (!SceneFileV2::WriteHeader(file, header))
    {
        ShowErrorMessage("Failed to write header to SceneFile!");
        return;
    }

    if (!SceneFileV2::WriteVersionTags(file))
    {
        ShowErrorMessage("Failed to write version tags to SceneFile!");
        return;
    }

    if (!SceneFileV2::WriteDescriptor(file, descriptor))
    {
        ShowErrorMessage("Failed to write descriptor to SceneFile!");
        return;
    }

    if (!sceneArchive->Save(file))
    {
        ShowErrorMessage("Failed to write scene archive to SceneFile!");
        return;
    }
}

void MainWindow::LoadFromYamlFile()
{
    using namespace DAVA;

    QString tmp = QFileDialog::getOpenFileName(nullptr, "Load From Yaml file", "", "Yaml (*.yaml)");
    FilePath yamlFilePath(tmp.toStdString());

    if (yamlFilePath.IsEmpty() || !yamlFilePath.IsEqualToExtension(".yaml"))
    {
        return;
    }

    QFile file(tmp.toStdString().c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    m_codeEditor->setPlainText(content);
}

void MainWindow::SaveToYamlFile()
{
    using namespace DAVA;

    String content = m_codeEditor->toPlainText().toStdString();

    QString tmp = QFileDialog::getSaveFileName(nullptr, "Save To Yaml file", "", "Yaml (*.yaml)");
    FilePath yamlFilePath(tmp.toStdString());

    if (yamlFilePath.IsEmpty() || !yamlFilePath.IsEqualToExtension(".yaml"))
    {
        return;
    }

    QFile file(tmp.toStdString().c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << m_codeEditor->toPlainText();
    file.close();
}

void MainWindow::ShowErrorMessage(std::string message)
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error", message.c_str());
    messageBox.setFixedSize(500, 200);
}