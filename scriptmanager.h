#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include "../../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include "plginterface.h"
#include <QMap>
#include <QObject>

struct ScriptMeta {
  QString name;
  QString filename;
  QString author;
  QString license;
  QString commnet;
  uint version;
};

Q_DECLARE_METATYPE(ScriptMeta)

class ScriptManager : public QObject {
  Q_OBJECT
public:
  static ScriptManager *instance();
  void loadMenu(QMenu *menu);
  void loadTreeWidget(QTreeWidget *tree);
  void parseMeta(QFileInfo fileinfo, QString folder, QList<ScriptMeta> &m);
  static ScriptManager *m_instance;

private:
  ScriptManager(QObject *parent = nullptr);
  QMap<QString, QList<ScriptMeta>> m_metas;
};

#endif // SCRIPTMANAGER_H
