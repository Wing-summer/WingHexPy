#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include "QCodeEditor/QCodeEditor.hpp"
#include "QCodeEditor/QSyntaxStyle.hpp"
#include "findbar.h"
#include "recentfilemanager.h"
#include "replacebar.h"
#include <DGuiApplicationHelper>
#include <DMainWindow>
#include <DStatusBar>
#include <DToolBar>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

class ScriptWindow : public DMainWindow {
  Q_OBJECT
public:
  ScriptWindow(DMainWindow *parent = nullptr);
  static ScriptWindow *instance();

private:
  void setTheme(DGuiApplicationHelper::ColorType theme);

public:
  bool openFile(QString filename, bool readonly = false);

private:
  void on_new();
  void on_open();
  void on_save();
  void on_saveas();
  void on_undo();
  void on_redo();
  void on_copy();
  void on_cut();
  void on_paste();
  void on_run();
  void on_runfile();
  void on_close();
  void on_sponsor();
  void on_about();
  void on_find();
  void on_replace();

private:
  QCodeEditor *editor;
  static ScriptWindow *m_instance;
  QSyntaxStyle *m_styles[2];
  DToolBar *toolbar;
  DStatusBar *status;
  QAction *aundo, *aredo;
  QAction *mundo, *mredo;
  DMenu *menu;
  RecentFileManager *recentmanager;
  QVBoxLayout *vlayout;
  FindBar *findbar;
  ReplaceBar *replacebar;

private:
  bool isSaved = true;
};

#endif // SCRIPTWINDOW_H
