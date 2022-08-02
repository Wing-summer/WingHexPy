#include "plginterface.h"
#include "PythonQt/PythonQtCppWrapperFactory.h"
#include "PythonQt/PythonQt_QtAll.h"
#include "iwingpluginstructwrapper.h"
#include <QKeySequence>
#include <QListWidgetItem>
#include <QShortcut>
#include <QTemporaryFile>

PlgInterface *PlgInterface::m_instace = nullptr;

bool PlgInterface::init() {
  PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
  PythonQt_QtAll::init();
  auto py = PythonQt::self();
  py->registerClass(&QListWidget::staticMetaObject, "QtGui");
  py->registerClass(&QTableWidget::staticMetaObject, "QtGui");
  py->registerClass(&QTreeWidget::staticMetaObject, "QtGui");
  py->registerClass(&QTextBrowser::staticMetaObject, "QtGui");
  py->registerCPPClass("plgreader", nullptr, "wingplg",
                       PythonQtCreateObject<WingPlugin::Reader>);
  py->registerCPPClass("plgcontroller", nullptr, "wingplg",
                       PythonQtCreateObject<WingPlugin::Controller>);
  py->registerCPPClass("plgservice", nullptr, "wingplg",
                       PythonQtCreateObject<PlgService>);
  py->addDecorators(new IWingPluginStructWrapper());

  qRegisterMetaType<HexPosition>("HexPosition");
  qRegisterMetaType<BookMark>("BookMark");
  qRegisterMetaType<ErrFile>("ErrFile");
  qRegisterMetaType<FindResult>("FindResult");
  qRegisterMetaType<HexMetadataAbsoluteItem>("HexMetadataAbsoluteItem");
  qRegisterMetaType<HexMetadataItem>("HexMetadataItem");
  qRegisterMetaType<HexLineMetadata>("HexLineMetadata");

  mainContext = PythonQt::self()->getMainModule();
  mainContext.addObject("reader", &plg->reader);
  mainContext.addObject("controller", &plg->controller);
  mainContext.addObject("service", new PlgService(this));

  mainContext.evalScript("from PythonQt import *");
  console = new PythonQtScriptingConsole(nullptr, mainContext);
  m_instace = this;
  auto shortcut = new QShortcut(
      QKeySequence(Qt::KeyboardModifier::ControlModifier | Qt::Key_L), console);
  connect(shortcut, &QShortcut::activated, this, [=] { console->clear(); });
  return true;
}

void PlgInterface::initInfo(QListWidget *infolist, QTreeWidget *infotree,
                            QTableWidget *infotable, QTextBrowser *infotxt) {
  mainContext.addObject("infotree", infotree);
  mainContext.addObject("infotable", infotable);
  mainContext.addObject("infolist", infolist);
  mainContext.addObject("infotxt", infotxt);
}

PythonQtScriptingConsole *PlgInterface::getScriptingConsole() {
  return console;
}

bool PlgInterface::RunPyFile(QString filename) {
  if (mutex.tryLock()) {
    auto cur = console->textCursor();
    cur.movePosition(QTextCursor::End);
    console->setTextCursor(cur);
    console->insertPlainText(filename);
    mainContext.evalFile(filename);
    console->appendCommandPrompt();
    if (plg->hasControl())
      plg->requestRelease();
    mutex.unlock();
    return true;
  }
  return false;
}

bool PlgInterface::requestControl(int timeout) {
  return plg->requestControl(timeout);
}

bool PlgInterface::requestRelease() { return plg->requestRelease(); }

bool PlgInterface::RunPyText(QString content) {
  if (mutex.tryLock()) {
    auto cur = console->textCursor();
    cur.movePosition(QTextCursor::End);
    console->setTextCursor(cur);
    console->insertPlainText(tr("[ExcuteFromScriptWindow]"));
    mainContext.evalScript(content);
    console->appendCommandPrompt();
    if (plg->hasControl())
      plg->requestRelease();
    mutex.unlock();
    return true;
  }
  return false;
}

PlgInterface::PlgInterface(IWingPlugin *parent) : QObject(parent) {
  plg = parent;
}

PlgInterface *PlgInterface::instance() { return m_instace; }
