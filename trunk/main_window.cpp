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

#include "main_window.h"

MainWindow::MainWindow()
{
    weblinguist_version = "0.1.0";
    f_weblinguist_version = 0.1;
    setupUi(this);
    http = new QHttp(this);
	http_buffer = new QBuffer(this);
    this->setUnifiedTitleAndToolBarOnMac(true);
    tbtn_files_add->setDefaultAction(action_Add);
    tbtn_files_remove->setDefaultAction(action_Remove);
    tbtn_add_lang->setMenu(menu_Add_language);
    tbtn_remove_lang->setMenu(menu_Remove_language);
    actgrp_all_languages = new QActionGroup(this);
    QObject::connect(actgrp_all_languages, SIGNAL(triggered(QAction *)), this, SLOT(addLanguage(QAction *)));
    QStringList languages;
    for (int i = 2; i < 166; ++i) {
        languages << QLocale::languageToString((QLocale::Language)i);
    }
    languages.sort();
    for (int i = 0; i < languages.count(); ++i) {
        QAction * a = new QAction(this);
        a->setText(languages.at(i));
        actgrp_all_languages->addAction(a);
        menu_Add_language->addAction(a);
    }
    tw_strings->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    tw_strings->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tw_strings->verticalHeader()->hide();
    actgrp_languages = new QActionGroup(this);
    QObject::connect(actgrp_languages, SIGNAL(triggered(QAction *)), this, SLOT(removeLanguage(QAction *)));
    QObject::connect(cb_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(loadHighlightedFile()));
    QObject::connect(cb_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStringCount()));
    QObject::connect(lw_recent_docs, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(openRecent(QListWidgetItem *)));
    QObject::connect(le_files_search, SIGNAL(textChanged(QLineEdit *, const QString &)), lw_files, SLOT(filterItems(QLineEdit *, const QString &)));
    QObject::connect(le_strings_search, SIGNAL(textChanged(QLineEdit *, const QString &)), tw_strings, SLOT(filterItems(QLineEdit *, const QString &)));
    QObject::connect(le_encoding, SIGNAL(textEdited(const QString &)), this, SLOT(encodingChanged(const QString &)));
    QObject::connect(action_New, SIGNAL(triggered()), this, SLOT(newProject()));
    QObject::connect(action_Open, SIGNAL(triggered()), this, SLOT(open()));
    QObject::connect(action_Save, SIGNAL(triggered()), this, SLOT(save()));
    QObject::connect(action_Close, SIGNAL(triggered()), this, SLOT(closeProject()));
    QObject::connect(action_Add, SIGNAL(triggered()), this, SLOT(addFile()));
    QObject::connect(action_Remove, SIGNAL(triggered()), this, SLOT(removeFile()));
    QObject::connect(action_Update_all, SIGNAL(triggered()), this, SLOT(updateAll()));
    QObject::connect(action_Release, SIGNAL(triggered()), this, SLOT(release()));
    QObject::connect(action_Previous_unfinished, SIGNAL(triggered()), this, SLOT(previousUnfinished()));
    QObject::connect(action_Next_unfinished, SIGNAL(triggered()), this, SLOT(nextUnfinished()));
    QObject::connect(action_Previous, SIGNAL(triggered()), this, SLOT(previous()));
    QObject::connect(action_Next, SIGNAL(triggered()), this, SLOT(next()));
    QObject::connect(action_Done_and_next, SIGNAL(triggered()), this, SLOT(doneAndNext()));
    QObject::connect(action_Add_other_language, SIGNAL(triggered()), this, SLOT(addOtherLanguage()));
    QObject::connect(action_Remove_all_languages, SIGNAL(triggered()), this, SLOT(removeAllLanguages()));
    QObject::connect(action_Check_for_updates, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
    QObject::connect(action_About_Weblinguist, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(lw_files, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(loadFile(QListWidgetItem *)));
    QObject::connect(tw_strings, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(loadString(QTableWidgetItem *)));
    QObject::connect(http, SIGNAL(done(bool)), this, SLOT(httpRequestFinished(bool)));
    setAllEnabled(false);
    loadSettings();
}

void MainWindow::addRecent(QString name)
{
    for (int i = 0; i < lw_recent_docs->count();) {
        if (lw_recent_docs->item(i)->text() == name) {
            delete lw_recent_docs->item(i);
        } else { i++; }
    }
    lw_recent_docs->insertItem(0, name);
    lw_recent_docs->setCurrentRow(0);
}

bool MainWindow::saveChangesBeforeProceeding(QString title, bool close_)
{
	if (project_open && this->isWindowModified()) {
		switch (QMessageBox::information(this, title, tr("Save changes before proceeding?"), tr("&Save"), tr("&Discard"), tr("Cancel"), 0, 2)) {
			case 0: // Save
				project_open = false;
				save(); if (close_) { closeProject(); }; return false;
				break;
			case 1: // Discard
				project_open = false;
				if (close_) { closeProject(); }; return false;
				break;
			case 2: // Cancel
				return true;
				break;
		}
	} else if (project_open && !this->isWindowModified()) {
		if (close_) { closeProject(); }; return false;
	}
	return false;
}

void MainWindow::newProject()
{
    if (saveChangesBeforeProceeding(tr("New project - Weblinguist"), true)) { return; }
    QString path = QFileDialog::getSaveFileName(this, tr("New project - Weblinguist"), tr("untitled.wlproj"), tr("Weblinguist Project (*.wlproj)"));
	if (path.isNull() || path.isEmpty()) { return; }
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
		QMessageBox::critical(this, tr("New project - Weblinguist"), tr("Cannot write file %1:\n%2.").arg(path).arg(file.errorString()));
		this->setWindowTitle(tr("Weblinguist"));
		return;
    }
    addRecent(path);
    clearAll(); setAllEnabled(true);
    project.clear();
    QDomElement root = project.createElement("wlproj");
    project.appendChild(root);
    QDomElement lang = project.createElement("lang");
    lang.setAttribute("encoding", "UTF-8");
    root.appendChild(lang);
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << project.toString(4);
    file.close();
    project_open = true;
    project_path = path;
#ifdef Q_WS_MAC
	this->setWindowTitle(QString("%1[*]").arg(QFileInfo(file).fileName()));
#else
    this->setWindowTitle(QString("%1[*] - Weblinguist").arg(QFileInfo(file).fileName()));
#endif
    this->setWindowModified(false);
    stw_main->setCurrentIndex(1);
}

