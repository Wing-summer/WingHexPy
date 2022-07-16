#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"
#include "plginterface.h"
#include <Python.h>
#include <QTextBrowser>

class WingHexPy : public IWingPlugin {
  Q_OBJECT

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID IWINGPLUGIN_INTERFACE_IID FILE "WingHexPy.json")
#endif // QT_VERSION >= 0x050000

  Q_INTERFACES(IWingPlugin)

public:
  WingHexPy(QObject *parent = nullptr);
  int sdkVersion() override;
  bool init(QList<WingPluginInfo> loadedplugin) override;
  ~WingHexPy() override;
  void unload() override;
  QMenu *registerMenu() override;
  QToolBar *registerToolBar() override;
  QDockWidget *registerDockWidget() override;
  QString pluginName() override;
  QString pluginAuthor() override;
  uint pluginVersion() override;
  QString signature() override;
  QString pluginComment() override;
  void plugin2MessagePipe(WingPluginMessage type, QList<QVariant> msg) override;
  Qt::DockWidgetArea registerDockWidgetDockArea() override;

public:
  void log(QString message);

public:
  void RunPyFile(QString filename);
  void RunPyText(QString content);

private:
  PlgInterface *plgint;
  QMenu *menu;
  QToolBar *tb;
  QDockWidget *dw;
  PythonQtScriptingConsole *txt;
};

#endif // GENERICPLUGIN_H
