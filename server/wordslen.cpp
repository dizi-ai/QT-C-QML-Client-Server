#include "wordslen.h"

WordsLen::WordsLen(){}
QVariantMap WordsLen::countLens(const QString& msg){
    QMap<int, int> len;
    QStringList words = msg.split(" ");
    for(int i = 0; i < words.length(); i++){
        if(len.contains(words[i].length())){
            len[words[i].length()]++;
        }
        else{
            len[words[i].length()] = 1;
        }
    }
    QVariantMap toret;
    QMapIterator<int, int> i(len);
    while (i.hasNext()) {
        i.next();
        toret.insert(QString::number(i.key()), QString::number(i.value()));
    }
    return toret;
}