void MainWindow::openRecent(QListWidgetItem * item)
{
    QString s = item->text();
    addRecent(s);
    openProject(s);
}

void MainWindow::open()
{
    if (saveChangesBeforeProceeding(tr("Open - Weblinguist"), true)) { return; }
    QString path = QFileDialog::getOpenFileName(this, tr("Open - Weblinguist"), "", tr("Weblinguist Projects (*.wlproj);;All files (*.*)"));
	if (path.isNull() || path.isEmpty()) { return; }
    addRecent(path);
    openProject(path);
}

void MainWindow::openProject(QString path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
		QMessageBox::critical(this, tr("Open - Weblinguist"), tr("Cannot read file %1:\n%2.").arg(path).arg(file.errorString()));
		this->setWindowTitle(tr("Weblinguist"));
		return;
    }
    clearAll(); setAllEnabled(true);
    project.setContent(&file);
    file.close();
    QDomNodeList files = project.elementsByTagName("file");
    for (int i = 0; i < files.count(); ++i) {
        QListWidgetItem * item = new QListWidgetItem(files.at(i).toElement().attribute("path"));
        item->setData(Qt::UserRole, files.at(i).toElement().attribute("path"));
        lw_files->addItem(item);
    }
    QDomNodeList langs = project.elementsByTagName("lang");
    if (langs.count() > 0) {
        le_encoding->setText(langs.at(0).toElement().attribute("encoding", "UTF-8"));
        QDomNodeList languages = langs.at(0).toElement().elementsByTagName("language");
        for (int i = 0; i < languages.count(); ++i) {
            cb_lang->addItem(languages.at(i).toElement().attribute("name"));
            QAction * a = new QAction(this);
            a->setText(languages.at(i).toElement().attribute("name"));
            actgrp_languages->addAction(a);
            menu_Remove_language->addAction(a);
        }
    }
    project_open = true;
    project_path = path;
#ifdef Q_WS_MAC
	this->setWindowTitle(QString("%1[*]").arg(QFileInfo(file).fileName()));
#else
    this->setWindowTitle(QString("%1[*] - Weblinguist").arg(QFileInfo(file).fileName()));
#endif
    this->setWindowModified(false);
    updateStringCount();
    stw_main->setCurrentIndex(1);
}

void MainWindow::save()
{
    saveProject(project_path);
}

void MainWindow::saveProject(QString path)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
		QMessageBox::critical(this, tr("Save project - Weblinguist"), tr("Cannot write file %1:\n%2.").arg(path).arg(file.errorString()));
		return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << project.toString(4);
    file.close();
    project_path = path;
    this->setWindowModified(false);
}

void MainWindow::closeProject()
{
	if (saveChangesBeforeProceeding(tr("Close - Weblinguist"), false)) { return; }
	clearAll(); setAllEnabled(false); project_open = false;
    gb_strings->setEnabled(false);
    gb_translation->setEnabled(false);
	this->setWindowTitle(tr("Weblinguist"));
	this->setWindowModified(false);
	stw_main->setCurrentIndex(0);
}

