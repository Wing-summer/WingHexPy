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
                QTableWidget *infotable);
  PythonQtScriptingConsole *getScriptingConsole();
  bool RunPyFile(QString filename);
  bool RunPyText(QString content);
  bool requestControl(int timeout = 1500);
  bool requestRelease();

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

  void print(QString message) { inter->log(message); }
  void errPrint(QString message) { inter->log(ERRLOG(message)); }
  void infoPrint(QString message) { inter->log(INFOLOG(message)); }
  void warnPrint(QString message) { inter->log(WARNLOG(message)); }

private:
  PlgInterface *inter;
};

class InfoListWrapper : public QObject {
  Q_OBJECT
public:
  InfoListWrapper() {}
  InfoListWrapper(QListWidget *list) : m_list(list) {}

  void addItem(QString content) { m_list.addItem(content); }
  void addIconItem(QIcon icon, QString content) {
    m_list.addItem(new QListWidgetItem(icon, content));
  }
  void clear() { m_list.clear(); }
  void insertItem(int index, QString content) {
    m_list.insertItem(index, content);
  }
  void insertIconItem(int index, QIcon icon, QString content) {
    m_list.insertItem(index, new QListWidgetItem(icon, content));
  }
  void removeAt(int index) {
    auto item = m_list.item(index);
    m_list.removeItemWidget(item);
    delete item;
  }

private:
  QListWidget m_list;
};

class InfoTableWrapper : public QObject {
  Q_OBJECT
public:
  InfoTableWrapper() {}
  InfoTableWrapper(QTableWidget *table) : m_table(table) {}

  void setColumnCount(int count) { m_table->setColumnCount(count); }
  void setRowCount(int count) { m_table->setRowCount(count); }
  void setRowHeight(int row, int height) { m_table->setRowHeight(row, height); }
  void setColumnWidth(int col, int width) {
    m_table->setColumnWidth(col, width);
  }
  void clear() { m_table->clear(); }
  void clearContents() { m_table->clearContents(); }
  void setItem(int row, int col, QString content) {
    m_table->setItem(row, col, new QTableWidgetItem(content));
  }
  void setIconItem(int row, int col, QIcon icon, QString content) {
    m_table->setItem(row, col, new QTableWidgetItem(icon, content));
  }
  void hideRow(int row) { m_table->hideRow(row); }
  void hideCol(int col) { m_table->hideColumn(col); }
  void setHorizontalHeaderLabels(QStringList lbls) {
    m_table->setHorizontalHeaderLabels(lbls);
  }
  void setVerticalHeaderLabels(QStringList lbls) {
    m_table->setVerticalHeaderLabels(lbls);
  }
  void setHorizontalHeaderLabel(int index, QString lbl) {
    m_table->setHorizontalHeaderItem(index, new QTableWidgetItem(lbl));
  }
  void setVerticalHeaderLabel(int index, QString lbl) {
    m_table->setVerticalHeaderItem(index, new QTableWidgetItem(lbl));
  }
  void setCurrentCell(int row, int col) { m_table->setCurrentCell(row, col); }

private:
  QTableWidget *m_table;
};

class InfoTreeWrapper : public QObject {
  Q_OBJECT
public:
  InfoTreeWrapper() {}
  InfoTreeWrapper(QTreeWidget *tree) : m_tree(tree) {}

private:
  QTreeWidget *m_tree;
};

#endif // PLGINTERFACE_H
