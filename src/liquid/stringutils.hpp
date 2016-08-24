#ifndef LIQUID_STRINGUTILS_HPP
#define LIQUID_STRINGUTILS_HPP

#include <QStringRef>

namespace Liquid {

    QStringRef rtrim(const QStringRef& input);
    QStringRef ltrim(const QStringRef& input);
    QString doubleToString(double value, int precision = 6);

}

#endif