void MainWindow::addFile()
{
    if (!project_open) { return; }
    QString file_path = QFileDialog::getOpenFileName(this, tr("Add file - Weblinguist"), "", tr("Web pages (*.html *.htm *.shtml *.xhtml);;All files (*.*)"));
    if (file_path.isEmpty()) { return; }
    QFile file(file_path);
    if (!file.exists()) { return; }
    QString file_relpath = QFileInfo(project_path).dir().relativeFilePath(file_path);
    QDomElement root = project.documentElement();
    QDomNodeList files = root.elementsByTagName("file");
    for (int i = 0; i < files.count(); ++i) {
        if (files.at(i).toElement().attribute("path").toLower() == file_relpath.toLower()) { return; }
    }
    QDomElement element = project.createElement("file");
    element.setAttribute("path", file_relpath);
    root.appendChild(element);
    QListWidgetItem * item = new QListWidgetItem(file_relpath);
    item->setData(Qt::UserRole, file_relpath);
    lw_files->addItem(item);
    updateAll();
}

void MainWindow::removeFile()
{
    if (!project_open) { return; }
    if (lw_files->highlightedRow() < 0) { return; }
    QListWidgetItem * item = lw_files->highlightedItem();
    switch (QMessageBox::information(this, tr("Remove file - Weblinguist"), tr("Are you sure you want to remove file \"%1\" from the project?").arg(item->data(Qt::UserRole).toString()), tr("&Remove"), tr("Cancel"), 0, 1)) {
        case 0: // Remove
            break;
        case 1: // Cancel
            return; break;
    }
    QDomElement root = project.documentElement();
    QDomNodeList files = root.elementsByTagName("file");
    for (int i = 0; i < files.count(); ++i) {
        if (files.at(i).toElement().attribute("path").toLower() == item->data(Qt::UserRole).toString().toLower()) {
            root.removeChild(files.at(i));
            break;
        }
    }
    delete item;
    tw_strings->clearContents();
    tw_strings->setRowCount(0);
    gb_strings->setEnabled(false);
    txb_source->clear();
    pte_translation->clear();
    gb_translation->setEnabled(false);
    action_Remove->setEnabled(false);
    this->setWindowModified(true);
}

void MainWindow::loadHighlightedFile()
{
    loadFile(lw_files->highlightedItem());
}

void MainWindow::loadFile(QListWidgetItem * item)
{
    if (item == NULL) { return; }
    if (!project_open) { return; }
    QDomNodeList files = project.documentElement().elementsByTagName("file");
    for (int i = 0; i < files.count(); ++i) {
        if (files.at(i).toElement().attribute("path").toLower() == item->data(Qt::UserRole).toString().toLower()) {
            lw_files->highlightItem(item);
            action_Remove->setEnabled(true);
            loadFile(files.at(i).toElement());
            return;
        }
    }
}

void MainWindow::loadFile(const QDomElement & element)
{
    gb_strings->setEnabled(true);
    tw_strings->clearContents();
    QDomNodeList strings = element.elementsByTagName("string");
    tw_strings->setRowCount(strings.count());
    QTableWidgetItem * item; QDomNodeList sources, translations;
    bool hide_translation = cb_lang->currentIndex() == -1;
    if (hide_translation) {
        tw_strings->hideColumn(1);
    } else {
        tw_strings->showColumn(1);
    }
    QString lang_name = cb_lang->currentText(); QDomElement translation;
    bool obsolete = false; bool found = false;
    for (int i = 0; i < strings.count(); ++i) {
        sources = strings.at(i).toElement().elementsByTagName("source");
        translations = strings.at(i).toElement().elementsByTagName("translation");
        obsolete = strings.at(i).toElement().attribute("obsolete", "false") == "true";
        item = new QTableWidgetItem(sources.count() < 1 ? QString() : sources.at(0).toElement().text());
        if (obsolete) { item->setForeground(QBrush::QBrush(QColor::QColor(128, 128, 128))); }
        tw_strings->setItem(i, 0, item);
        found = false;
        if (!hide_translation) {
            for (int t = 0; t < translations.count(); ++t) {
                translation = translations.at(t).toElement();
                if (translation.attribute("lang") != lang_name) { continue; }
                found = true;
                item = new QTableWidgetItem(translation.text());
                if (translation.attribute("unfinished", "false") == "true") {
                    item->setData(Qt::UserRole, "unfinished;");
                    item->setIcon(QIcon(QString::fromUtf8(":/images/images/unfinished16.png")));
                } else {
                    item->setIcon(QIcon(QString::fromUtf8(":/images/images/done16.png")));
                }
                if (obsolete) { item->setForeground(QBrush::QBrush(QColor::QColor(128, 128, 128))); }
                tw_strings->setItem(i, 1, item);
                break;
            }
        }
        if (hide_translation || !found) {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, "unfinished;");
            item->setIcon(QIcon(QString::fromUtf8(":/images/images/unfinished16.png")));
            if (obsolete) { item->setForeground(QBrush::QBrush(QColor::QColor(128, 128, 128))); }
            tw_strings->setItem(i, 1, item);
        }
    }
    txb_source->clear();
    pte_translation->clear();
    gb_translation->setEnabled(false);
}

