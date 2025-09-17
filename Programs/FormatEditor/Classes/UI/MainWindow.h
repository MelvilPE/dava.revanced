#pragma once

#include <memory>
#include <QMainWindow>
#include <QSystemTrayIcon>

#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QSettings>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFile>
#include <QTextStream>

#include <CodeEditor/QCodeEditor.h>
#include <CodeEditor/QGLSLCompleter.h>
#include <CodeEditor/QLuaCompleter.h>
#include <CodeEditor/QPythonCompleter.h>
#include <CodeEditor/QSyntaxStyle.h>
#include <CodeEditor/QCXXHighlighter.h>
#include <CodeEditor/QGLSLHighlighter.h>
#include <CodeEditor/QXMLHighlighter.h>
#include <CodeEditor/QJSONHighlighter.h>
#include <CodeEditor/QLuaHighlighter.h>
#include <CodeEditor/QPythonHighlighter.h>

class QMenu;
class QVBoxLayout;

namespace Ui
{
class MainWindowWidget;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
    void OnFirstLaunch();

private slots:
    void OnTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void CreateTrayMenu();

    // QCodeEditor
    void LoadStyle(QString path);
    QString LoadCode(QString path);
    void InitData();
    void CreateWidgets();
    void SetupWidgets();
    QVBoxLayout* m_setupLayout;
    QCodeEditor* m_codeEditor;
    QVector<QPair<QString, QString>> m_codeSamples;
    QVector<QPair<QString, QCompleter*>> m_completers;
    QVector<QPair<QString, QStyleSyntaxHighlighter*>> m_highlighters;
    QVector<QPair<QString, QSyntaxStyle*>> m_styles;

    void LoadFromKeyedArchive();
    void LoadFromSceneFile();
    void SaveToKeyedArchive();
    void SaveToSceneFile();
    void LoadFromYamlFile();
    void SaveToYamlFile();

    void ShowErrorMessage(std::string message);

private:
    std::unique_ptr<Ui::MainWindowWidget> ui;
    QSystemTrayIcon* appIcon = nullptr;
};
