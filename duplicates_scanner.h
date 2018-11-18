//
// Created by Andrew Berlin on 2018-11-18.
//

#ifndef DFF_DUPLICATES_SCANNER_H
#define DFF_DUPLICATES_SCANNER_H

#include <QString>
#include <unordered_map>
#include <vector>

namespace std {
template<>
struct hash<QByteArray> {
    std::size_t operator()(const QByteArray &s) const {
        return qHash(s);
    }
};
}

struct duplicates_scanner {
    std::unordered_map<QByteArray, std::vector<QString>> get_duplicates(QString const &dir);
private:
    std::vector<QString> get_candidates(const QString &root);
};

#endif //DFF_DUPLICATES_SCANNER_H
