#include "winghexpy.h"
#include "aboutsoftwaredialog.h"
#include "scriptwindow.h"
#include "sponsordialog.h"
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
    PluginMenuAddItemIconLamba(menu, tr("ScriptEditor"), ICONRES("pys"),
                               [=] { ScriptWindow::instance()->show(); });
    PluginMenuAddItemIconAction(menu, tr("Run File"), ICONRES("runf"),
                                WingHexPy::runPyFile);
  }
  PluginMenuInitEnd();

  txt = plgint->getScriptingConsole();
  PluginDockWidgetInit(dw, txt, tr("WingHexPyConsole"), "WingHexPyConsole");

  infolist = new QListWidget;
  PluginDockWidgetInit(dlist, infolist, tr("InfoList"), "WingHexPyInfoList");
  dlist->setMinimumSize(500, 200);
  infotree = new QTreeWidget;
  PluginDockWidgetInit(dtree, infotree, tr("InfoTree"), "WingHexPyInfoTree");
  dtree->setMinimumSize(500, 200);
  infotable = new QTableWidget;
  PluginDockWidgetInit(dtable, infotable, tr("InfoTable"),
                       "WingHexPyInfoTable");
  dtable->setMinimumSize(500, 200);
  infotxt = new QTextBrowser;
  PluginDockWidgetInit(dtxt, infotxt, tr("InfoTxt"), "WingHexPyInfoTxt");
  dtxt->setMinimumSize(500, 200);

  plgint->initInfo(infolist, infotree, infotable, infotxt);

  PluginToolBarInitBegin(tb, "WingHexPy") {
    PluginToolBarAddLamba(
        tb, ICONRES("pys"), [=] { ScriptWindow::instance()->show(); },
        tr("ScriptWindow"));
    tb->addSeparator();
    PluginToolBarAddAction(tb, ICONRES("runf"), WingHexPy::runPyFile,
                           tr("Run File"));
    PluginToolBarAddLamba(
        tb, ICONRES("infolist"),
        [=] {
          dlist->show();
          dlist->raise();
        },
        tr("InfoList"));
    PluginToolBarAddLamba(
        tb, ICONRES("infotable"),
        [=] {
          dtable->show();
          dtable->raise();
        },
        tr("InfoTable"));
    PluginToolBarAddLamba(
        tb, ICONRES("infotree"),
        [=] {
          dtree->show();
          dtree->raise();
        },
        tr("InfoTree"));
    PluginToolBarAddLamba(
        tb, ICONRES("infotxt"),
        [=] {
          dtxt->show();
          dtxt->raise();
        },
        tr("InfoTxt"));
    tb->addSeparator();
    PluginToolBarAddLamba(
        tb, ICONRES("author"),
        [=] {
          AboutSoftwareDialog d;
          d.exec();
        },
        tr("About"));
    PluginToolBarAddLamba(
        tb, ICONRES("sponsor"),
        [=] {
          SponsorDialog d;
          d.exec();
        },
        tr("Sponsor"));
  }
  PluginToolBarInitEnd();

  return true;
}

void WingHexPy::runPyFile() {
  auto py = QFileDialog::getOpenFileName(nullptr, tr("ChooseFile"), QString(),
                                         "Py (*.py)");
  if (py.isEmpty())
    return;
  this->RunPyFile(py);
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
  rdw.insert(dtxt, Qt::DockWidgetArea::BottomDockWidgetArea);
}

QToolBar *WingHexPy::registerToolBar() { return tb; }

void WingHexPy::log(QString message) { txt->consoleMessage(message); }
