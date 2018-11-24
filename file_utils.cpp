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
