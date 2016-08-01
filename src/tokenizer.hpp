#ifndef LIQUID_TOKENIZER_HPP
#define LIQUID_TOKENIZER_HPP

#include <QString>
#include <QStringList>
#include <vector>

namespace Liquid {
    
    class Tokenizer {
    public:
        Tokenizer(const QString& source);
        
        bool shift(QString& token);
        
        int line_number();
    
    private:
        QString source_;
        int line_number_;
        QStringList tokens_;
        QStringList tokenize();
        static QStringList split(const QString& source, const QRegularExpression& regex);
    };
    
}

#endif
