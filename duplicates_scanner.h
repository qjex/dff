//
// Created by Andrew Berlin on 2018-11-18.
//

#ifndef DFF_DUPLICATES_SCANNER_H
#define DFF_DUPLICATES_SCANNER_H

#include <QString>
#include <QThread>
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

struct duplicates_scanner : public QThread {
Q_OBJECT
public:
    explicit duplicates_scanner(QString const &root);
    ~duplicates_scanner();
protected:
    virtual void run();
signals:
    void send_duplicates(std::unordered_map<QByteArray, std::vector<QString>> const &);
    void send_status(QString const &);
private:
    std::unordered_map<QByteArray, std::vector<QString>> get_duplicates(QString const &dir);
    std::vector<QString> get_candidates(const QString &root);
private:
    QString root;
};

#endif //DFF_DUPLICATES_SCANNER_H
