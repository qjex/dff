#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicates_scanner.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QtWidgets>
#include <QLocale>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    qRegisterMetaType<std::unordered_map<QByteArray, std::vector<QString>>>(
        "std::unordered_map<QByteArray,std::vector<QString>>");
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeWidget->setUniformRowHeights(true);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &main_window::exit);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);
    connect(ui->pushButton, SIGNAL(released()), SLOT(delete_items()));
}

main_window::~main_window() {}

void main_window::select_directory() {
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    "Select Directory for Scanning",
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->treeWidget->clear();
    setWindowTitle(QString("Duplicates in: %1").arg(dir));
    scanner = new duplicates_scanner(dir);
    QObject::connect(scanner,
                     SIGNAL(send_duplicates(std::unordered_map<QByteArray, std::vector<QString>> const &)),
                     this,
                     SLOT(show_duplicates(std::unordered_map<QByteArray, std::vector<QString>> const &)));
    QObject::connect(scanner,
                     SIGNAL(send_status(QString const &)),
                     this,
                     SLOT(show_status(QString const &)));
    connect(scanner, SIGNAL (finished()), scanner, SLOT (deleteLater()));
    ui->actionScan_Directory->setDisabled(true);
    scanner->start();
}

void main_window::show_duplicates(std::unordered_map<QByteArray, std::vector<QString>> const &duplicates) {
    ui->treeWidget->clear();
    size_t total_cnt = 0;
    for (auto &h : duplicates) {
        if (h.second.size() < 2) {
            continue;
        }
        total_cnt += h.second.size();
        QTreeWidgetItem *separator_item = new QTreeWidgetItem(ui->treeWidget);
        separator_item->setExpanded(true);
        separator_item->setText(0, QString("%1 duplicates").arg(h.second.size()));
        qint64 sz = -1;
        for (auto &file_path : h.second) {
            QTreeWidgetItem *item = new QTreeWidgetItem(separator_item);
            QFileInfo file_info(file_path);
            if (sz == -1) {
                sz = file_info.size();
            }
            item->setText(0, file_path);
            item->setToolTip(0, file_path);
            ui->treeWidget->addTopLevelItem(item);
        }
        separator_item->setText(1, human_size(static_cast<qint64>(h.second.size()) * sz));
    }
    setWindowTitle(QString("Found %1 duplicates").arg(total_cnt));
    ui->actionScan_Directory->setDisabled(false);
}

void main_window::delete_items() {
    QList<QTreeWidgetItem *> sel_items = ui->treeWidget->selectedItems();
    for (auto item : sel_items) {
        QString path = item->text(0);
        if (path.isEmpty()) {
            continue;
        }
        QFile file(path);
        if (file.remove()) {
            item->setDisabled(true);
        }
    }
}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}

QString main_window::human_size(qint64 size) {
    return QString("%1 B").arg(size);
//    QLocale locale = this->locale();
//    return locale.formattedDataSize(size);
}

void main_window::show_status(QString const &txt) {
    ui->treeWidget->clear();
    QTreeWidgetItem *status_item = new QTreeWidgetItem(ui->treeWidget);
    status_item->setText(0, txt);
}

void main_window::exit() {
    scanner->wait();
    this->close();
}