void MainWindow::loadString(QTableWidgetItem * item)
{
    loadString(tw_strings->row(item));
}

void MainWindow::loadString(int r)
{
    if (r < 0) { return; }
    if (!project_open) { return; }
    if (lw_files->highlightedRow() < 0) { return; }
    QString lang_name = cb_lang->currentText();
    QDomNodeList files = project.documentElement().elementsByTagName("file");
    for (int f = 0; f < files.count(); ++f) {
        if (files.at(f).toElement().attribute("path").toLower() == lw_files->highlightedItem()->data(Qt::UserRole).toString().toLower()) {
            QDomNodeList strings = files.at(f).toElement().elementsByTagName("string");
            if (r < strings.count()) {
                gb_translation->setEnabled(true);
                tw_strings->highlightRow(r);
                QDomNodeList sources = strings.at(r).toElement().elementsByTagName("source");
                QDomNodeList translations = strings.at(r).toElement().elementsByTagName("translation");
                txb_source->setPlainText(sources.count() < 1 ? QString() : sources.at(0).toElement().text());
                pte_translation->clear();
                for (int t = 0; t < translations.count(); ++t) {
                    if (translations.at(t).toElement().attribute("lang") != lang_name) { continue; }
                    pte_translation->setPlainText(translations.at(t).toElement().text());
                    break;
                }
                pte_translation->setFocus();
            }
            break;
        }
    }
}

void MainWindow::saveString(bool done)
{
    if (!project_open) { return; }
    if (lw_files->highlightedRow() < 0) { return; }
    int r = tw_strings->highlightedRow();
    if (r < 0) { return; }
    QString lang_name = cb_lang->currentText(); QDomElement translation;
    bool found = false; bool modified = false;
    QDomNodeList files = project.documentElement().elementsByTagName("file");
    for (int f = 0; f < files.count(); ++f) {
        if (files.at(f).toElement().attribute("path").toLower() == lw_files->highlightedItem()->data(Qt::UserRole).toString().toLower()) {
            QDomNodeList strings = files.at(f).toElement().elementsByTagName("string");
            if (r < strings.count()) {
                QDomNodeList translations = strings.at(r).toElement().elementsByTagName("translation");
                for (int t = 0; t < translations.count(); ++t) {
                    translation = translations.at(t).toElement();
                    if (translation.attribute("lang") != lang_name) { continue; }
                    found = true;
                    break;
                }
                if (!found && !pte_translation->toPlainText().isEmpty()) {
                    translation = project.createElement("translation");
                    translation.setAttribute("lang", lang_name);
                    strings.at(r).toElement().appendChild(translation);
                    modified = true;
                } else if (!found && pte_translation->toPlainText().isEmpty()) { break; }
                if (translation.text() != pte_translation->toPlainText()) {
                    while (translation.hasChildNodes()) { translation.removeChild(translation.firstChild()); }
                    QDomText translation_text = project.createTextNode(pte_translation->toPlainText());
                    translation.appendChild(translation_text);
                    modified = true;
                }
                if (done && translation.hasAttribute("unfinished"))
                    { translation.removeAttribute("unfinished"); modified = true; }
                else if (!done && !found && translation.attribute("unfinished", "false") == "false")
                    { translation.setAttribute("unfinished", "true"); modified = true; }
                if (modified) { this->setWindowModified(true); }
            }
            break;
        }
    }
    updateStringCount();
    return;
}

