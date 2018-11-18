#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow {
Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();
private:
    void show_duplicates(const QString &dir,
                         std::unordered_map<QByteArray, std::vector<QString>> &duplicates);
    QString human_size(qint64 size);
private slots:
    void select_directory();
    void show_about_dialog();
    void delete_items();

private:
    std::unique_ptr<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
