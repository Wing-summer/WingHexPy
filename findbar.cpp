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

#include "findbar.h"

#include <QDebug>

FindBar::FindBar(QWidget *parent) : DFloatingWidget(parent) {
  // Init.
  // setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
  hide();
  setFixedHeight(60);

  // Init layout and widgets.

  m_layout = new QHBoxLayout();
  m_layout->setSpacing(10);
  m_findLabel = new QLabel(tr("Find"));
  m_findLabel->setMinimumHeight(36);
  m_editLine = new LineBar();
  m_editLine->lineEdit()->setMinimumHeight(36);
  m_findPrevButton = new QPushButton(tr("Previous"));
  // m_findPrevButton->setFixedSize(80, 36);
  m_findNextButton = new QPushButton(tr("Next"));
  // m_findNextButton->setFixedSize(80, 36);
  m_closeButton = new DIconButton(DStyle::SP_CloseButton);
  m_closeButton->setIconSize(QSize(30, 30));
  m_closeButton->setFixedSize(30, 30);
  m_closeButton->setEnabledCircle(true);
  m_closeButton->setFlat(true);
  m_layout->setContentsMargins(16, 6, 10, 6);

  m_layout->addWidget(m_findLabel);
  m_layout->addWidget(m_editLine);
  m_layout->addWidget(m_findPrevButton);
  m_layout->addWidget(m_findNextButton);
  m_layout->addWidget(m_closeButton);
  this->setLayout(m_layout);

  // Make button don't grab keyboard focus after click it.
  // m_findNextButton->setFocusPolicy(Qt::NoFocus);
  // m_findPrevButton->setFocusPolicy(Qt::NoFocus);
  // m_closeButton->setFocusPolicy(Qt::NoFocus);

  connect(this, &FindBar::pressEsc, this, &FindBar::findCancel,
          Qt::QueuedConnection);
  // connect(m_editLine, &LineBar::pressEnter, this, &FindBar::findNext,
  // Qt::QueuedConnection);            //Shielded by Hengbo ,for new demand.
  // 20200220
  // connect(m_editLine, &LineBar::pressCtrlEnter, this, &FindBar::findPrev,
  // Qt::QueuedConnection);
  connect(m_editLine, &LineBar::returnPressed, this,
          &FindBar::handleContentChanged, Qt::QueuedConnection);
  connect(m_editLine, &LineBar::signal_sentText, this, &FindBar::receiveText,
          Qt::QueuedConnection);
  // connect(m_editLine, &LineBar::contentChanged, this,
  // &FindBar::slot_ifClearSearchWord, Qt::QueuedConnection);

  connect(m_findNextButton, &QPushButton::clicked, this,
          &FindBar::handleFindNext, Qt::QueuedConnection);
  connect(m_findPrevButton, &QPushButton::clicked, this,
          &FindBar::handleFindPrev, Qt::QueuedConnection);
  // connect(m_findPrevButton, &QPushButton::clicked, this, &FindBar::findPrev,
  // Qt::QueuedConnection);

  connect(m_closeButton, &DIconButton::clicked, this, &FindBar::findCancel,
          Qt::QueuedConnection);
}

bool FindBar::isFocus() { return m_editLine->lineEdit()->hasFocus(); }

void FindBar::focus() {
  m_editLine->lineEdit()->setFocus();
  m_editLine->lineEdit()->selectAll();
}

void FindBar::activeInput(QString text, int row, int column, int scrollOffset) {
  // Try fill keyword with select text.
  m_editLine->lineEdit()->clear();
  m_editLine->lineEdit()->insert(text);
  m_editLine->lineEdit()->selectAll();

  // Show.
  QWidget::show();

  // Save file info for back to position.
  m_findFileRow = row;
  m_findFileColumn = column;
  m_findFileSrollOffset = scrollOffset;

  // Focus.
  focus();
}

void FindBar::findCancel() {
  QWidget::hide();
  emit sigFindbarClose();
}

void FindBar::handleContentChanged() {
  updateSearchKeyword(m_editLine->lineEdit()->text().trimmed());
}

void FindBar::handleFindPrev() {
  findPrev(m_editLine->lineEdit()->text().trimmed());
}

void FindBar::handleFindNext() {
  findNext(m_editLine->lineEdit()->text().trimmed());
}

void FindBar::hideEvent(QHideEvent *) {
  //保留查询标记
  // removeSearchKeyword();
}

bool FindBar::focusNextPrevChild(bool next) {
  Q_UNUSED(next);
  return false;
}

// modiefied by wingsummer
void FindBar::keyPressEvent(QKeyEvent *e) {
  bool unhandled = false;
  if (e->modifiers() == Qt::KeyboardModifier::NoModifier) {
    switch (e->key()) {
    case Qt::Key_Escape: {
      QWidget::hide();
      emit sigFindbarClose();
    } break;
    case Qt::Key_Tab: {
      if (m_closeButton->hasFocus())
        m_editLine->lineEdit()->setFocus();
    } break;
    case Qt::Key_Enter: {
      if (m_findPrevButton->hasFocus()) {
        m_editLine->lineEdit()->setFocus();
      }
      if (m_findNextButton->hasFocus()) {
        m_findNextButton->click();
      }
    } break;
    default:
      unhandled = true;
      break;
    }
  }
  if (unhandled)
    DFloatingWidget::keyPressEvent(e);
}

void FindBar::setMismatchAlert(bool isAlert) { m_editLine->setAlert(isAlert); }

void FindBar::receiveText(QString t) {
  searched = false;
  if (t != "") {
    m_receivedText = t;
  }
}

void FindBar::setSearched(bool _) { searched = _; }

void FindBar::findPreClicked() {
  if (!searched) {
    updateSearchKeyword(m_editLine->lineEdit()->text());
    emit findPrev(m_editLine->lineEdit()->text());
    searched = true;
  } else {
    emit findPrev(m_editLine->lineEdit()->text());
  }
}