void MainWindow::updateAll()
{
    if (!project_open) { return; }
    QString begin_tag = "<i18n>"; QString end_tag = "</i18n>";
    QDomNodeList files = project.documentElement().elementsByTagName("file");
    QDir project_dir = QFileInfo(project_path).dir();
    QString line; QString i18n; int c = 0; int skip = 0;
    int begin = 0; int end = 0;
    QStringList i18n_list; QDomNodeList strings;
    QDomNodeList sources; QString source;
    for (int i = 0; i < files.count(); ++i) {
        QFile file(project_dir.absoluteFilePath(files.at(i).toElement().attribute("path")));
        if (!file.open(QFile::ReadOnly | QFile::Text)) { continue; }
        QTextStream s(&file); s.setCodec(QTextCodec::codecForName(le_encoding->text().toUtf8()));
        i18n_list.clear(); i18n.clear();
        while (!s.atEnd()) {
            line = s.readLine(); skip = 0;
            c = line.count(begin_tag, Qt::CaseInsensitive) + (end == -1 ? 1 : 0);
            for (int n = 0; n < c; ++n) {
                if (end == -1) { begin = 0; }
                else { begin = line.indexOf(begin_tag, skip, Qt::CaseInsensitive); }
                if (begin == -1) { break; }
                skip = begin + 1;
                if (end != -1) { begin += begin_tag.length(); }
                end = line.indexOf(end_tag, skip, Qt::CaseInsensitive);
                i18n.append(line.mid(begin, (end < 0) ? end : (end - begin)));
                if (end != -1) {
                    skip = end + 1;
                    i18n_list << i18n; i18n.clear();
                }
            }
        }
        strings = files.at(i).toElement().elementsByTagName("string");
        for (int s = 0; s < strings.count(); ++s) {
            sources = strings.at(s).toElement().elementsByTagName("source");
            if (sources.count() < 1) { continue; }
            source = sources.at(0).toElement().text();
            if (i18n_list.contains(source)) {
                i18n_list.removeAll(source);
                strings.at(s).toElement().removeAttribute("obsolete");
            } else {
                strings.at(s).toElement().setAttribute("obsolete", "true");
            }
        }
        for (int s = 0; s < i18n_list.count(); ++s) {
            QDomElement string = project.createElement("string");
            QDomElement source = project.createElement("source");
            QDomText source_text = project.createTextNode(i18n_list.at(s));
            source.appendChild(source_text);
            string.appendChild(source);
            files.at(i).toElement().appendChild(string);
        }
    }
    loadHighlightedFile();
    this->setWindowModified(true);
    updateStringCount();
}

void MainWindow::updateStringCount()
{
    if (!project_open) { return; }
    if (cb_lang->currentIndex() == -1) {
        for (int i = 0; i < lw_files->count(); ++i) {
            lw_files->item(i)->setText(lw_files->item(i)->data(Qt::UserRole).toString());
            lw_files->item(i)->setIcon(QIcon(QString::fromUtf8(":/images/images/unfinished16.png")));
        }
    } else {
        QString lang_name = cb_lang->currentText();
        QDomNodeList files = project.documentElement().elementsByTagName("file");
        QDomNodeList strings, translations; QDomElement translation;
        QString file_relpath; QListWidgetItem * item = NULL; int num_translated = 0;
        for (int f = 0; f < files.count(); ++f) {
            num_translated = 0;
            file_relpath = files.at(f).toElement().attribute("path");
            item = NULL;
            for (int i = 0; i < lw_files->count(); ++i) {
                if (lw_files->item(i)->data(Qt::UserRole).toString().toLower() == file_relpath.toLower())
                    { item = lw_files->item(i); break; }
            }
            if (item == NULL) { continue; }
            strings = files.at(f).toElement().elementsByTagName("string");
            for (int i = 0; i < strings.count(); ++i) {
                translations = strings.at(i).toElement().elementsByTagName("translation");
                for (int t = 0; t < translations.count(); ++t) {
                    translation = translations.at(t).toElement();
                    if (translation.attribute("lang") != lang_name) { continue; }
                    if (translation.attribute("unfinished", "false") == "false") { num_translated++; }
                    break;
                }
            }
            item->setText(QString("%1 (%2/%3)").arg(item->data(Qt::UserRole).toString()).arg(num_translated).arg(strings.count()));
            item->setIcon(QIcon(QString::fromUtf8(num_translated == strings.count() ? ":/images/images/done16.png" : ":/images/images/unfinished16.png")));
        }
    }
}

QString escapeDoubleQuotes(const QString & s)
{
    QString r = s;
    r.replace("\\", "\\\\");
    r.replace("\"", "\\\"");
    return r;
}

