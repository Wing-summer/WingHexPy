#ifndef PLGINTERFACE_H
#define PLGINTERFACE_H

#define PY_SSIZE_T_CLEAN

#include "../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include "PythonQt/PythonQt.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"
#include <QListWidget>
#include <QMutex>
#include <QObject>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTreeWidget>

#define INFOLOG(msg) "<font color=\"green\">" + msg + "</font>"
#define ERRLOG(msg) "<font color=\"red\">" + msg + "</font>"
#define WARNLOG(msg) "<font color=\"gold\">" + msg + "</font>"

class PlgInterface : public QObject {
  Q_OBJECT
public:
  explicit PlgInterface(IWingPlugin *parent);

signals:
  void log(QString message);

public:
  bool init();
  void initInfo(QListWidget *infolist, QTreeWidget *infotree,
                QTableWidget *infotable, QTextBrowser *infotxt);
  PythonQtScriptingConsole *getScriptingConsole();
  bool RunPyFile(QString filename);
  bool RunPyText(QString content);
  bool requestControl(int timeout = 1500);
  bool requestRelease();
  bool hasControl();
  void toast(QIcon icon, QString message);

  static PlgInterface *instance();
private slots:

private:
  IWingPlugin *plg;
  PythonQtObjectPtr mainContext;
  PythonQtScriptingConsole *console;

  QMutex mutex;

private:
  static PlgInterface *m_instace;
};

class PlgService : public QObject {
  Q_OBJECT
public:
  PlgService() {}
  PlgService(PlgInterface *parent) : inter(parent) {}

public slots:
  bool requestControl(int timeout = 1500) {
    return inter->requestControl(timeout);
  }
  bool requestRelease() { return inter->requestRelease(); }
  bool hasControl() { return inter->hasControl(); }
  void toast(QIcon icon, QString message) { inter->toast(icon, message); }
  void toast(QString message) {
    inter->toast(QIcon(":/img/icon.png"), message);
  }

  void print(QString message) { inter->log(message); }
  void errPrint(QString message) { inter->log(ERRLOG(message)); }
  void infoPrint(QString message) { inter->log(INFOLOG(message)); }
  void warnPrint(QString message) { inter->log(WARNLOG(message)); }

private:
  PlgInterface *inter;
};

#endif // PLGINTERFACE_H
