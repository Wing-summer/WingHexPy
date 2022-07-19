/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Rekols    <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REPLACEBAR_H
#define REPLACEBAR_H

#include "dimagebutton.h"
#include "linebar.h"
#include <DAbstractDialog>
#include <DApplicationHelper>
#include <DFloatingWidget>
#include <DIconButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QWidget>

DWIDGET_USE_NAMESPACE

class ReplaceBar : public DFloatingWidget {
  Q_OBJECT

public:
  explicit ReplaceBar(QWidget *parent = nullptr);

  bool isFocus();
  void focus();

  void activeInput(QString text, int row, int column, int scrollOffset);
  void setMismatchAlert(bool isAlert);
  void setsearched(bool _);

signals:
  void pressEsc();
  void replaceNext(QString replaceText, QString withText);
  void replaceSkip(QString keyword);
  void replaceAll(QString replaceText, QString withText);
  void backToPosition(int row, int column, int scrollOffset);

  void removeSearchKeyword();
  void updateSearchKeyword(QString keyword);

  void sigReplacebarClose();

public slots:
  void change();
  void replaceClose();
  void handleContentChanged();
  void handleReplaceAll();
  void handleReplaceNext();

protected:
  void hideEvent(QHideEvent *event);
  bool focusNextPrevChild(bool next);
  void keyPressEvent(QKeyEvent *e);

private:
  QPushButton *m_replaceAllButton;
  QPushButton *m_replaceButton;
  QPushButton *m_replaceSkipButton;
  DIconButton *m_closeButton;
  LineBar *m_replaceLine;
  LineBar *m_withLine;
  QHBoxLayout *m_layout;
  QLabel *m_replaceLabel;
  QLabel *m_withLabel;
  int m_replaceFileColumn;
  int m_replaceFileRow;
  int m_replaceFileSrollOffset;
  QColor m_backgroundColor;
  bool searched = false;

  QPoint last;
};

#endif
