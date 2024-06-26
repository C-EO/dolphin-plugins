/*
    SPDX-FileCopyrightText: 2011 Vishesh Yadav <vishesh3y@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef HGIMPORTDIALOG_H
#define HGIMPORTDIALOG_H

#include "dialogbase.h"

class QListWidget;
class QCheckBox;
class QGroupBox;

/**
 * Implements dialog to import changesets from patch files generated by
 * Mercurial's export command
 */
class HgImportDialog : public DialogBase
{
    Q_OBJECT

public:
    explicit HgImportDialog(QWidget *parent = nullptr);

public Q_SLOTS:
    void done(int r) override;

private Q_SLOTS:
    void saveGeometry();
    void slotAddPatches();
    void slotRemovePatches();

private:
    void setupUI();
    void getPatchInfo(const QString &fileName);

private:
    QListWidget *m_patchList;
    QPushButton *m_addPatches;
    QPushButton *m_removePatches;

    // options
    QGroupBox *m_optionGroup;
    QCheckBox *m_optNoCommit;
    QCheckBox *m_optBypass;
    QCheckBox *m_optExact;
    QCheckBox *m_optForce;
};

#endif /* HGIMPORTDIALOG_H */
