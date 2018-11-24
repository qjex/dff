#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "duplicates_scanner.h"

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow {
Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();
private:
    QString human_size(qint64 size);
private slots:
    void select_directory();
    void show_about_dialog();
    void delete_items();
    void show_duplicates(std::unordered_map<QByteArray, std::vector<QString>> const &duplicates);
    void show_status(QString const &);
    void exit();
private:
    std::unique_ptr<Ui::MainWindow> ui;
    duplicates_scanner *scanner = nullptr;
};

#endif // MAINWINDOW_H
