#include "winghexpy.h"
#include "scriptwindow.h"
#include <QFileDialog>

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

  txt = new QTextBrowser;
  PluginDockWidgetInit(dw, txt, tr("WingHexPyConsole"), "WingHexPyConsole");

  PluginToolBarInitBegin(tb, "WingHexPy") {
    PluginToolBarAddLamba(
        tb, QIcon(":/img/pys.png"), [=] { ScriptWindow::instance()->show(); },
        tr("ScriptWindow"));
  }
  PluginToolBarInitEnd();

  return true;
}

bool WingHexPy::RunPyFile(QString filename) {
  return plgint->RunPyFile(filename);
}

bool WingHexPy::RunPyText(QString content) {
  return plgint->RunPyText(content);
}

void WingHexPy::unload() { Py_Finalize(); }

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

Qt::DockWidgetArea WingHexPy::registerDockWidgetDockArea() {
  return Qt::DockWidgetArea::BottomDockWidgetArea;
}

QMenu *WingHexPy::registerMenu() { return menu; }

QDockWidget *WingHexPy::registerDockWidget() { return dw; }

QToolBar *WingHexPy::registerToolBar() { return tb; }

void WingHexPy::log(QString message) { txt->insertHtml(message); }
