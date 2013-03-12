/*
 *   Citybuilding Mappers - create minimaps from citybuilding game files
 *   Copyright (C) 2007, 2008  Bianca van Schaik
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class AboutDialog : public QDialog {
	public:
		/**
		* Constructor, creates a new about dialog
		* @param parent Parent widget
		* @param appname Name of the application, used in title bar and aboutbox
		* @param version Version of the application
		* @param copyright Line of copyright information, HTML
		* @param description Short description of the application, plain text
		*/
		AboutDialog(QWidget *parent,
			const QString &appname, const QString &version,
			const QString &copyright,
			const QString &description);
};

#endif /* ABOUTDIALOG_H */
