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

#include <QTextEdit>
#include <qstyleditemdelegate.h>
#include "CREMultilineItemDelegate.h"

CREMultilineItemDelegate::CREMultilineItemDelegate(QObject* parent, bool asStringList, bool trimEmpty)
: QStyledItemDelegate(parent), myAsStringList(asStringList), myTrimEmpty(trimEmpty)
{
}

CREMultilineItemDelegate::~CREMultilineItemDelegate()
{
}

QWidget* CREMultilineItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
    QTextEdit* edit = new QTextEdit(parent);
    edit->setAcceptRichText(false);
    return edit;
}

void CREMultilineItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!index.isValid())
        return;
    QTextEdit* edit = qobject_cast<QTextEdit*>(editor);
    if (edit == NULL)
        return;

    if (myAsStringList)
        edit->setPlainText(index.model()->data(index, Qt::EditRole).value<QStringList>().join("\n"));
    else
        edit->setPlainText(index.model()->data(index, Qt::EditRole).toString());
}

QStringList convert(const QString& source, bool trimEmpty)
{
    QStringList value;
    for (QString line : source.split("\n"))
    {
        if (!line.isEmpty() || !trimEmpty)
            value.append(line);
    }
    return value;
}

void CREMultilineItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (!index.isValid())
        return;
    QTextEdit* edit = qobject_cast<QTextEdit*>(editor);
    if (edit == NULL)
        return;

    if (myAsStringList)
        model->setData(index, convert(edit->toPlainText(), myTrimEmpty));
    else
        model->setData(index, edit->toPlainText());
}
