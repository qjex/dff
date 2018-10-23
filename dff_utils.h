//
// Created by Andrew Berlin on 10/22/18.
//

#ifndef DFF_DFF_UTILS_H
#define DFF_DFF_UTILS_H

#include <QString>

const int READ_BUFF_SIZE = 8192;

QByteArray file_checksum(const QString &fileName);
QByteArray next_hash(const QByteArray &h);
bool is_equal_files(QString path1, QString path2);

#endif //DFF_DFF_UTILS_H
