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
#include "aboutdialog.h"
#include <QtGui>
#include <QGridLayout>

AboutDialog::AboutDialog(QWidget *parent,
	const QString &appname, const QString &version,
	const QString &copyright, const QString &description)
	: QDialog(parent, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint)
{
	QPushButton *okButton = new QPushButton(tr("OK"), this);
	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
	
	QLabel *icon = new QLabel();
	icon->setPixmap(parent->windowIcon().pixmap(32, 32));
	QLabel *app = new QLabel(
		QString("<center><big><strong>%1 %2</strong></big></center>")
			.arg(appname).arg(version));
	QLabel *copy = new QLabel(QString("<center>%1</center>").arg(copyright));
	QLabel *text = new QLabel(description);
	text->setWordWrap(false);
	
	QGridLayout *grid = new QGridLayout(this);
	
	grid->addWidget(icon, 0, 0, 3, 1, Qt::AlignTop);
	grid->addWidget(app, 0, 1);
	grid->addWidget(copy, 1, 1);
	grid->addWidget(text, 2, 1);
	grid->addWidget(okButton, 3, 0, 1, 2, Qt::AlignHCenter);
	
	setWindowTitle(tr("About %1").arg(appname));
	setLayout(grid);
	setFixedSize(sizeHint());
}
