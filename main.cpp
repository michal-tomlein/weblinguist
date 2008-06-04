/******************************************************************************
 *                                 Weblinguist                                *
 * -------------------------------------------------------------------------- *
 * Version:      0.1.0                                                        *
 * Qt version:   4.4.0 or higher required                                     *
 * -------------------------------------------------------------------------- *
 * Weblinguist is a Qt web development tool which allows you to translate web *
 * pages with ease.                                                           *
 * -------------------------------------------------------------------------- *
 * Weblinguist is distributed under the terms of the GPL v2, details below.   *
 ******************************************************************************/

#include <QApplication>

#include "main_window.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MainWindow * weblinguist_window = new MainWindow;
	weblinguist_window->show();
	return app.exec();
}

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