void MainWindow::release()
{
    if (!project_open) { return; }
    QDomNodeList files = project.documentElement().elementsByTagName("file");
    QDir project_dir = QFileInfo(project_path).dir();
    QDomNodeList langs = project.documentElement().elementsByTagName("lang");
    if (langs.count() < 1) { return; }
    QDomNodeList languages = langs.at(0).toElement().elementsByTagName("language");
    if (languages.count() < 1) { return; }
    QStringList list_languages; QString path; int i_last_dot, i_last_slash;
    for (int i = 0; i < languages.count(); ++i) {
        list_languages << languages.at(i).toElement().attribute("name");
    }
    QDomNodeList strings, sources, translations; QDomElement translation; bool found = false;
    for (int f = 0; f < files.count(); ++f) {
        path = project_dir.absoluteFilePath(files.at(f).toElement().attribute("path"));
        i_last_dot = path.lastIndexOf("."); i_last_slash = path.lastIndexOf("/");
        if (i_last_slash < i_last_dot && i_last_dot != -1) { path.truncate(i_last_dot); }
        path.append(".i18n.js");
        QFile file(path);
        if (!file.open(QFile::WriteOnly | QFile::Text)) { continue; }
        QTextStream s(&file); s.setCodec(QTextCodec::codecForName(le_encoding->text().toUtf8()));
        s << "function Dictionary(startValues) {" << endl;
        s << "    this.values = startValues || {};" << endl;
        s << "}" << endl;
        s << "Dictionary.prototype.add = function(name, value) {" << endl;
        s << "    this.values[name] = value;" << endl;
        s << "};" << endl;
        s << "Dictionary.prototype.value = function(name) {" << endl;
        s << "    return this.values[name];" << endl;
        s << "};" << endl;
        s << "Dictionary.prototype.contains = function(name) {" << endl;
        s << "    return Object.prototype.hasOwnProperty.call(this.values, name) &&" << endl;
        s << "        Object.prototype.propertyIsEnumerable.call(this.values, name);" << endl;
        s << "};" << endl;
        s << "Dictionary.prototype.each = function(action) {" << endl;
        s << "    forEachIn(this.values, action);" << endl;
        s << "};" << endl;
        s << "function getUrlParameter(name) {" << endl;
        s << "    name = name.replace(/[\\[]/, \"\\\\\\[\").replace(/[\\]]/, \"\\\\\\]\");" << endl;
        s << "    var regexS = \"[\\\\?&]\" + name + \"=([^&#]*)\";" << endl;
        s << "    var regex = new RegExp(regexS);" << endl;
        s << "    var results = regex.exec(window.location.href);" << endl;
        s << "    if (results == null) return ""; else return results[1];" << endl;
        s << "}" << endl;
        s << "function onLoad() {" << endl;
        s << "    translate(getUrlParameter(\"lang\"));" << endl;
        s << "}" << endl;
        s << "function translate(l) {" << endl;
        s << "    var lang = l.toLowerCase();" << endl;
        s << "    var elements = document.getElementsByTagName(\"i18n\");" << endl;
        s << "    for (var i = 0; i < elements.length; i++) {" << endl;
        s << "        if (langs.value(lang).contains(elements[i].innerHTML))" << endl;
        s << "            elements[i].innerHTML = langs.value(lang).value(elements[i].innerHTML);" << endl;
        s << "    }" << endl;
        s << "    var links = document.getElementsByTagName(\"a\");" << endl;
        s << "    for (var i = 0; i < links.length; i++) {" << endl;
        s << "        if (links[i].hasAttribute(\"external\")) { continue; }" << endl;
        s << "        if (!links[i].hasAttribute(\"href\")) { continue; }" << endl;
        s << "        var href = links[i].getAttribute(\"href\");" << endl;
        s << "        if (href.indexOf(\"?\") == -1) { links[i].setAttribute(\"href\", href + \"?lang=\" + lang); }" << endl;
        s << "        else { links[i].setAttribute(\"href\", href + \"&lang=\" + lang); }" << endl;
        s << "    }" << endl;
        s << "}" << endl << endl;
        strings = files.at(f).toElement().elementsByTagName("string");
        for (int l = 0; l < list_languages.count(); ++l) {
            s << QString("var dict_%1 = new Dictionary();").arg(list_languages.at(l)) << endl;
            for (int i = 0; i < strings.count(); ++i) {
                sources = strings.at(i).toElement().elementsByTagName("source");
                translations = strings.at(i).toElement().elementsByTagName("translation");
                if (sources.count() < 1) { continue; }
                found = false;
                for (int t = 0; t < translations.count(); ++t) {
                    translation = translations.at(t).toElement();
                    if (translation.attribute("lang") != list_languages.at(l)) { continue; }
                    found = true;
                    break;
                }
                if (found) {
                    s << QString("dict_%1.add(\"%2\", \"%3\");").arg(list_languages.at(l)).arg(escapeDoubleQuotes(sources.at(0).toElement().text())).arg(escapeDoubleQuotes(translation.text())) << endl;
                } else {
                    s << QString("dict_%1.add(\"%2\", \"%2\");").arg(list_languages.at(l)).arg(escapeDoubleQuotes(sources.at(0).toElement().text())) << endl;
                }
            }
            s << endl;
        }
        s << "var langs = new Dictionary();" << endl;
        for (int l = 0; l < list_languages.count(); ++l) {
            s << QString("langs.add(\"%1\", dict_%2);").arg(list_languages.at(l).toLower()).arg(list_languages.at(l)) << endl;
        }
        file.close();
    }
}

void MainWindow::previousUnfinished()
{
    saveString(false);
    int r = tw_strings->highlightedRow();
    loadHighlightedFile();
    if (r < 0) { r = tw_strings->rowCount(); }
    bool found = false;
    for (int i = r - 1; i >= 0 && !found; --i) {
        if (tw_strings->item(i, 1)->data(Qt::UserRole).toString().contains("unfinished")) { found = true; r = i; }
    }
    for (int i = tw_strings->rowCount() - 1; i > r && !found; --i) {
        if (tw_strings->item(i, 1)->data(Qt::UserRole).toString().contains("unfinished")) { found = true; r = i; }
    }
    loadString(r);
}

