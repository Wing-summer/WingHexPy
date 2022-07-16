#include "plginterface.h"
#include "PythonQt/PythonQtCppWrapperFactory.h"
#include <QKeySequence>
#include <QShortcut>
#include <QTemporaryFile>

PlgInterface *PlgInterface::m_instace = nullptr;

bool PlgInterface::init() {
  PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
  auto py = PythonQt::self();
  py->registerCPPClass("plgreader", nullptr, "wingplg",
                       PythonQtCreateObject<WingPlugin::Reader>);
  py->registerCPPClass("plgcontroller", nullptr, "wingplg",
                       PythonQtCreateObject<WingPlugin::Controller>);
  py->registerCPPClass("plgservice", nullptr, "wingplg",
                       PythonQtCreateObject<PlgService>);
  qRegisterMetaType<HexPosition>("hexposition");
  qRegisterMetaType<BookMark>("bookmark");
  qRegisterMetaType<ErrFile>("errfile");
  qRegisterMetaType<FindResult>("findresult");
  qRegisterMetaType<HexMetadataAbsoluteItem>("absmetadata");
  qRegisterMetaType<HexMetadataItem>("metadata");
  qRegisterMetaType<HexLineMetadata>("linematadata");

  mainContext = PythonQt::self()->getMainModule();
  mainContext.addObject("reader", &plg->reader);
  mainContext.addObject("controller", &plg->controller);
  mainContext.addObject("service", new PlgService(this));
  mainContext.evalScript(
      "from PythonQt import *\nfrom PythonQt import wingplg");
  console = new PythonQtScriptingConsole(nullptr, mainContext);
  m_instace = this;
  auto shortcut = new QShortcut(
      QKeySequence(Qt::KeyboardModifier::ControlModifier | Qt::Key_L), console);
  connect(shortcut, &QShortcut::activated, this, [=] { console->clear(); });
  return true;
}

PythonQtScriptingConsole *PlgInterface::getScriptingConsole() {
  return console;
}

void PlgInterface::RunPyFile(QString filename) {
  mainContext.evalFile(filename);
  if (plg->hasControl())
    plg->requestRelease();
}

bool PlgInterface::requestControl(int timeout) {
  return plg->requestControl(timeout);
}

bool PlgInterface::requestRelease() { return plg->requestRelease(); }

void PlgInterface::RunPyText(QString content) {
  mainContext.evalScript(content);
  if (plg->hasControl())
    plg->requestRelease();
}

PlgInterface::PlgInterface(IWingPlugin *parent) : QObject(parent) {
  plg = parent;
}

PlgInterface *PlgInterface::instance() { return m_instace; }
