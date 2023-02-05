/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "AssetTextEdit.h"
#include <QtWidgets>
#include "AssetFieldCompleter.h"

AssetTextEdit::AssetTextEdit(QWidget *parent) : QTextEdit(parent) {
  setAcceptRichText(false);
  myCompleter = new AssetFieldCompleter(this);
  myCompleter->setCompletionMode(QCompleter::PopupCompletion);
  myCompleter->setCaseSensitivity(Qt::CaseInsensitive);
  myCompleter->setWidget(this);
  connect(myCompleter, SIGNAL(activated(const QString &)), this, SLOT(insertCompletion(const QString &)));
}

void AssetTextEdit::keyPressEvent(QKeyEvent *e) {
  if (myCompleter && myCompleter->popup()->isVisible()) {
    switch (e->key()) {
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Escape:
      case Qt::Key_Tab:
      case Qt::Key_Backtab:
        e->ignore();
        return;
      default:
        break;
    }
  }
  QTextEdit::keyPressEvent(e);

  const QString completionPrefix = textUnderCursor();
  if (completionPrefix != myCompleter->completionPrefix()) {
    myCompleter->setCompletionPrefix(completionPrefix);
    if (!completionPrefix.isEmpty()) {
        myCompleter->popup()->setCurrentIndex(myCompleter->completionModel()->index(0, 0));
        QRect cr = cursorRect();
        cr.setWidth(myCompleter->popup()->sizeHintForColumn(0) + myCompleter->popup()->verticalScrollBar()->sizeHint().width());
        myCompleter->complete(cr);
    }
  }
}

void AssetTextEdit::insertCompletion(const QString &completion) {
  QTextCursor tc = textCursor();
  int extra = completion.length() - myCompleter->completionPrefix().length();
  tc.movePosition(QTextCursor::Left);
  tc.movePosition(QTextCursor::EndOfWord);
  tc.insertText(completion.right(extra));
  setTextCursor(tc);
}

QString AssetTextEdit::textUnderCursor() const {
  QString item;
  int end = textCursor().position(), start = end - 1;
  while (start >= 0) {
    QChar c = document()->characterAt(start);
    if (c == '.' || c.isLetter()) {
      --start;
      item.prepend(c);
      continue;
    }
    break;
  }
  while (end < document()->characterCount()) {
    QChar c = document()->characterAt(end);
    if (c.isLetter()) {
      ++end;
      item.append(c);
      continue;
    }
    break;
  }
  return item;
}