void MainWindow::nextUnfinished()
{
    saveString(false);
    int r = tw_strings->highlightedRow();
    loadHighlightedFile();
    if (r < 0) { r = -1; }
    bool found = false;
    for (int i = r + 1; i < tw_strings->rowCount() && !found; ++i) {
        if (tw_strings->item(i, 1)->data(Qt::UserRole).toString().contains("unfinished")) { found = true; r = i; }
    }
    for (int i = 0; i < r && !found; ++i) {
        if (tw_strings->item(i, 1)->data(Qt::UserRole).toString().contains("unfinished")) { found = true; r = i; }
    }
    loadString(r);
}

void MainWindow::previous()
{
    saveString(false);
    int r = tw_strings->highlightedRow();
    loadHighlightedFile();
    if (r < 0) { r = tw_strings->rowCount() - 1; }
    else if (r == 0) { r = tw_strings->rowCount() - 1; } else { r--; }
    loadString(r);
}

void MainWindow::next()
{
    saveString(false);
    int r = tw_strings->highlightedRow();
    loadHighlightedFile();
    if (r < 0) { r = 0; }
    else if (r >= tw_strings->rowCount() - 1) { r = 0; } else { r++; }
    loadString(r);
}

void MainWindow::doneAndNext()
{
    saveString(true);
    int r = tw_strings->highlightedRow();
    loadHighlightedFile();
    if (r < 0) { r = -1; }
    bool found = false;
    for (int i = r + 1; i < tw_strings->rowCount() && !found; ++i) {
        if (tw_strings->item(i, 1)->data(Qt::UserRole).toString().contains("unfinished")) { found = true; r = i; }
    }
    for (int i = 0; i < r && !found; ++i) {
        if (tw_strings->item(i, 1)->data(Qt::UserRole).toString().contains("unfinished")) { found = true; r = i; }
    }
    loadString(r);
}

void MainWindow::addOtherLanguage()
{
    if (!project_open) { return; }
    bool ok;
    QString lang_name = QInputDialog::getText(this, tr("Add other language - Weblinguist"), tr("Language name:"), QLineEdit::Normal, "", &ok);
    if (!ok || lang_name.isEmpty()) { return; }
    QDomElement root = project.documentElement();
    QDomNodeList langs = root.elementsByTagName("lang");
    QDomElement lang;
    if (langs.count() < 1) {
        lang = project.createElement("lang");
        lang.setAttribute("encoding", "UTF-8");
        root.appendChild(lang);
    } else {
        lang = langs.at(0).toElement();
    }
    QDomNodeList languages = lang.elementsByTagName("language");
    for (int i = 0; i < languages.count(); ++i) {
        if (languages.at(i).toElement().attribute("name") == lang_name) { return; }
    }
    QDomElement language = project.createElement("language");
    language.setAttribute("name", lang_name);
    lang.appendChild(language);
    cb_lang->addItem(lang_name);
    QAction * a = new QAction(this);
    a->setText(lang_name);
    actgrp_languages->addAction(a);
    menu_Remove_language->addAction(a);
    this->setWindowModified(true);
}

void MainWindow::addLanguage(QAction * a)
{
    if (!project_open) { return; }
    QString lang_name = a->text();
    QDomNodeList langs = project.documentElement().elementsByTagName("lang");
    if (langs.count() < 1) { return; }
    QDomNodeList languages = langs.at(0).toElement().elementsByTagName("language");
    for (int i = 0; i < languages.count(); ++i) {
        if (languages.at(i).toElement().attribute("name") == lang_name)
            { return; }
    }
    QDomElement language = project.createElement("language");
    language.setAttribute("name", lang_name);
    langs.at(0).toElement().appendChild(language);
    cb_lang->addItem(lang_name);
    QAction * new_a = new QAction(this);
    new_a->setText(lang_name);
    actgrp_languages->addAction(new_a);
    menu_Remove_language->addAction(new_a);
    this->setWindowModified(true);
}

void MainWindow::removeAllLanguages()
{
    if (!project_open) { return; }
    QDomNodeList langs = project.documentElement().elementsByTagName("lang");
    if (langs.count() < 1) { return; }
    QDomNodeList languages = langs.at(0).toElement().elementsByTagName("language");
    for (int i = 0; i < languages.count(); ++i) {
        langs.at(0).toElement().removeChild(languages.at(i));
    }
    QList<QAction *> actions = actgrp_languages->actions();
    for (int i = 0; i < actions.count();) {
        actgrp_languages->removeAction(actions.at(i));
        delete actions.takeAt(i);
    }
    cb_lang->clear();
    this->setWindowModified(true);
}

