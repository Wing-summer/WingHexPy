#ifndef PLGINTERFACE_H
#define PLGINTERFACE_H

#define PY_SSIZE_T_CLEAN

#include "../WingHexExplorer/WingHexExplorer/plugin/iwingplugin.h"
#include "PythonQt/PythonQt.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"
#include <QApplication>
#include <QClipboard>
#include <QListWidget>
#include <QMutex>
#include <QObject>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTreeWidget>

#define INFOLOG(msg) "<font color=\"green\">" + msg + "</font>"
#define ERRLOG(msg) "<font color=\"red\">" + msg + "</font>"
#define WARNLOG(msg) "<font color=\"gold\">" + msg + "</font>"

#define WingHexPyVersion 5

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
  bool RunPyFile(const QString &filename);
  bool RunPyText(const QString &content);
  bool requestControl(int timeout = 1500);
  bool requestRelease();
  bool hasControl();
  void toast(const QIcon &icon, const QString &message);
  void logWarn(const QString &message);
  void logError(const QString &message);
  void logInfo(const QString &message);
  void logDebug(const QString &message);

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
  void toast(const QIcon &icon, const QString &message) {
    inter->toast(icon, message);
  }
  void toast(const QString &message) {
    inter->toast(QIcon(":/WingHexPy/img/icon.png"), message);
  }
  void logWarn(const QString &message) { inter->logWarn(message); }
  void logError(const QString &message) { inter->logError(message); }
  void logInfo(const QString &message) { inter->logInfo(message); }
  void logDebug(const QString &message) { inter->logDebug(message); }
  void print(QString message) { inter->log(message); }
  void errPrint(QString message) { inter->log(ERRLOG(message)); }
  void infoPrint(QString message) { inter->log(INFOLOG(message)); }
  void warnPrint(QString message) { inter->log(WARNLOG(message)); }

  void copy2Clipboard(QString content) {
    QApplication::clipboard()->setText(content);
  }
  QString getClipboardText() { return QApplication::clipboard()->text(); }

  int version() { return WingHexPyVersion; }

private:
  PlgInterface *inter;
};

#endif // PLGINTERFACE_H
