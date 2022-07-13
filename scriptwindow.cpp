#include "scriptwindow.h"
#include "../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include "QCodeEditor/QPythonCompleter.hpp"
#include "QCodeEditor/QPythonHighlighter.hpp"
#include <DTitlebar>
#include <DWidgetUtil>
#include <QVBoxLayout>

ScriptWindow *ScriptWindow::m_instance = nullptr;

ScriptWindow *ScriptWindow::instance() {
  if (m_instance == nullptr) {
    m_instance = new ScriptWindow;
  }
  return m_instance;
}

ScriptWindow::ScriptWindow(DMainWindow *parent) : DMainWindow(parent) {
  setMinimumSize(QSize(600, 500));

  auto _title = titlebar();
  auto picon = QIcon(":/img/pys.png");
  setWindowIcon(picon);
  _title->setIcon(picon);
  _title->setTitle(tr("WingHexPyScriptWindow"));

  auto w = new QWidget(this);
  setCentralWidget(w);
  auto vlayout = new QVBoxLayout(w);

  m_styles[0] = QSyntaxStyle::defaultStyle();
  auto darkstyle = new QSyntaxStyle(this);
  QFile file(":/drakula.xml");
  if (file.open(QFile::ReadOnly)) {
    darkstyle->load(file.readAll());
    file.close();
  }
  auto guihelper = DGuiApplicationHelper::instance();
  connect(guihelper, &DGuiApplicationHelper::themeTypeChanged, this,
          &ScriptWindow::setTheme);

  m_styles[1] = darkstyle;
  editor = new QCodeEditor(this);
  editor->setCompleter(new QPythonCompleter(this));
  editor->setHighlighter(new QPythonHighlighter);
  editor->setSyntaxStyle(
      m_styles[guihelper->themeType() == DGuiApplicationHelper::LightType ? 0
                                                                          : 1]);
  editor->setWordWrapMode(QTextOption::NoWrap);
  editor->setTabReplace(true);
  editor->setAutoIndentation(true);

  vlayout->addWidget(editor);

  PluginMenuInitBegin(menu, "") {
    PluginMenuAddItemLamba(menu, tr("New"), [=] {

    });
  }
  PluginMenuInitEnd();

  _title->setMenu(menu);
  _title->setFullScreenButtonVisible(true);
  _title->setSwitchThemeMenuVisible(false);

  PluginToolBarInitBegin(toolbar, "") {
    PluginToolBarAddLamba(
        toolbar, QIcon(), [=] {}, tr(""));
  }
  PluginToolBarInitEnd();

  addToolBar(toolbar);

  status = new DStatusBar(this);
  setStatusBar(status);

  Dtk::Widget::moveToCenter(this);
}

void ScriptWindow::setTheme(DGuiApplicationHelper::ColorType theme) {
  if (theme == DGuiApplicationHelper::LightType) {
    editor->setSyntaxStyle(m_styles[0]);
  } else {
    editor->setSyntaxStyle(m_styles[1]);
  }
}