void MainWindow::removeLanguage(QAction * a)
{
    if (!project_open) { return; }
    QString lang_name = a->text();
    QDomNodeList langs = project.documentElement().elementsByTagName("lang");
    if (langs.count() < 1) { return; }
    QDomNodeList languages = langs.at(0).toElement().elementsByTagName("language");
    for (int i = 0; i < languages.count(); ++i) {
        if (languages.at(i).toElement().attribute("name") == lang_name)
            { langs.at(0).toElement().removeChild(languages.at(i)); }
    }
    delete a;
    for (int i = 0; i < cb_lang->count();) {
        if (cb_lang->itemText(i) == lang_name) { cb_lang->removeItem(i); }
        else { i++; }
    }
    this->setWindowModified(true);
}

void MainWindow::encodingChanged(const QString & s)
{
    if (!project_open) { return; }
    QDomNodeList langs = project.documentElement().elementsByTagName("lang");
    if (langs.count() < 1) { return; }
    langs.at(0).toElement().setAttribute("encoding", s);
    this->setWindowModified(true);
}

void MainWindow::clearAll()
{
    lw_files->clear();
    tw_strings->clearContents();
    tw_strings->setRowCount(0);
    txb_source->clear();
    pte_translation->clear();
    le_encoding->setText("UTF-8");
    cb_lang->clear();
    for (int i = 0; i < menu_Remove_language->actions().count();) {
        if (menu_Remove_language->actions().at(i) != action_Remove_all_languages && !menu_Remove_language->actions().at(i)->isSeparator())
            { menu_Remove_language->removeAction(menu_Remove_language->actions().at(i)); }
        else { i++; }
    }
}

void MainWindow::setAllEnabled(bool enable)
{
    menu_File->setEnabled(enable);
    menu_Translation->setEnabled(enable);
    tlb_translation->setEnabled(enable);
    menu_Language->setEnabled(enable);
    action_Save->setEnabled(enable);
    action_Close->setEnabled(enable);
    action_Update_all->setEnabled(enable);
    action_Release->setEnabled(enable);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if (!saveChangesBeforeProceeding(tr("Quit Weblinguist"), true)) {
        saveSettings();
        event->accept();
    } else { event->ignore(); }
}

void MainWindow::loadSettings()
{
    QSettings settings("Michal Tomlein", "Weblinguist");
    lw_recent_docs->addItems(settings.value("recent_docs").toStringList());
    this->move(settings.value("pos", this->pos()).toPoint());
    this->resize(settings.value("size", this->size()).toSize());
}

void MainWindow::saveSettings()
{
    QSettings settings("Michal Tomlein", "Weblinguist");
    QStringList recent;
    for (int i = 0; i < lw_recent_docs->count(); ++i)
    { recent << lw_recent_docs->item(i)->text(); }
    settings.setValue("recent_docs", recent);
    settings.setValue("pos", this->pos());
    settings.setValue("size", this->size());
}

void MainWindow::checkForUpdates()
{
	delete http_buffer; http_buffer = new QBuffer(this);
    http->setHost("michal.tomlein.googlepages.com");
	http->get("/weblinguist-current-version", http_buffer);
}

void MainWindow::httpRequestFinished(bool error)
{
	httpRequestFinished_start:
	if (error) {
		switch (QMessageBox::critical(this, tr("Weblinguist"), tr("Failed to check for updates."), tr("&Try again"), tr("Cancel"), 0, 1)) {
			case 0: // Try again
				checkForUpdates(); return; break;
			case 1: // Cancel
				return; break;
		}
	}
	QString str(http_buffer->data()); QTextStream in(&str);
	if (in.readLine() != "[Weblinguist.current-version]") { error = true; goto httpRequestFinished_start; }
	QString current_ver = in.readLine();
	if (in.readLine() != "[Weblinguist.current-version.float]") { error = true; goto httpRequestFinished_start; }
	float f_current_ver = in.readLine().toFloat();
	if (in.readLine() != "[Weblinguist.release-notes]") { error = true; goto httpRequestFinished_start; }
	QString release_notes;
	while (!in.atEnd()) { release_notes.append(in.readLine()); }
	if (f_current_ver <= f_weblinguist_version) {
		QMessageBox::information(this, tr("Weblinguist"), tr("Your Weblinguist is up-to-date."));
	} else {
		QString info; QTextStream out(&info);
		out << "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body><p>" << endl;
		out << "<b>" << tr("Weblinguist %1 is available now.").arg(current_ver) << "</b><br><br>" << endl;
		out << release_notes << endl << "</p></body></html>";
		QMessageBox::information(this, tr("Weblinguist"), info);
	}
}

void MainWindow::about()
{
    AboutWidget * weblinguist_about = new AboutWidget(weblinguist_version, QString("2008"));
	weblinguist_about->setParent(this);
    weblinguist_about->setWindowFlags(Qt::Dialog /*| Qt::WindowMaximizeButtonHint*/ | Qt::WindowStaysOnTopHint);
	weblinguist_about->show();
}
