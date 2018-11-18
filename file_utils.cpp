//
// Created by Andrew Berlin on 10/22/18.
//

#include <QFile>
#include <QCryptographicHash>
#include "file_utils.h"

QByteArray file_checksum(const QString &fileName) {
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(&f);
        return hash.result();
    }
    return QByteArray();
}

QByteArray next_hash(const QByteArray &h) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(h);
    return hash.result().toHex();
}

bool is_equal_files(QString path1, QString path2) {
    QFile f1(path1);
    QFile f2(path2);
    if (f1.open(QFile::ReadOnly) && f2.open(QFile::ReadOnly)) {
        while (!f1.atEnd() && !f1.atEnd()) {
            if (f1.read(READ_BUFF_SIZE) != f2.read(READ_BUFF_SIZE)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
