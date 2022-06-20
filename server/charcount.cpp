#include "charcount.h"
#include <QDebug>

CharCount::CharCount(){}
QVariantMap CharCount::countChars(const QString& msg){
    QMap<QString, int> count;
    for(int i = 0; i < msg.length(); i++){
        qDebug() << "|" << msg[i] << "|";
        if(msg[i] != '\r'){
            if(count.contains(msg[i])){
                count[msg[i]]++;
            } else {
                count[msg[i]] = 1;
            }
        }
    }
    QVariantMap toret;
    QMapIterator<QString, int> i(count);
    while (i.hasNext()) {
        i.next();
        QString temp = i.key();
        if(temp == '\n'){
            temp = "Новая строка";
        } else if(temp == ' '){
            temp = "Пробел";
        }
        toret.insert(temp, QString::number(i.value()));
    }
    return toret;
}
