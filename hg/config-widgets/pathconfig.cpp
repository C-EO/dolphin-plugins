/*
    SPDX-FileCopyrightText: 2011 Vishesh Yadav <vishesh3y@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "pathconfig.h"
#include "hgconfig.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <QAction>
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

HgPathConfigWidget::HgPathConfigWidget(QWidget *parent)
    : QWidget(parent)
    , m_loadingCell(false)
    , m_allValidData(true)
    , m_newAdd(false)
{
    setupUI();
    loadConfig();
}

void HgPathConfigWidget::setupContextMenu()
{
    m_addAction = new QAction(this);
    m_addAction->setIcon(QIcon::fromTheme(QStringLiteral("add")));
    m_addAction->setText(xi18nc("@action:inmenu", "Add"));
    connect(m_addAction, SIGNAL(triggered()), this, SLOT(slotAddPath()));

    m_modifyAction = new QAction(this);
    m_modifyAction->setIcon(QIcon::fromTheme(QStringLiteral("edit")));
    m_modifyAction->setText(xi18nc("@action:inmenu", "Edit"));
    connect(m_modifyAction, SIGNAL(triggered()), this, SLOT(slotModifyPath()));

    m_deleteAction = new QAction(this);
    m_deleteAction->setIcon(QIcon::fromTheme(QStringLiteral("remove")));
    m_deleteAction->setText(xi18nc("@action:inmenu", "Remove"));
    connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(slotDeletePath()));

    m_contextMenu = new QMenu(this);
    m_contextMenu->addAction(m_addAction);
    m_contextMenu->addAction(m_modifyAction);
    m_contextMenu->addAction(m_deleteAction);

    connect(m_pathsListWidget, &QTableWidget::cellChanged, this, &HgPathConfigWidget::slotCellChanged);
    connect(m_pathsListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    connect(m_pathsListWidget, &QTableWidget::customContextMenuRequested, this, &HgPathConfigWidget::slotContextMenuRequested);
}

void HgPathConfigWidget::setupUI()
{
    // add, remove, modify buttons goes here
    QHBoxLayout *actionsLayout = new QHBoxLayout;
    m_addPathButton = new QPushButton(xi18nc("@label:button", "Add"));
    m_modifyPathButton = new QPushButton(xi18nc("@label:button", "Edit"));
    m_deletePathButton = new QPushButton(xi18nc("@label:button", "Remove"));

    actionsLayout->addWidget(m_addPathButton);
    actionsLayout->addWidget(m_modifyPathButton);
    actionsLayout->addWidget(m_deletePathButton);

    connect(m_addPathButton, SIGNAL(clicked()), this, SLOT(slotAddPath()));
    connect(m_modifyPathButton, SIGNAL(clicked()), this, SLOT(slotModifyPath()));
    connect(m_deletePathButton, SIGNAL(clicked()), this, SLOT(slotDeletePath()));

    /// create and setup the Table Widget
    m_pathsListWidget = new QTableWidget;
    setupContextMenu(); // make context menu

    m_pathsListWidget->setColumnCount(2);
    m_pathsListWidget->verticalHeader()->hide();
    m_pathsListWidget->horizontalHeader()->hide();
    m_pathsListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pathsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pathsListWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_pathsListWidget->horizontalHeader()->setStretchLastSection(true);
    m_pathsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // setup main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(actionsLayout);
    mainLayout->addWidget(m_pathsListWidget);
    setLayout(mainLayout);
}

void HgPathConfigWidget::loadConfig()
{
    HgConfig hgc(HgConfig::RepoConfig);
    m_remotePathMap = hgc.repoRemotePathList();
    m_loadingCell = true;

    m_pathsListWidget->clearContents();
    m_removeList.clear();

    QMutableMapIterator<QString, QString> it(m_remotePathMap);
    int count = 0;
    while (it.hasNext()) {
        it.next();

        QTableWidgetItem *alias = new QTableWidgetItem;
        QTableWidgetItem *path = new QTableWidgetItem;

        alias->setText(it.key());
        path->setText(it.value());

        m_pathsListWidget->insertRow(count);
        m_pathsListWidget->setItem(count, 0, alias);
        m_pathsListWidget->setItem(count, 1, path);
    }

    m_pathsListWidget->resizeRowsToContents();
    m_loadingCell = false;
}

void HgPathConfigWidget::saveConfig()
{
    HgConfig hgc(HgConfig::RepoConfig);

    if (!m_allValidData) {
        return;
    }

    // first delete the alias in remove list from hgrc
    for (const QString &alias : std::as_const(m_removeList)) {
        hgc.deleteRepoRemotePath(alias);
    }

    // now save the new map in table to hgrc
    QMutableMapIterator<QString, QString> it(m_remotePathMap);
    while (it.hasNext()) {
        it.next();
        QString alias = it.key();
        QString url = it.value();
        hgc.setRepoRemotePath(alias, url);
    }
}

void HgPathConfigWidget::slotContextMenuRequested(const QPoint &pos)
{
    if (m_pathsListWidget->indexAt(pos).isValid()) {
        m_deleteAction->setEnabled(true);
        m_modifyAction->setEnabled(true);
    } else {
        m_deleteAction->setEnabled(false);
        m_modifyAction->setEnabled(false);
    }
    m_addAction->setEnabled(true);
    m_contextMenu->exec(m_pathsListWidget->mapToGlobal(pos));
}

void HgPathConfigWidget::slotAddPath()
{
    QTableWidgetItem *alias = new QTableWidgetItem;
    QTableWidgetItem *path = new QTableWidgetItem;

    int count = m_pathsListWidget->rowCount();
    m_loadingCell = true;
    m_pathsListWidget->insertRow(count);
    m_pathsListWidget->setItem(count, 0, alias);
    m_pathsListWidget->setItem(count, 1, path);
    m_pathsListWidget->resizeRowsToContents();
    m_pathsListWidget->setCurrentItem(alias);
    m_pathsListWidget->editItem(m_pathsListWidget->item(count, 0));
    m_loadingCell = false;
    m_newAdd = true;
}

void HgPathConfigWidget::slotDeletePath()
{
    int currentRow = m_pathsListWidget->currentRow();
    m_removeList << m_pathsListWidget->item(currentRow, 0)->text();
    m_remotePathMap.remove(m_pathsListWidget->item(currentRow, 0)->text());
    m_pathsListWidget->removeRow(currentRow);
}

void HgPathConfigWidget::slotModifyPath()
{
    m_pathsListWidget->editItem(m_pathsListWidget->currentItem());
}

void HgPathConfigWidget::slotCellChanged(int row, int col)
{
    if (m_loadingCell || m_oldSelValue == m_pathsListWidget->currentItem()->text()) {
        return;
    }

    QTableWidgetItem *alias = m_pathsListWidget->item(row, 0);
    QTableWidgetItem *url = m_pathsListWidget->item(row, 1);
    if (alias->text().isEmpty() || url->text().isEmpty()) {
        alias->setBackground(Qt::red);
        url->setBackground(Qt::red);
        m_allValidData = false;
        return;
    } else if (m_remotePathMap.contains(alias->text()) && m_newAdd) {
        m_oldSelValue = m_pathsListWidget->currentItem()->text();
        alias->setBackground(Qt::red);
        url->setBackground(Qt::red);
        m_allValidData = false;
        return;
    } else if (m_remotePathMap.contains(alias->text()) && col == 0) {
        m_oldSelValue = m_pathsListWidget->currentItem()->text();
        alias->setBackground(Qt::red);
        url->setBackground(Qt::red);
        m_allValidData = false;
        return;
    } else {
        qDebug() << "bingo";
        if (!m_newAdd && col == 0) {
            m_remotePathMap.remove(m_oldSelValue);
            m_removeList << m_oldSelValue;
        }
        m_remotePathMap.insert(alias->text(), url->text());
        m_oldSelValue = m_pathsListWidget->currentItem()->text();
        alias->setBackground(Qt::NoBrush);
        url->setBackground(Qt::NoBrush);
        m_allValidData = true;
    }

    m_newAdd = false;
}

void HgPathConfigWidget::slotSelectionChanged()
{
    m_oldSelValue = m_pathsListWidget->currentItem()->text();
}

#include "moc_pathconfig.cpp"
