#include "scriptmanager.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

ScriptManager *ScriptManager::m_instance = nullptr;

ScriptManager *ScriptManager::instance() {
  if (m_instance == nullptr)
    m_instance = new ScriptManager;
  return m_instance;
}

ScriptManager::ScriptManager(QObject *parent) : QObject(parent) {
  auto dir = PLUGINDIR + "/winghexpys";
  QDir pdir(dir);
  if (!pdir.exists()) {
    return;
  }
  pdir.setFilter(QDir::Filter::Dirs);
  auto fdirs = pdir.entryInfoList();
  for (auto folder : fdirs) {
    auto na = folder.fileName();
    if (na == "." || na == "..")
      continue;
    QDir jdir(folder.filePath());
    QList<ScriptMeta> m;
    jdir.setFilter(QDir::Filter::Dirs);
    auto paks = jdir.entryInfoList();
    for (auto f : paks) {
      auto na = f.fileName();
      if (na == "." || na == "..")
        continue;
      auto fn = f.absoluteFilePath();
      QDir dir(fn);
      auto j = fn + "/" + f.fileName() + ".json";
      if (QFile::exists(j))
        parseMeta(QFileInfo(j), fn, m);
    }
    m_metas.insert(folder.fileName(), m);
  }
}

void ScriptManager::loadMenu(QMenu *menu) {
  if (!menu)
    return;
  if (!m_metas.count()) {
    auto a = new QAction(tr("NoScript"));
    a->setEnabled(false);
    menu->addAction(a);
    return;
  }
  auto ke = m_metas.keyValueEnd();
  auto k = m_metas.keyValueBegin();
  for (; k != ke; k++) {
    auto i = new QMenu;
    auto n = *k;
    i->setTitle(n.first);
    for (auto mitem : n.second) {
      QAction *a;
      PluginMenuAddItemLamba(i, mitem.name, [=] {
        PlgInterface::instance()->RunPyFile(mitem.filename);
      });
    }
    menu->addMenu(i);
  }
}

void ScriptManager::loadTreeWidget(QTreeWidget *tree) {
  if (!tree)
    return;
  tree->clear();
  auto ke = m_metas.keyValueEnd();
  auto k = m_metas.keyValueBegin();
  for (; k != ke; k++) {
    auto n = *k;
    auto p = new QTreeWidgetItem(tree, {k->first});
    p->setIcon(0, QIcon(":/img/pydb.png"));
    for (auto mitem : n.second) {
      auto i = new QTreeWidgetItem(p, {mitem.name});
      i->setIcon(0, QIcon(":/img/py.png"));
      i->setData(0, Qt::UserRole, QVariant::fromValue(mitem));
    }
  }
}

void ScriptManager::parseMeta(QFileInfo fileinfo, QString folder,
                              QList<ScriptMeta> &m) {
  QFile f(fileinfo.absoluteFilePath());
  QJsonParseError err;
  if (!f.open(QFile::ReadOnly))
    return;
  QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
  f.close();
  if (err.error != QJsonParseError::NoError)
    return;
  auto jobj = doc.object();
  auto n = fileinfo.fileName();
  ScriptMeta meta;
  auto p = n.lastIndexOf('.');
  meta.name = n.left(p);
  meta.filename = folder + "/" + meta.name + ".py";
  auto t = jobj.value("author");
  if (!t.isUndefined() && t.isString()) {
    meta.author = t.toString();
  }
  t = jobj.value("license");
  if (!t.isUndefined() && t.isString()) {
    meta.license = t.toString();
  } else {
    meta.license = "Unlicense";
  }
  t = jobj.value("commnet");
  if (!t.isUndefined() && t.isString()) {
    meta.commnet = t.toString();
  }
  t = jobj.value("version");
  meta.version = uint(t.toInt());
  m.push_back(meta);
}
