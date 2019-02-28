/*
Copyright (c) 2019 Revanth Babu, Pradeep Moturi, Jeevan Chandra, Udit Maniyar

This file is part of Arkanoid 
(see https://github.com/IITH-SBJoshi/concurrency-1).

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <QWidget>

namespace Ui {
class pause_menu;
}

class pause_menu : public QWidget
{
    Q_OBJECT

public:
    explicit pause_menu(QWidget *parent = nullptr);
    ~pause_menu();

private slots:
    void on_exit_clicked();
    void on_resume_clicked();
    void on_replay_clicked();
signals:
    void start();

private:
    Ui::pause_menu *ui;
};

#endif // PAUSE_MENU_H
