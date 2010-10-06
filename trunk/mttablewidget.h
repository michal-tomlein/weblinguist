/*******************************************************************
 This file is part of Weblinguist
 Copyright (C) 2008 Michal Tomlein (michal.tomlein@gmail.com)

 Weblinguist is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public Licence
 as published by the Free Software Foundation; either version 2
 of the Licence, or (at your option) any later version.

 Weblinguist is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public Licence for more details.

 You should have received a copy of the GNU General Public Licence
 along with Weblinguist; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
********************************************************************/

#ifndef MTTABLEWIDGET_H
#define MTTABLEWIDGET_H

#include <QHeaderView>
#include <QTableWidget>
#include <QLineEdit>

#include "mtwidgetpalettes.h"

class MTTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    MTTableWidget(QWidget * parent = 0):
    QTableWidget(parent) {
        highlighted_row = -1;
        QObject::connect(this, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(emitCurrentIndexAvailabilityChanged()));
    };

public slots:
    int filterItems(QLineEdit * le, const QString & keyword) {
        if (keyword.isEmpty()) {
            le->setPalette(((QWidget *)(parent()))->palette());
        } else {
            le->setPalette(searchLineEditPalettes.search_active_palette);
        }
        int n = 0;
        for (int i = 0; i < rowCount(); ++i) {
            if (rowContains(i, keyword)) {
                showRow(i); n++;
            } else { hideRow(i); }
        }
        if ((!keyword.isEmpty()) && rowCount() != 0 && n == 0) {
            le->setPalette(searchLineEditPalettes.search_noresults_palette);
        }
        return n;
    };
    void highlightItem(QTableWidgetItem * item) { highlightRow(row(item)); };
    void highlightRow(int i) {
        if (highlighted_row >= 0) {
            for (int c = 0; c < columnCount(); ++c) {
                item(highlighted_row, c)->setBackground(Qt::NoBrush);
            }
        }
        highlighted_row = i;
        if (highlighted_row < 0) { return; }
        for (int c = 0; c < columnCount(); ++c) {
            item(i, c)->setBackground(QBrush::QBrush(QColor::QColor(197, 255, 120)));
        }
        clearSelection();
        if (columnCount() > 0) { scrollToItem(item(i, 0)); }
    };
    bool isRowHighlighted(int i) { return isItemHighlighted(item(i, 0)); };
    static bool isItemHighlighted(QTableWidgetItem * item) {
        if (item == NULL) { return false; }
        return (item->background() == QBrush::QBrush(QColor::QColor(197, 255, 120)));
    };
    int highlightedRow() { return highlighted_row; };

private slots:
    void emitCurrentIndexAvailabilityChanged() {
        emit currentIndexAvailabilityChanged(currentIndex().isValid());
    };

signals:
    void currentIndexAvailabilityChanged(bool);

private:
    bool rowContains(int r, const QString & keyword) {
        bool contains = false;
        for (int i = 0; i < columnCount(); ++i) {
            if (item(r, i)->text().contains(keyword, Qt::CaseInsensitive)) { contains = true; }
        }
        return contains;
    };
    int highlighted_row;
    SearchLineEditPalettes searchLineEditPalettes;
};

#endif // MTTABLEWIDGET_H
