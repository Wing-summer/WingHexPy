#include "winghexpy.h"
#include "scriptwindow.h"
#include <QFileDialog>

#define ICONRES(name) QIcon(":/img/" name ".png")

WingHexPy::WingHexPy(QObject *parent) { Q_UNUSED(parent) }

WingHexPy::~WingHexPy() {}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(WingHexPy, GenericPlugin)
#endif // QT_VERSION < 0x050000

int WingHexPy::sdkVersion() { return SDKVERSION; }

QString WingHexPy::signature() { return WINGSUMMER; }

bool WingHexPy::init(QList<WingPluginInfo> loadedplugin) {
  Q_UNUSED(loadedplugin);

  plgint = new PlgInterface(this);
  if (!plgint->init()) {
    return false;
  }
  connect(plgint, &PlgInterface::log, this, &WingHexPy::log);

  PluginMenuInitBegin(menu, tr("WingHexPy")) {
    PluginMenuAddItemLamba(menu, tr("Run File"), [=] {
      auto py = QFileDialog::getOpenFileName(nullptr, tr("ChooseFile"),
                                             QString(), "Py (*.py)");
      if (py.isEmpty())
        return;
      this->RunPyFile(py);
    });
  }
  PluginMenuInitEnd();

  txt = plgint->getScriptingConsole();
  PluginDockWidgetInit(dw, txt, tr("WingHexPyConsole"), "WingHexPyConsole");

  infolist = new QListWidget;
  PluginDockWidgetInit(dlist, infolist, tr("InfoList"), "WingHexPyInfoList");
  infotree = new QTreeWidget;
  PluginDockWidgetInit(dtree, infotree, tr("InfoTree"), "WingHexPyInfoTree");
  infotable = new QTableWidget;
  PluginDockWidgetInit(dtable, infotable, tr("InfoTable"),
                       "WingHexPyInfoTable");

  plgint->initInfo(infolist, infotree, infotable);

  PluginToolBarInitBegin(tb, "WingHexPy") {
    PluginToolBarAddLamba(
        tb, ICONRES("pys"), [=] { ScriptWindow::instance()->show(); },
        tr("ScriptWindow"));
  }
  PluginToolBarInitEnd();

  return true;
}

void WingHexPy::RunPyFile(QString filename) { plgint->RunPyFile(filename); }

void WingHexPy::RunPyText(QString content) { plgint->RunPyText(content); }

void WingHexPy::unload() {}

void WingHexPy::plugin2MessagePipe(WingPluginMessage type,
                                   QList<QVariant> msg) {
  Q_UNUSED(type);
  Q_UNUSED(msg);
}

QString WingHexPy::pluginName() { return tr("WingHexPy"); }

QString WingHexPy::pluginAuthor() { return WINGSUMMER; }

QString WingHexPy::pluginComment() {
  return tr("A useful plugin for python3.7 support.");
}

uint WingHexPy::pluginVersion() { return 1; }

QMenu *WingHexPy::registerMenu() { return menu; }

void WingHexPy::registerDockWidget(
    QMap<QDockWidget *, Qt::DockWidgetArea> &rdw) {
  rdw.insert(dw, Qt::DockWidgetArea::BottomDockWidgetArea);
  rdw.insert(dtree, Qt::DockWidgetArea::BottomDockWidgetArea);
  rdw.insert(dtable, Qt::DockWidgetArea::BottomDockWidgetArea);
  rdw.insert(dlist, Qt::DockWidgetArea::BottomDockWidgetArea);
}

QToolBar *WingHexPy::registerToolBar() { return tb; }

void WingHexPy::log(QString message) { txt->consoleMessage(message); }
