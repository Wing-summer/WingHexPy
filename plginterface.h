#ifndef PLGINTERFACE_H
#define PLGINTERFACE_H

#define PY_SSIZE_T_CLEAN

#include "../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include "PythonQt/PythonQt.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"
#include <QObject>

#define INFOLOG(msg) "<font color=\"green\">" + msg + "</font>"
#define ERRLOG(msg) "<font color=\"red\">" + msg + "</font>"
#define WARNLOG(msg) "<font color=\"yellow\">" + msg + "</font>"

class PlgInterface : public QObject {
  Q_OBJECT
public:
  explicit PlgInterface(IWingPlugin *parent);

signals:
  void log(QString message);

public:
  bool init();
  PythonQtScriptingConsole *getScriptingConsole();
  void RunPyFile(QString filename);
  void RunPyText(QString content);
  bool requestControl(int timeout = 1500);
  bool requestRelease();

  static PlgInterface *instance();
private slots:

private:
  IWingPlugin *plg;
  PythonQtObjectPtr mainContext;
  PythonQtScriptingConsole *console;

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

  void print(QString message) { inter->log(message); }
  void errPrint(QString message) { inter->log(ERRLOG(message)); }
  void infoPrint(QString message) { inter->log(INFOLOG(message)); }
  void warnPrint(QString message) { inter->log(WARNLOG(message)); }

private:
  PlgInterface *inter;
};

#endif // PLGINTERFACE_H
