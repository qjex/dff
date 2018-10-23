#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QtWidgets>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
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
    duplicates.clear();
    setWindowTitle(QString("Duplicates in: %1").arg(dir));
    scan_directory(dir);
    show_duplicates(dir);
}

void main_window::show_duplicates(const QString &dir) {
    size_t total_cnt = 0;
    for (auto &h : duplicates) {
        if (h.second.size() < 2) {
            continue;
        }
        total_cnt += h.second.size();
        QTreeWidgetItem *separator_item = new QTreeWidgetItem(ui->treeWidget);
        separator_item->setExpanded(true);
        separator_item->setText(0, QString("%1 duplicates").arg(h.second.size()));
        for (auto &file_path : h.second) {
            QTreeWidgetItem *item = new QTreeWidgetItem(separator_item);
            QFileInfo file_info(file_path);
            item->setText(0, file_info.fileName());
            item->setText(1, file_path);
            item->setToolTip(1, file_path);
            item->setText(2, QString::number(file_info.size()));
            ui->treeWidget->addTopLevelItem(item);
        }
    }
    setWindowTitle(QString("Duplicates in: %1 (%2 duplicates)").arg(dir).arg(total_cnt));
}

void main_window::scan_directory(QString const &dir) {
    QDir d(dir);
    QDir pd = d;
    pd.cdUp();
    QString parent = pd.absolutePath();
    QFileInfoList list = d.entryInfoList();
    for (QFileInfo file_info : list) {
        if (file_info.isSymLink()) {
            continue;
        }
        QString path = file_info.absoluteFilePath();
        if (path == dir || path == parent) {
            continue;
        }
        if (file_info.isDir()) {
            scan_directory(path);
            continue;
        }
        auto h = file_checksum(path);
        while (!duplicates[h].empty() && !is_equal_files(duplicates[h].back(), path)) {
            qDebug() << "!!!!!" << path << h << "\n";
            h = next_hash(h);
        }
        duplicates[h].emplace_back(path);
    }
}

void main_window::delete_items() {
    QList<QTreeWidgetItem *> sel_items = ui->treeWidget->selectedItems();
    for (auto item : sel_items) {
        QString path = item->text(1);
        if (path.isEmpty()) {
            continue;
        }
        QFile file(path);
        if (file.remove()) {
            item->parent()->removeChild(item);
        }
    }

}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}
