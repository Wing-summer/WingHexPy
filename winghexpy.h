#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "../../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"
#include "plginterface.h"
#include "scriptmanager.h"
#include <Python.h>
#include <QListWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTreeWidget>

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
  void
  registerDockWidget(QMap<QDockWidget *, Qt::DockWidgetArea> &rdw) override;
  QString pluginName() override;
  QString pluginAuthor() override;
  uint pluginVersion() override;
  QString signature() override;
  QString pluginComment() override;
  void plugin2MessagePipe(WingPluginMessage type, QList<QVariant> msg) override;

public:
  void log(QString message);
  void on_about();
  void on_sponsor();

public:
  void RunPyFile(QString filename);
  void RunPyText(QString content);

private:
  PlgInterface *plgint;
  QMenu *menu;
  QToolBar *tb;
  QDockWidget *dw;
  PythonQtScriptingConsole *txt;
  ScriptManager *smanager;

private:
  QDockWidget *dlist, *dtree, *dtable, *dtxt;
  QListWidget *infolist;
  QTreeWidget *infotree;
  QTableWidget *infotable;
  QTextBrowser *infotxt;

private:
  void runPyFile();
};

#endif // GENERICPLUGIN_H
