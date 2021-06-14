/***************************************************************************
 *   Copyright (C) 2011 by Vishesh Yadav <vishesh3y@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef HGCONFIG_IGNOREWIDGET_H
#define HGCONFIG_IGNOREWIDGET_H

#include <QWidget>

class QListWidget;
class QPushButton;
class QInputDialog;

/**
 * Widget to manage ignored files. Used .hgignore file in repository.
 * Repository only configuration
 */
class HgIgnoreWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HgIgnoreWidget(QWidget *parent = nullptr);

    void loadConfig();
    void saveConfig();

private Q_SLOTS:
    void slotAddFiles();
    void slotAddPattern();
    void slotRemoveEntries();
    void slotEditEntry();

private:
    void setupUI();
    void setupUntrackedList();

private:
    QListWidget *m_ignoreTable;
    QListWidget *m_untrackedList;
    QPushButton *m_addFiles;
    QPushButton *m_addPattern;
    QPushButton *m_removeEntries;
    QPushButton *m_editEntry;
};

#endif /* HGCONFIG_IGNOREWIDGET_H */

