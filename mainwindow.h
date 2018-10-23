#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace Ui {
class MainWindow;
}

namespace std {
template<>
struct hash<QByteArray> {
    std::size_t operator()(const QByteArray &s) const {
        return qHash(s);
    }
};
}

class main_window : public QMainWindow {
Q_OBJECT

public:
    explicit main_window(QWidget *parent = 0);
    ~main_window();
private:
    void show_duplicates(QString const &dir);
private slots:
    void select_directory();
    void scan_directory(QString const &dir);
    void show_about_dialog();
    void delete_items();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unordered_map<QByteArray, std::vector<QString>> duplicates;
};

#endif // MAINWINDOW_H
