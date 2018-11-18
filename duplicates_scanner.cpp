//
// Created by Andrew Berlin on 2018-11-18.
//

#include <QFileInfo>
#include <QDirIterator>
#include <QDir>
#include "duplicates_scanner.h"
#include "file_utils.h"

std::unordered_map<QByteArray, std::vector<QString>> duplicates_scanner::get_duplicates(QString const &dir) {
    auto candidates = get_candidates(dir);
    std::unordered_map<QByteArray, std::vector<QString>> duplicates;
    for (auto &path : candidates) {
        auto h = file_checksum(path);
//        while (!duplicates[h].empty() && !is_equal_files(duplicates[h].back(), path)) {
//            qDebug() << "NON EQUAL FILES, EQUAL HASHES " << path << ' ' << h << "\n";
//            h = next_hash(h);
//        }
        duplicates[h].emplace_back(path);
    }
    return duplicates;
}

std::vector<QString> duplicates_scanner::get_candidates(const QString &root) {
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
