#include "standardfilters.hpp"
#include "stringscanner.hpp"
#include "stringutils.hpp"
#include <QDebug>

namespace Liquid { namespace StandardFilters {

Data append(const Data& input, const std::vector<Data>& args)
{
    Data value = input;
    for (const auto& arg : args) {
        value = value.toString() + arg.toString();
    }
    return value;
}

Data prepend(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("prepend only takes one argument, but was passed %1.").arg(args.size()).toStdString();
    }
    return args[0].toString() + input.toString();
}

Data downcase(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("downcase doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().toLower();
}

Data upcase(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("upcase doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().toUpper();
}

Data capitalize(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("capitalize doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString str = input.toString();
    return str.left(1).toUpper() + str.mid(1);
}

Data strip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("strip doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().trimmed();
}

Data rstrip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("rstrip doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString str = input.toString();
    return rtrim(&str).toString();
}

Data lstrip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("lstrip doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString str = input.toString();
    return ltrim(&str).toString();
}

Data strip_newlines(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("strip_newlines doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().replace("\n", "").replace("\r", "");
}

Data newline_to_br(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("newline_to_br doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().replace("\n", "<br />\n");
}

Data escape(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("escape doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().toHtmlEscaped().replace("'", "&#39;");
}

int scanEntity(StringScanner& ss)
{
    const int startPos = ss.position();
    const auto isNameChar = [](const ushort ch) {
        return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    };
    const auto isDecimalChar = [](const ushort ch) {
        return (ch >= '0' && ch <= '9');
    };
    const auto isHexChar = [](const ushort ch) {
        return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    };
    std::function<bool(const ushort)> func;
    bool firstch = true;
    while (!ss.eof()) {
        const ushort ch = ss.getch().at(0).unicode();
        if (firstch) {
            firstch = false;
            if (isNameChar(ch)) {
                func = isNameChar;
            } else if (ch == '#') {
                // Decimal or Hexadecimal
                const QStringRef peek = ss.peekch();
                if (peek.isNull()) {
                    return 0;
                }
                const ushort nextch = peek.at(0).unicode();
                if (nextch == 'x') {
                    const QStringRef firsthexch = ss.peekch(1);
                    if (firsthexch.isNull() || !isHexChar(firsthexch.at(0).unicode())) {
                        return 0;
                    }
                    func = isHexChar;
                    ss.setPosition(ss.position() + 2);
                } else if (isDecimalChar(nextch)) {
                    func = isDecimalChar;
                }
            }
            if (!func) {
                return 0;
            }
        } else if (ch == ';') {
            break;
        } else if (!func(ch)) {
            return 0;
        }
    }
    return ss.position() - startPos;
}

Data escape_once(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("escape_once doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString str = input.toString();
    StringScanner ss(&str);
    QString result;
    while (!ss.eof()) {
        const QChar ch = ss.getch().at(0);
        switch (ch.unicode()) {
            case '"':
                result += "&quot;";
                break;
            case '\'':
                result += "&#39;";
                break;
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '&': {
                const int currentPosition = ss.position();
                const int numEntityChars = scanEntity(ss);
                if (numEntityChars == 0) {
                    result += "&amp;";
                    ss.setPosition(currentPosition);
                } else {
                    QString chunk = str.mid(currentPosition - 1, numEntityChars + 1);
                    result += chunk;
                    ss.setPosition(currentPosition + numEntityChars);
                }
                break;
            }
            default:
                result += ch;
                break;
        }
    }
    return result;
}

Data url_encode(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("url_encode doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    QString result;
    const QString inputStr = input.toString();
    const int inputStrSize = inputStr.size();
    for (int i = 0; i < inputStrSize; ++i) {
        const ushort ch = inputStr.at(i).unicode();
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
            result += QChar(ch);
        } else {
            result += '%' + QString::number(ch, 16).toUpper();
        }
    }
    return result;
}

Data url_decode(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("url_decode doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    QString result;
    const QString inputStr = input.toString();
    const int inputStrSize = inputStr.size();
    for (int i = 0; i < inputStrSize; ++i) {
        const ushort ch = inputStr.at(i).unicode();
        if (ch == '+') {
            result += ' ';
        } else if (ch == '%' && i < (inputStrSize - 2)) {
            bool ok;
            const ushort value = inputStr.mid(i + 1, 2).toUShort(&ok, 16);
            if (ok) {
                result += QChar(value);
                i += 2;
            }
        } else {
            result += QChar(ch);
        }
    }
    return result;
}

Data strip_html(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("strip_html doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString inputStr = input.toString();
    const int inputStrSize = inputStr.size();
    QString output;
    bool html = false;
    for (int i = 0; i < inputStrSize; ++i) {
        const QChar ch = inputStr[i];
        if (ch == '<') {
            html = true;
        } else if (ch == '>') {
            html = false;
        } else if (!html) {
            output += ch;
        }
    }
    return output;
}

Data truncate(const Data& input, const std::vector<Data>& args)
{
    if (args.size() < 1 || args.size() > 2) {
        throw QString("truncate takes 1 or 2 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const int length = args[0].toInt();
    const QString truncateStr = args.size() == 2 ? args[1].toString() : "...";
    int len = length - truncateStr.size();
    if (len < 0) {
        len = 0;
    }
    return input.toString().left(len) + truncateStr;
}

Data truncatewords(const Data& input, const std::vector<Data>& args)
{
    if (args.size() < 1 || args.size() > 2) {
        throw QString("truncatewords takes 1 or 2 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const int numWords = args[0].toInt();
    const QString truncateStr = args.size() == 2 ? args[1].toString() : "...";
    const QChar delimiter = ' ';
    const QStringList wordlist = input.toString().split(delimiter);
    int len = numWords - 1;
    if (len < 0) {
        len = 0;
    }
    if (wordlist.size() <= len) {
        return input;
    }
    QString result;
    for (int i = 0; i <= len; ++i) {
        result += wordlist[i] + delimiter;
    }
    return result.trimmed() + truncateStr;
}

Data plus(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("plus takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() + arg.toInt();
    }
    return input.toFloat() + arg.toFloat();
}

Data minus(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("minus takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() - arg.toInt();
    }
    return input.toFloat() - arg.toFloat();
}

Data times(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("times takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() * arg.toInt();
    }
    return input.toFloat() * arg.toFloat();
}

Data divided_by(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("divided_by takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() / arg.toInt();
    }
    return input.toFloat() / arg.toFloat();
}

Data abs(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("abs takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return ::abs(input.toInt());
    }
    return ::fabs(input.toFloat());
}

Data ceil(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("ceil takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    return ::ceil(input.toFloat());
}

Data floor(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("floor takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    return ::floor(input.toFloat());
}

Data round(const Data& input, const std::vector<Data>& args)
{
    if (args.size() > 1) {
        throw QString("round takes 0 or 1 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    if (args.size() == 1) {
        const Data& arg = args[0];
        return doubleToString(input.toFloat(), arg.toInt());
    }
    return ::round(input.toFloat());
}

Data modulo(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("modulo takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() % arg.toInt();
    }
    return ::fmod(input.toFloat(), arg.toFloat());
}

Data split(const Data& input, const std::vector<Data>& args)
{
    if (args.size() > 1) {
        throw QString("split takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    Data array(Data::Type::Array);
    const QStringList items = input.toString().split(args[0].toString());
    for (const auto& item : items) {
        array.push_back(item);
    }
    return array;
}

Data join(const Data& input, const std::vector<Data>& args)
{
    if (args.size() > 1) {
        throw QString("join takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    QStringList inputList;
    const int inputSize = input.size();
    for (int i = 0; i < inputSize; ++i) {
        inputList << input.at(i).toString();
    }
    return inputList.join(args[0].toString());
}

Data uniq(const Data& input, const std::vector<Data>& args)
{
    if (args.size() > 1) {
        throw QString("uniq takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    Data result(Data::Type::Array);
    std::vector<Data> dupes;
    const int inputSize = input.size();
    for (int i = 0; i < inputSize; ++i) {
        const Data& item = input.at(i);
        if (std::find(dupes.begin(), dupes.end(), item) == dupes.end()) {
            dupes.push_back(item);
            result.push_back(item);
        }
    }
    return result;
}

size_t size_imp(const Data& input)
{
    return input.size();
}

Data size(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("size doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return static_cast<int>(size_imp(input));
}

const Data& first_imp(const Data& input)
{
    if (input.isArray() && input.size() > 0) {
        return input.at(0);
    }
    return kNilData;
}

Data first(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("first doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return first_imp(input);
}

const Data& last_imp(const Data& input)
{
    if (input.isArray() && input.size() > 0) {
        return input.at(input.size() - 1);
    }
    return kNilData;
}

Data last(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("last doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return last_imp(input);
}

Data def(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("default takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const bool useArg = input.isNil() ||
        (input.isArray() && input.size() == 0) ||
        (input.isHash() && input.size() == 0) ||
        (input.isString() && input.size() == 0);
    if (!useArg) {
        return input;
    }
    return args[0];
}

Data replace(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 2) {
        throw QString("replace takes 2 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().replace(args[0].toString(), args[1].toString());
}

Data replace_first(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 2) {
        throw QString("replace_first takes 2 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    QString inputStr = input.toString();
    const QString search = args[0].toString();
    const QString replace = args[1].toString();
    const int index = inputStr.indexOf(search);
    if (index != -1) {
        inputStr.replace(index, search.size(), replace);
    }
    return inputStr;
}

Data remove(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("remove takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().replace(args[0].toString(), "");
}

Data remove_first(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("remove_first takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    QString inputStr = input.toString();
    const QString search = args[0].toString();
    const int index = inputStr.indexOf(search);
    if (index != -1) {
        inputStr.replace(index, search.size(), "");
    }
    return inputStr;
}

Data slice(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1 && args.size() != 2) {
        throw QString("slice takes 1 or 2 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const int offset = args[0].toInt();
    const int length = args.size() == 2 ? args[1].toInt() : 1;
    const QString inputStr = input.toString();
    if (offset < 0) {
        return inputStr.mid(inputStr.size() - ::abs(offset), length);
    }
    return inputStr.mid(offset, length);
}

Data reverse(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("reverse takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    Data result(Data::Type::Array);
    const int size = input.size();
    for (int i = size - 1; i >= 0; --i) {
        result.push_back(input.at(i));
    }
    return result;
}

Data compact(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("compact takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    Data result(Data::Type::Array);
    const int size = input.size();
    for (int i = 0; i < size; ++i) {
        const Data& item = input.at(i);
        if (!item.isNil()) {
            result.push_back(item);
        }
    }
    return result;
}

Data map(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("map takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& property = args[0];
    Data result(Data::Type::Array);
    const int size = input.size();
    for (int i = 0; i < size; ++i) {
        result.push_back(input.at(i)[property.toString()]);
    }
    return result;
}

Data concat(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("concat takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (!arg.isArray()) {
        throw QString("concat requires an array argument").toStdString();
    }
    Data result(Data::Type::Array);
    int size = input.size();
    for (int i = 0; i < size; ++i) {
        result.push_back(input.at(i));
    }
    size = arg.size();
    for (int i = 0; i < size; ++i) {
        result.push_back(arg.at(i));
    }
    return result;
}

Data sort_imp(const Data& input, const std::vector<Data>& args, const QString& filterName, Qt::CaseSensitivity cs)
{
    if (args.size() > 1) {
        throw QString("%1 takes 0 or 1 arguments, but was passed %1.").arg(filterName).arg(args.size()).toStdString();
    }
    Data::Array objs = input.array();
    if (args.empty()) {
        std::sort(objs.begin(), objs.end(), [cs](const Data& a, const Data& b) -> bool {
            const QString s1 = a.toString();
            const QString s2 = b.toString();
            return s1.compare(s2, cs) < 0;
        });
    } else {
        const QString property = args[0].toString();
        std::sort(objs.begin(), objs.end(), [&property, cs](const Data& a, const Data& b) -> bool {
            const QString s1 = a[property].toString();
            const QString s2 = b[property].toString();
            return s1.compare(s2, cs) < 0;
        });
    }
    return objs;
}

Data sort(const Data& input, const std::vector<Data>& args)
{
    return sort_imp(input, args, "sort", Qt::CaseSensitive);
}

Data sort_natural(const Data& input, const std::vector<Data>& args)
{
    return sort_imp(input, args, "sort_natural", Qt::CaseInsensitive);
}

bool string_to_date(const std::string& input, struct ::tm& tm)
{
    const char *cstr = input.c_str();
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    if (::sscanf(cstr, "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second) == 6) {
        // ok
    } else if (::sscanf(cstr, "%04d-%02d-%02d", &year, &month, &day) == 3) {
        // ok
    } else {
        return false;
    }
    ::memset(&tm, 0, sizeof(tm));
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    return true;
}

Data date(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("date takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args.at(0);
    if (!arg.isString() || arg.size() == 0) {
        return input;
    }
    struct ::tm tm;
    if (!string_to_date(input.toString().toUtf8().constData(), tm)) {
        return nullptr;
    }
    const QByteArray formatBytes = arg.toString().toUtf8();
    const char *formatCstr = formatBytes.constData();
    size_t bufSize = 100;
    std::vector<char> buf(bufSize + 1, 0);
    size_t numChars;
    int infiniteLoopProtector = 0; // strftime() can return a valid empty string in some cases
    while ((numChars = ::strftime(buf.data(), bufSize, formatCstr, &tm)) == 0 && (infiniteLoopProtector < 100)) {
        bufSize *= 2;
        buf.resize(bufSize + 1, 0);
        ++infiniteLoopProtector;
    }
    return QString::fromUtf8(buf.data(), numChars);
}

void registerFilters(Template& tmpl)
{
    tmpl.registerFilter("append", append);
    tmpl.registerFilter("prepend", prepend);
    tmpl.registerFilter("downcase", downcase);
    tmpl.registerFilter("upcase", upcase);
    tmpl.registerFilter("capitalize", capitalize);
    tmpl.registerFilter("strip", strip);
    tmpl.registerFilter("rstrip", rstrip);
    tmpl.registerFilter("lstrip", lstrip);
    tmpl.registerFilter("strip_newlines", strip_newlines);
    tmpl.registerFilter("newline_to_br", newline_to_br);
    tmpl.registerFilter("escape", escape);
    tmpl.registerFilter("escape_once", escape_once);
    tmpl.registerFilter("url_encode", url_encode);
    tmpl.registerFilter("url_decode", url_decode);
    tmpl.registerFilter("strip_html", strip_html);
    tmpl.registerFilter("truncate", truncate);
    tmpl.registerFilter("truncatewords", truncatewords);
    tmpl.registerFilter("plus", plus);
    tmpl.registerFilter("minus", minus);
    tmpl.registerFilter("times", times);
    tmpl.registerFilter("divided_by", divided_by);
    tmpl.registerFilter("abs", abs);
    tmpl.registerFilter("ceil", ceil);
    tmpl.registerFilter("floor", floor);
    tmpl.registerFilter("round", round);
    tmpl.registerFilter("modulo", modulo);
    tmpl.registerFilter("split", split);
    tmpl.registerFilter("join", join);
    tmpl.registerFilter("uniq", uniq);
    tmpl.registerFilter("size", size);
    tmpl.registerFilter("first", first);
    tmpl.registerFilter("last", last);
    tmpl.registerFilter("default", def);
    tmpl.registerFilter("replace", replace);
    tmpl.registerFilter("replace_first", replace_first);
    tmpl.registerFilter("remove", remove);
    tmpl.registerFilter("remove_first", remove_first);
    tmpl.registerFilter("slice", slice);
    tmpl.registerFilter("reverse", reverse);
    tmpl.registerFilter("compact", compact);
    tmpl.registerFilter("map", map);
    tmpl.registerFilter("concat", concat);
    tmpl.registerFilter("sort", sort);
    tmpl.registerFilter("sort_natural", sort_natural);
    tmpl.registerFilter("date", date);
}

} } // namespace



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::StandardFilters") {
    
    SECTION("AppendToString") {
        Liquid::Template t;
        t.parse("{{ \"hello \" | append: \"world\" }}");
        CHECK(t.render().toStdString() == "hello world");
        t.parse("{{ 32 | append: \"world\" }}");
        CHECK(t.render().toStdString() == "32world");
        t.parse("{{ 32.94 | append: \"world\" }}");
        CHECK(t.render().toStdString() == "32.94world");
    }

    SECTION("Prepend") {
        Liquid::Template t;
        t.parse("{{ \" world\" | prepend: \"hello\" }}");
        CHECK(t.render().toStdString() == "hello world");
    }

    SECTION("Downcase") {
        Liquid::Template t;
        t.parse("{{ \"HELLO\" | downcase }}");
        CHECK(t.render().toStdString() == "hello");
    }

    SECTION("Upcase") {
        Liquid::Template t;
        t.parse("{{ \"hello\" | upcase }}");
        CHECK(t.render().toStdString() == "HELLO");
    }

    SECTION("Capitalize") {
        Liquid::Template t;
        t.parse("{{ \"hello world\" | capitalize }}");
        CHECK(t.render().toStdString() == "Hello world");
    }

    SECTION("Strip") {
        Liquid::Template t;
        t.parse("{{ \" \r\n\thello\t\n\r \" | strip }}");
        CHECK(t.render().toStdString() == "hello");
    }

    SECTION("Rstrip") {
        Liquid::Template t;
        t.parse("{{ \" \r\n\thello\t\n\r \" | rstrip }}");
        CHECK(t.render().toStdString() == " \r\n\thello");
    }

    SECTION("Lstrip") {
        Liquid::Template t;
        t.parse("{{ \" \r\n\thello\t\n\r \" | lstrip }}");
        CHECK(t.render().toStdString() == "hello\t\n\r ");
    }

    SECTION("StripNewLines") {
        Liquid::Template t;
        t.parse("{{ \"\r\nhe\nll\ro\r\" | strip_newlines }}");
        CHECK(t.render().toStdString() == "hello");
    }

    SECTION("NewlineToBr") {
        Liquid::Template t;
        t.parse("{{ \"hello\nand\ngoodbye\n\" | newline_to_br }}");
        CHECK(t.render().toStdString() == "hello<br />\nand<br />\ngoodbye<br />\n");
    }

    SECTION("Escape") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["what"] = "' \" & < > ' \" & < >";
        t.parse("{{ what | escape }}");
        CHECK(t.render(hash).toStdString() == "&#39; &quot; &amp; &lt; &gt; &#39; &quot; &amp; &lt; &gt;");
    }

    SECTION("EscapeOnce") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["what"] = "' \" & < > ' \" & < >";
        CHECK(t.parse("{{ what | escape_once }}").render(hash).toStdString() == "&#39; &quot; &amp; &lt; &gt; &#39; &quot; &amp; &lt; &gt;");
        CHECK(t.parse("{{ '1 < 2 & 3' | escape_once }}").render(hash).toStdString() == "1 &lt; 2 &amp; 3");
        CHECK(t.parse("{{ '1 &lt; 2 &amp; 3' | escape_once }}").render(hash).toStdString() == "1 &lt; 2 &amp; 3");
        CHECK(t.parse("{{ '&excl; &#x00021; &#33;' | escape_once }}").render(hash).toStdString() == "&excl; &#x00021; &#33;");
        CHECK(t.parse("{{ '&frac34; &#x000BE; &#190;' | escape_once }}").render(hash).toStdString() == "&frac34; &#x000BE; &#190;");
        CHECK(t.parse("{{ '&ctdot; &#x022EF; &#8943;' | escape_once }}").render(hash).toStdString() == "&ctdot; &#x022EF; &#8943;");
        CHECK(t.parse("{{ '&; &#x; &#;' | escape_once }}").render(hash).toStdString() == "&amp;; &amp;#x; &amp;#;");
        CHECK(t.parse("{{ '&#x0; &#0;' | escape_once }}").render(hash).toStdString() == "&#x0; &#0;");
    }

    SECTION("UrlEncode") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'hello @world' | url_encode }}").render().toStdString() == "hello%20%40world");
        CHECK(t.parse("{{ 'foo+1@example.com' | url_encode }}").render().toStdString() == "foo%2B1%40example.com");
    }
    
    SECTION("UrlDecode") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'hello%20%40world' | url_decode }}").render().toStdString() == "hello @world");
        CHECK(t.parse("{{ 'foo+bar' | url_decode }}").render().toStdString() == "foo bar");
        CHECK(t.parse("{{ 'foo%20bar' | url_decode }}").render().toStdString() == "foo bar");
        CHECK(t.parse("{{ 'foo%2B1%40example.com' | url_decode }}").render().toStdString() == "foo+1@example.com");
        CHECK(t.parse("{{ 'foo%2b1%40example.com' | url_decode }}").render().toStdString() == "foo+1@example.com");
        CHECK(Liquid::StandardFilters::url_decode("%20", {}) == " ");
        CHECK(Liquid::StandardFilters::url_decode("%2", {}) == "%2");
        CHECK(Liquid::StandardFilters::url_decode("%", {}) == "%");
    }

    SECTION("StripHtml") {
        Liquid::Template t;
        t.parse("{{ \"<p>hello <b>w<span>or</span>ld</b></p>\" | strip_html }}");
        CHECK(t.render().toStdString() == "hello world");
    }

    SECTION("Truncate") {
        Liquid::Template t;
        t.parse("{{ 'Ground control to Major Tom.' | truncatewords: 3 }}");
        CHECK(t.render().toStdString() == "Ground control to...");
        t.parse("{{ 'Ground control to Major Tom.' | truncatewords: 3, '--' }}");
        CHECK(t.render().toStdString() == "Ground control to--");
        t.parse("{{ 'Ground control to Major Tom.' | truncatewords: 3, '' }}");
        CHECK(t.render().toStdString() == "Ground control to");
    }

    SECTION("TruncateWords") {
        Liquid::Template t;
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 20 }}");
        CHECK(t.render().toStdString() == "Ground control to...");
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 25, \", and so on\" }}");
        CHECK(t.render().toStdString() == "Ground control, and so on");
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 20, \"\" }}");
        CHECK(t.render().toStdString() == "Ground control to Ma");
    }

    SECTION("Plus") {
        Liquid::Template t;
        t.parse("{{ 4 | plus: 2 }}");
        CHECK(t.render().toStdString() == "6");
        t.parse("{{ 16 | plus: 4 }}");
        CHECK(t.render().toStdString() == "20");
        t.parse("{{ 183.357 | plus: 12 }}");
        CHECK(t.render().toStdString() == "195.357");
    }

    SECTION("Minus") {
        Liquid::Template t;
        t.parse("{{ 4 | minus: 2 }}");
        CHECK(t.render().toStdString() == "2");
        t.parse("{{ 16 | minus: 4 }}");
        CHECK(t.render().toStdString() == "12");
        t.parse("{{ 183.357 | minus: 12 }}");
        CHECK(t.render().toStdString() == "171.357");
    }

    SECTION("Times") {
        Liquid::Template t;
        t.parse("{{ 4 | times: 2 }}");
        CHECK(t.render().toStdString() == "8");
        t.parse("{{ 16 | times: 4 }}");
        CHECK(t.render().toStdString() == "64");
        t.parse("{{ 183.357 | times: 12 }}");
        CHECK(t.render().toStdString() == "2200.284");
    }

    SECTION("DividedBy") {
        Liquid::Template t;
        t.parse("{{ 16 | divided_by: 4 }}");
        CHECK(t.render().toStdString() == "4");
        t.parse("{{ 5 | divided_by: 3 }}");
        CHECK(t.render().toStdString() == "1");
        t.parse("{{ 20 | divided_by: 7 }}");
        CHECK(t.render().toStdString() == "2");
        t.parse("{{ 20 | divided_by: 7.0 }}");
        CHECK(t.render().toStdString() == "2.857143");
    }

    SECTION("Abs") {
        Liquid::Template t;
        CHECK(t.parse("{{ -17 | abs }}").render().toStdString() == "17");
        CHECK(t.parse("{{ 4 | abs }}").render().toStdString() == "4");
        CHECK(t.parse("{{ -19.86 | abs }}").render().toStdString() == "19.86");
    }

    SECTION("Ceil") {
        Liquid::Template t;
        CHECK(t.parse("{{ 1.2 | ceil }}").render().toStdString() == "2");
        CHECK(t.parse("{{ 2.0 | ceil }}").render().toStdString() == "2");
        CHECK(t.parse("{{ 183.357 | ceil }}").render().toStdString() == "184");
    }

    SECTION("Floor") {
        Liquid::Template t;
        CHECK(t.parse("{{ 1.2 | floor }}").render().toStdString() == "1");
        CHECK(t.parse("{{ 2.0 | floor }}").render().toStdString() == "2");
        CHECK(t.parse("{{ 183.357 | floor }}").render().toStdString() == "183");
    }

    SECTION("Round") {
        Liquid::Template t;
        CHECK(t.parse("{{ 1.2 | round }}").render().toStdString() == "1");
        CHECK(t.parse("{{ 2.7 | round }}").render().toStdString() == "3");
        CHECK(t.parse("{{ 183.357 | round: 1}}").render().toStdString() == "183.4");
        CHECK(t.parse("{{ 183.357 | round: 2}}").render().toStdString() == "183.36");
    }

    SECTION("Modulo") {
        Liquid::Template t;
        CHECK(t.parse("{{ 3 | modulo: 2 }}").render().toStdString() == "1");
        CHECK(t.parse("{{ 24 | modulo: 7 }}").render().toStdString() == "3");
        CHECK(t.parse("{{ 183.357 | modulo: 12 }}").render().toStdString() == "3.357");
    }

    SECTION("Split") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'John, Paul, George, Ringo' | split: ', ' | size }}").render().toStdString() == "4");
    }

    SECTION("Join") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'John, Paul, George, Ringo' | split: ', ' | join: '.' }}").render().toStdString() == "John.Paul.George.Ringo");
    }

    SECTION("Unique") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'ants, bugs, bees, bugs, ants' | split: ', ' | uniq | join: '.' }}").render().toStdString() == "ants.bugs.bees");
    }

    SECTION("Size") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'Ground control to Major Tom.' | size }}").render().toStdString() == "28");
        Liquid::Data::Hash hash;
        hash["what"] = "Ground control to Major Tom.";
        CHECK(t.parse("{{ what.size }}").render(hash).toStdString() == "28");
    }

    SECTION("First") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'John, Paul, George, Ringo' | split: ', ' | first }}").render().toStdString() == "John");
        Liquid::Data::Hash hash;
        Liquid::Data::Array names;
        names.push_back("John");
        names.push_back("Paul");
        names.push_back("George");
        names.push_back("Ringo");
        hash["names"] = names;
        CHECK(t.parse("{{ names.first }}").render(hash).toStdString() == "John");
    }

    SECTION("Last") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'John, Paul, George, Ringo' | split: ', ' | last }}").render().toStdString() == "Ringo");
        Liquid::Data::Hash hash;
        Liquid::Data::Array names;
        names.push_back("John");
        names.push_back("Paul");
        names.push_back("George");
        names.push_back("Ringo");
        hash["names"] = names;
        CHECK(t.parse("{{ names.last }}").render(hash).toStdString() == "Ringo");
    }

    SECTION("Default") {
        Liquid::Template t;
        CHECK(t.parse("{{ nil | default: 'test' }}").render().toStdString() == "test");
        CHECK(t.parse("{{ product_price | default: 2.99 }}").render().toStdString() == "2.99");
        Liquid::Data::Hash hash;
        hash["product_price"] = 4.99;
        CHECK(t.parse("{{ product_price | default: 2.99 }}").render(hash).toStdString() == "4.99");
        hash["product_price"] = "";
        CHECK(t.parse("{{ product_price | default: 2.99 }}").render(hash).toStdString() == "2.99");
    }

    SECTION("Replace") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'Take my protein pills and put my helmet on' | replace: 'my', 'your' }}").render().toStdString() == "Take your protein pills and put your helmet on");
    }

    SECTION("ReplaceFirst") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'Take my protein pills and put my helmet on' | replace_first: 'my', 'your' }}").render().toStdString() == "Take your protein pills and put my helmet on");
    }

    SECTION("Remove") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'I strained to see the train through the rain' | remove: 'rain' }}").render().toStdString() == "I sted to see the t through the ");
    }

    SECTION("RemoveFirst") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'I strained to see the train through the rain' | remove_first: 'rain' }}").render().toStdString() == "I sted to see the train through the rain");
    }

    SECTION("Slice") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'Liquid' | slice: 0 }}").render().toStdString() == "L");
        CHECK(t.parse("{{ 'Liquid' | slice: 2 }}").render().toStdString() == "q");
        CHECK(t.parse("{{ 'Liquid' | slice: -3, 2 }}").render().toStdString() == "ui");
    }

    SECTION("Reverse") {
        Liquid::Template t;
        CHECK(t.parse("{{ 'apples,oranges,peaches' | split: ',' | reverse | join: ',' }}").render().toStdString() == "peaches,oranges,apples");
    }

    SECTION("Compact") {
        Liquid::Template t;
        Liquid::Data::Array array;
        array.push_back(nullptr);
        array.push_back("hello");
        array.push_back(nullptr);
        array.push_back(nullptr);
        array.push_back("world");
        array.push_back(nullptr);
        Liquid::Data::Hash hash;
        hash["array"] = array;
        CHECK(t.parse("{{ array | size }}").render(hash).toStdString() == "6");
        CHECK(t.parse("{{ array | compact | size }}").render(hash).toStdString() == "2");
        CHECK(t.parse("{{ array | compact | join: ' ' }}").render(hash).toStdString() == "hello world");
    }

    SECTION("Map") {
        Liquid::Template t;
        Liquid::Data::Array pages;
        Liquid::Data::Hash item;
        item["category"] = "business";
        pages.push_back(item);
        item["category"] = "celebrities";
        pages.push_back(item);
        item["category"] = "lifestyle";
        pages.push_back(item);
        item["category"] = "sports";
        pages.push_back(item);
        item["category"] = "technology";
        pages.push_back(item);
        Liquid::Data::Hash hash;
        hash["pages"] = pages;
        CHECK(t.parse("{{ pages | size }}").render(hash).toStdString() == "5");
        CHECK(t.parse("{{ pages | map: 'category' | size }}").render(hash).toStdString() == "5");
        CHECK(t.parse("{{ pages | map: 'category' | join: ' ' }}").render(hash).toStdString() == "business celebrities lifestyle sports technology");
    }

    SECTION("Concat") {
        Liquid::Template t;
        Liquid::Data::Array names1;
        Liquid::Data::Array names2;
        Liquid::Data::Hash hash;
        names1.push_back("bill");
        names1.push_back("steve");
        names2.push_back("larry");
        names2.push_back("michael");
        hash["names1"] = names1;
        hash["names2"] = names2;
        CHECK(t.parse("{{ names1 | concat: names2 | join: ',' }}").render(hash).toStdString() == "bill,steve,larry,michael");
    }

    SECTION("Sort") {
        Liquid::Template t;
        Liquid::Data::Array array;
        Liquid::Data::Hash hash;
        array.push_back("zebra");
        array.push_back("octopus");
        array.push_back("giraffe");
        array.push_back("Sally Snake");
        hash["array"] = array;
        CHECK(t.parse("{{ array | sort | join: ', ' }}").render(hash).toStdString() == "Sally Snake, giraffe, octopus, zebra");
        Liquid::Data::Hash item;
        Liquid::Data::Array names;
        item["name"] = "bob";
        names.push_back(item);
        item["name"] = "Sally";
        names.push_back(item);
        item["name"] = "george";
        names.push_back(item);
        item["name"] = "Jane";
        names.push_back(item);
        hash["names"] = names;
        CHECK(t.parse("{{ names | sort: 'name' | map: 'name' | join: ', ' }}").render(hash).toStdString() == "Jane, Sally, bob, george");
    }

    SECTION("SortNatural") {
        Liquid::Template t;
        Liquid::Data::Array array;
        Liquid::Data::Hash hash;
        array.push_back("zebra");
        array.push_back("octopus");
        array.push_back("giraffe");
        array.push_back("Sally Snake");
        hash["array"] = array;
        CHECK(t.parse("{{ array | sort_natural | join: ', ' }}").render(hash).toStdString() == "giraffe, octopus, Sally Snake, zebra");
        Liquid::Data::Hash item;
        Liquid::Data::Array names;
        item["name"] = "bob";
        names.push_back(item);
        item["name"] = "Sally";
        names.push_back(item);
        item["name"] = "george";
        names.push_back(item);
        item["name"] = "Jane";
        names.push_back(item);
        hash["names"] = names;
        CHECK(t.parse("{{ names | sort_natural: 'name' | map: 'name' | join: ', ' }}").render(hash).toStdString() == "bob, george, Jane, Sally");
    }
    
    SECTION("Date") {
        using namespace Liquid::StandardFilters;
        CHECK(date("2006-05-05 10:00:00", {"%B"}).toString().toStdString() == "May");
        CHECK(date("2006-06-05 10:00:00", {"%B"}).toString().toStdString() == "June");
        CHECK(date("2006-07-05 10:00:00", {"%B"}).toString().toStdString() == "July");

        CHECK(date("2006-07-05 10:00:00", {""}).toString().toStdString() == "2006-07-05 10:00:00");
        CHECK(date("2006-07-05 10:00:00", {nullptr}).toString().toStdString() == "2006-07-05 10:00:00");

        CHECK(date("2006-07-05", {"%m/%d/%Y"}).toString().toStdString() == "07/05/2006");
        CHECK(date("2006-07-05 10:00:00", {"%m/%d/%Y"}).toString().toStdString() == "07/05/2006");
        CHECK(date("2006-07-05 10:32:11", {"%m/%d/%Y %k.%M.%S"}).toString().toStdString() == "07/05/2006 10.32.11");
    }
}

#endif
