#ifndef SCRIPTCENTERWINDOW_H
#define SCRIPTCENTERWINDOW_H

#include "scriptmanager.h"
#include <DDialog>
#include <DMainWindow>
#include <QObject>

DWIDGET_USE_NAMESPACE

class ScriptCenterWindow : public DDialog {
  Q_OBJECT
public:
  ScriptCenterWindow(DMainWindow *parent = nullptr);

private:
  ScriptManager *m;
};

#endif // SCRIPTCENTERWINDOW_H
