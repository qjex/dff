#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dff_utils.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QtWidgets>
#include <string>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
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
        separator_item->setText(1, QString("%1B per file").arg(sz));
    }
    setWindowTitle(QString("Duplicates in: %1 (%2 duplicates)").arg(dir).arg(total_cnt));
}

std::vector<QString> main_window::get_candidates(const QString &root) {
    std::unordered_map<qint64, std::vector<QString>> files_by_size;
    QDirIterator it(root, QDir::Hidden | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo file_info(path);
        files_by_size[file_info.size()].emplace_back(std::move(path));
    }
    std::vector<QString> result;
    for (auto &entry : files_by_size) {
        if (entry.second.size() < 2) {
            continue;
        }
        for (auto &p : entry.second) {
            result.emplace_back(std::move(p));
        }
    }
    return result;
}

void main_window::scan_directory(QString const &dir) {
    auto candidates = get_candidates(dir);
    qDebug() << "Done cand\n";
    for (auto &path : candidates) {
        auto h = file_checksum(path);
//        while (!duplicates[h].empty() && !is_equal_files(duplicates[h].back(), path)) {
//            qDebug() << "NON EQUAL FILES, EQUAL HASHES " << path << ' ' << h << "\n";
//            h = next_hash(h);
//        }
        duplicates[h].emplace_back(path);
    }
}

int get_decreased(const QString &prev) {
    int pos = prev.indexOf(' ');
    QStringRef ref(&prev, 0, pos);
    return ref.toInt() - 1;
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
            int nxt = get_decreased(item->parent()->text(0));
            item->parent()->setText(0, QString("%1 duplicates").arg(nxt));
            item->parent()->removeChild(item);
        }
    }

}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}
