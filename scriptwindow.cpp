#include "scriptwindow.h"
#include "../WingHexExplorer/wing-hex-explorer.sourcecode/WingHexExplorer/plugin/iwingplugin.h"
#include "QCodeEditor/QPythonHighlighter.hpp"
#include "plginterface.h"
#include <DTitlebar>
#include <DWidgetUtil>
#include <Python.h>
#include <QVBoxLayout>

#define ICONRES(name) QIcon(":/img/" name ".png")

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
  auto picon = ICONRES("pys");
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
  editor->setHighlighter(new QPythonHighlighter);
  editor->setSyntaxStyle(
      m_styles[guihelper->themeType() == DGuiApplicationHelper::LightType ? 0
                                                                          : 1]);
  editor->setWordWrapMode(QTextOption::NoWrap);
  editor->setTabReplace(true);
  editor->setAutoIndentation(true);

  vlayout->addWidget(editor);

  PluginMenuInitBegin(menu, "") {
    Q_UNUSED(a);
    QMenu *m;
    PluginMenuInitBegin(m, tr("File")) {
      m->setIcon(ICONRES("file"));
      PluginMenuAddItemIconAction(m, tr("New"), ICONRES("new"),
                                  ScriptWindow::on_new);
      PluginMenuAddItemIconAction(m, tr("Open"), ICONRES("open"),
                                  ScriptWindow::on_open);
      PluginMenuAddItemIconAction(m, tr("Save"), ICONRES("save"),
                                  ScriptWindow::on_save);
      PluginMenuAddItemIconAction(m, tr("SaveAs"), ICONRES("saveas"),
                                  ScriptWindow::on_saveas);
      auto rm = new QMenu(menu);
      rm->setTitle(tr("RecentFile"));
      recentmanager = new RecentFileManager(rm, this);
      recentmanager->apply();
      m->addMenu(rm);
      PluginMenuAddItemIconAction(m, tr("Close"), ICONRES("close"),
                                  ScriptWindow::on_close);
    }
    PluginMenuInitEnd();
    menu->addMenu(m);

    PluginMenuInitBegin(m, tr("Edit")) {
      m->setIcon(ICONRES("edit"));
      PluginMenuAddItemIconAction(m, tr("Undo"), ICONRES("undo"),
                                  ScriptWindow::on_undo);
      a->setEnabled(false);
      mundo = a;
      PluginMenuAddItemIconAction(m, tr("Redo"), ICONRES("redo"),
                                  ScriptWindow::on_redo);
      a->setEnabled(false);
      mredo = a;
      m->addSeparator();
      PluginMenuAddItemIconAction(m, tr("Cut"), ICONRES("cut"),
                                  ScriptWindow::on_cut);
      PluginMenuAddItemIconAction(m, tr("Copy"), ICONRES("copy"),
                                  ScriptWindow::on_copy);
      PluginMenuAddItemIconAction(m, tr("Paste"), ICONRES("paste"),
                                  ScriptWindow::on_paste);
    }
    PluginMenuInitEnd();
    menu->addMenu(m);
  }
  PluginMenuInitEnd();

  _title->setMenu(menu);
  _title->setFullScreenButtonVisible(true);
  _title->setSwitchThemeMenuVisible(false);
  _title->setQuitMenuVisible(false);

  PluginToolBarInitBegin(toolbar, "") {
    PluginToolBarAddAction(toolbar, ICONRES("new"), ScriptWindow::on_new,
                           tr("New"));
    PluginToolBarAddAction(toolbar, ICONRES("open"), ScriptWindow::on_open,
                           tr("Open"));
    PluginToolBarAddAction(toolbar, ICONRES("save"), ScriptWindow::on_save,
                           tr("Save"));
    PluginToolBarAddAction(toolbar, ICONRES("saveas"), ScriptWindow::on_saveas,
                           tr("SaveAs"));
    toolbar->addSeparator();
    PluginToolBarAddAction(toolbar, ICONRES("undo"), ScriptWindow::on_undo,
                           tr("Undo"));
    a->setEnabled(false);
    aundo = a;
    PluginToolBarAddAction(toolbar, ICONRES("redo"), ScriptWindow::on_redo,
                           tr("Redo"));
    a->setEnabled(false);
    aredo = a;
    PluginToolBarAddAction(toolbar, ICONRES("cut"), ScriptWindow::on_cut,
                           tr("Cut"));
    PluginToolBarAddAction(toolbar, ICONRES("copy"), ScriptWindow::on_copy,
                           tr("Copy"));
    PluginToolBarAddAction(toolbar, ICONRES("paste"), ScriptWindow::on_saveas,
                           tr("Paste"));
    toolbar->addSeparator();
    PluginToolBarAddAction(toolbar, ICONRES("run"), ScriptWindow::on_run,
                           tr("Run"));
  }
  PluginToolBarInitEnd();

  addToolBar(toolbar);

  status = new DStatusBar(this);
  setStatusBar(status);

  connect(editor, &QCodeEditor::undoAvailable, this, [=](bool b) {
    aundo->setEnabled(b);
    mundo->setEnabled(b);
  });
  connect(editor, &QCodeEditor::redoAvailable, this, [=](bool b) {
    aredo->setEnabled(b);
    mredo->setEnabled(b);
  });

  Dtk::Widget::moveToCenter(this);
}

void ScriptWindow::setTheme(DGuiApplicationHelper::ColorType theme) {
  if (theme == DGuiApplicationHelper::LightType) {
    editor->setSyntaxStyle(m_styles[0]);
  } else {
    editor->setSyntaxStyle(m_styles[1]);
  }
}

void ScriptWindow::on_new() {}

void ScriptWindow::on_open() {}

void ScriptWindow::on_save() {}

void ScriptWindow::on_saveas() {}

void ScriptWindow::on_close() { close(); }

void ScriptWindow::on_redo() { editor->redo(); }

void ScriptWindow::on_undo() { editor->undo(); }

void ScriptWindow::on_copy() { editor->copy(); }

void ScriptWindow::on_cut() { editor->cut(); }

void ScriptWindow::on_paste() { editor->paste(); }

void ScriptWindow::on_run() {
  auto inst = PlgInterface::instance();
  inst->RunPyText(editor->toPlainText());
}
