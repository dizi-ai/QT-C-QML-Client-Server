#ifndef WORDSLEN_H
#define WORDSLEN_H
#include <QVariantMap>

class WordsLen
{
public:
    WordsLen();
    QVariantMap countLens(const QString& msg);
};

#endif // WORDSLEN_H
