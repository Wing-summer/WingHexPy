#include "winghexpy.h"
#include "scriptcenterwindow.h"
#include "scriptwindow.h"
#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>

#define ICONRES(name) QIcon(":/WingHexPy/img/" name ".png")
#define HOSTICONRES(name) QIcon(HOSTRESPIMG(name))

WingHexPy::WingHexPy(QObject *parent) { Q_UNUSED(parent) }

WingHexPy::~WingHexPy() {}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(WingHexPy, GenericPlugin)
#endif // QT_VERSION < 0x050000

int WingHexPy::sdkVersion() { return SDKVERSION; }

QString WingHexPy::signature() { return WINGSUMMER; }

bool WingHexPy::init(QList<WingPluginInfo> loadedplugin) {
  Q_UNUSED(loadedplugin);

  if (SDKVERSION < 8) {
    QMessageBox::critical(nullptr, "Error",
                          "UnSupported Plugin System Version!",
                          QMessageBox::Ok);
    return false;
  }

  auto translator = new QTranslator(this);
  auto s = GETPLUGINQM("WingHexPy.qm");
  if (!translator->load(s) || !QApplication::installTranslator(translator)) {
    QMessageBox::critical(nullptr, "Error", "Error Loading Translation File!",
                          QMessageBox::Ok);
    return false;
  }

  plgint = new PlgInterface(this);
  if (!plgint->init()) {
    return false;
  }
  connect(plgint, &PlgInterface::log, this, &WingHexPy::log);

  smanager = ScriptManager::instance();

  PluginMenuInitBegin(menu, tr("WingHexPy")) {
    menu->setIcon(ICONRES("icon"));
    PluginMenuAddItemIconLamba(menu, tr("ScriptEditor"), ICONRES("pys"),
                               [=] { ScriptWindow::instance()->show(); });
    PluginMenuAddItemIconAction(menu, tr("Run File"), ICONRES("runf"),
                                WingHexPy::runPyFile);
    PluginMenuAddItemIconLamba(menu, tr("Database"), ICONRES("pydb"), [=] {
      ScriptCenterWindow d;
      d.exec();
    });
    auto m = new QMenu;
    m->setTitle(tr("Script"));
    m->setIcon(ICONRES("py"));
    smanager->loadMenu(m);
    menu->addMenu(m);
  }
  PluginMenuInitEnd();

  txt = plgint->getScriptingConsole();
  PluginDockWidgetInit(dw, txt, tr("Console"), "WingHexPyConsole");

  infolist = new QListWidget;
  PluginDockWidgetInit(dlist, infolist, tr("InfoList"), "WingHexPyInfoList");
  dlist->setMinimumSize(600, 200);
  infotree = new QTreeWidget;
  infotree->setHeaderLabel("");
  PluginDockWidgetInit(dtree, infotree, tr("InfoTree"), "WingHexPyInfoTree");
  dtree->setMinimumSize(600, 200);
  infotable = new QTableWidget;
  PluginDockWidgetInit(dtable, infotable, tr("InfoTable"),
                       "WingHexPyInfoTable");
  dtable->setMinimumSize(600, 200);
  infotxt = new QTextBrowser;
  PluginDockWidgetInit(dtxt, infotxt, tr("InfoTxt"), "WingHexPyInfoTxt");
  dtxt->setMinimumSize(600, 200);

  plgint->initInfo(infolist, infotree, infotable, infotxt);

  auto sw = ScriptWindow::instance();
  connect(sw, &ScriptWindow::sigAbout, this, &WingHexPy::on_about);
  connect(sw, &ScriptWindow::sigSponsor, this, &WingHexPy::on_sponsor);

  PluginToolBarInitBegin(tb, "WingHexPy", "WingHexPy") {
    PluginToolBarAddLamba(
        tb, ICONRES("pys"), [=] { ScriptWindow::instance()->show(); },
        tr("ScriptWindow"));
    PluginToolBarAddLamba(
        tb, ICONRES("pydb"),
        [=] {
          ScriptCenterWindow d;
          d.exec();
        },
        tr("Database"));

    auto m = new QMenu;
    smanager->loadMenu(m);
    auto tbtn = new QToolButton;
    tbtn->setIcon(ICONRES("py"));
    tbtn->setMenu(m);
    tbtn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    tb->addWidget(tbtn);

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
    PluginToolBarAddAction(tb, HOSTICONRES("author"), WingHexPy::on_about,
                           tr("About"));
    PluginToolBarAddLamba(
        tb, HOSTICONRES("wiki"),
        [=] {
          QDesktopServices::openUrl(
              QUrl("https://code.gitlink.org.cn/wingsummer/WingHexPy/wiki/"
                   "%E6%95%99%E7%A8%8B"));
        },
        tr("Wiki"));
    PluginToolBarAddAction(tb, HOSTICONRES("sponsor"), WingHexPy::on_sponsor,
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

Qt::ToolBarArea WingHexPy::registerToolBarArea() {
  return Qt::ToolBarArea::LeftToolBarArea;
}

void WingHexPy::log(QString message) { txt->consoleMessage(message); }

void WingHexPy::on_about() {
  auto authord = newAboutDialog(QPixmap(), {":/WingHexPy", ":/WingHexPy/img"});
  authord->exec();
  delete authord;
}

void WingHexPy::on_sponsor() {
  auto sponsor = newSponsorDialog();
  sponsor->exec();
  delete sponsor;
}
