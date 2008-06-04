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

#include "ui_main_window.h"
#include "about_widget.h"

#include <QHeaderView>
#include <QDomDocument>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QTextCodec>
#include <QSettings>
#include <QHttp>
#include <QBuffer>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openRecent(QListWidgetItem *);
    void newProject();
    void open();
    void save();
    void closeProject();
    void addFile();
    void removeFile();
    void loadHighlightedFile();
    void loadFile(QListWidgetItem *);
    void loadString(QTableWidgetItem *);
    void loadString(int);
    void saveString(bool);
    void updateAll();
    void updateStringCount();
    void release();
    void previousUnfinished();
    void nextUnfinished();
    void previous();
    void next();
    void doneAndNext();
    void addOtherLanguage();
    void addLanguage(QAction *);
    void removeAllLanguages();
    void removeLanguage(QAction *);
    void encodingChanged(const QString &);
    void checkForUpdates();
    void httpRequestFinished(bool);
    void about();

private:
    void addRecent(QString);
    bool saveChangesBeforeProceeding(QString, bool);
    void openProject(QString);
    void saveProject(QString);
    void loadFile(const QDomElement &);
    void clearAll();
    void setAllEnabled(bool);
    void closeEvent(QCloseEvent *);
    void loadSettings();
    void saveSettings();

    QDomDocument project;
    bool project_open;
    QString project_path;
    QActionGroup * actgrp_languages;
    QActionGroup * actgrp_all_languages;
    QHttp * http; QBuffer * http_buffer;
    QString weblinguist_version; float f_weblinguist_version;
};
