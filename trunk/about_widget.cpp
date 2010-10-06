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

#include "about_widget.h"

AboutWidget::AboutWidget(QString ver, QString year)
{
	setupUi(this);
	QObject::connect(btn_close, SIGNAL(released()), this, SLOT(close()));
	QString about = "<p style=\"font-family: sans-serif; font-style:italic;\"><span style=\"font-size:12pt;\">Weblinguist</span><br>";
	about.append("<span style=\"font-size:8pt;\">");
    about.append(tr("Version"));
    about.append(QString(" %1</span></p><p></p>").arg(ver));
	about.append("<p style=\"font-family: sans-serif; font-size:8pt;\">");
    about.append(tr("Weblinguist is a Qt web development tool which allows you to translate web pages with ease. It features an intuitive user interface similar to Qt Linguist."));
    about.append("</p><p></p>");
	about.append("<p style=\"font-family: sans-serif; font-size:8pt;\">");
    about.append(tr("This program is distributed under the terms of the GPL v2."));
	about.append("</p><p></p>");
	about.append(QString("<p style=\"font-family: sans-serif; font-size:8pt;\">%1 <span style=\"font-style:italic;\">Michal Tomlein</span> (michal.tomlein@gmail.com)</p>").arg(year));
	about.append("<p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
	about.append(tr("The program is provided AS IS with ABSOLUTELY NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."));
	about.append("</p>");
#ifdef Q_WS_MAC
	about.remove("font-family: sans-serif;");
	about.replace("font-size:12pt;", "font-size:14pt;");
	about.replace("font-size:8pt;", "font-size:10pt;");
#endif
    txb_about->setHtml(about);
	QString about_qt = "<p style=\"font-family: sans-serif; font-style:italic;\"><span style=\"font-size:12pt;\">";
    about_qt.append(tr("About Qt"));
	about_qt.append("</span></p><p></p><p style=\"font-family: sans-serif; font-size:8pt; font-style:italic;\">");
	about_qt.append(tr("This program uses Qt Open Source Edition version %1.").arg(qVersion()));
	about_qt.append("</p><p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
	about_qt.append(tr("Qt is a C++ toolkit for cross-platform application development."));
	about_qt.append("</p><p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
	about_qt.append(tr("Qt provides single-source portability across MS Windows, Mac OS X, Linux and all major commercial Unix variants. Qt is also available for embedded devices as Qtopia Core."));
	about_qt.append("</p><p></p><p style=\"font-family: sans-serif; font-size:8pt;\">");
	about_qt.append(tr("Qt is a Trolltech product. See <span style=\"font-style:italic;\">http://www.trolltech.com/qt/</span> for more information."));
	about_qt.append("</p>");
#ifdef Q_WS_MAC
	about_qt.remove("font-family: sans-serif;");
	about_qt.replace("font-size:12pt;", "font-size:14pt;");
	about_qt.replace("font-size:8pt;", "font-size:10pt;");
#endif
    txb_about_qt->setHtml(about_qt);
}
