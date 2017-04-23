#include "standardfilters.hpp"
#include "stringscanner.hpp"
#include "stringutils.hpp"
#include "template.hpp"
#include "error.hpp"
#include <cerrno>
#include <ctime>

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
        throw syntax_error(String("prepend only takes one argument, but was passed %1.").arg(args.size()));
    }
    return args[0].toString() + input.toString();
}

Data downcase(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("downcase doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    return input.toString().toLower();
}

Data upcase(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("upcase doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    return input.toString().toUpper();
}

Data capitalize(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("capitalize doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    const String str = input.toString();
    return str.left(1).toUpper() + str.mid(1);
}

Data strip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("strip doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    return trim(input.toString());
}

Data rstrip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("rstrip doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    const String str = input.toString();
    return rtrim(StringRef{&str}).toString();
}

Data lstrip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("lstrip doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    const String str = input.toString();
    return ltrim(StringRef{&str}).toString();
}

Data strip_newlines(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("strip_newlines doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    return input.toString().replace("\n", "").replace("\r", "");
}

Data newline_to_br(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("newline_to_br doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    return input.toString().replace("\n", "<br />\n");
}

Data escape(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("escape doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    const String inputStr = input.toString();
    String s;
    const auto sz = inputStr.size();
    for (String::size_type i = 0; i < sz; ++i) {
        const auto ch = inputStr.at(i);
        switch (ch) {
            case '&':
                s += "&amp;";
                break;
            case '<':
                s += "&lt;";
                break;
            case '>':
                s += "&gt;";
                break;
            case '\"':
                s += "&quot;";
                break;
            case '\'':
                s += "&#39;";
                break;
            default:
                s += ch;
                break;
        }
    }
    return s;
}

String::size_type scanEntity(StringScanner& ss)
{
    const auto startPos = ss.position();
    const auto isNameChar = [](const String::value_type ch) {
        return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    };
    const auto isDecimalChar = [](const String::value_type ch) {
        return (ch >= '0' && ch <= '9');
    };
    const auto isHexChar = [](const String::value_type ch) {
        return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    };
    std::function<bool(const String::value_type)> func;
    bool firstch = true;
    while (!ss.eof()) {
        const String::value_type ch = ss.getch().at(0);
        if (firstch) {
            firstch = false;
            if (isNameChar(ch)) {
                func = isNameChar;
            } else if (ch == '#') {
                // Decimal or Hexadecimal
                const StringRef peek = ss.peekch();
                if (peek.isEmpty()) {
                    return 0;
                }
                const auto nextch = peek.at(0);
                if (nextch == 'x' || nextch == 'X') {
                    const StringRef firsthexch = ss.peekch(1);
                    if (firsthexch.isEmpty() || !isHexChar(firsthexch.at(0))) {
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
        throw syntax_error(String("escape_once doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    const String str = input.toString();
    const StringRef ref{&str};
    StringScanner ss{ref};
    String result;
    while (!ss.eof()) {
        const StringRef chr = ss.getch();
        if (chr.isNull()) {
            break;
        }
        const String::value_type ch = chr.at(0);
        switch (ch) {
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
                const auto currentPosition = ss.position();
                const auto numEntityChars = scanEntity(ss);
                if (numEntityChars == 0) {
                    result += "&amp;";
                    ss.setPosition(currentPosition);
                } else {
                    const String chunk = str.mid(currentPosition - 1, numEntityChars + 1);
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
        throw syntax_error(String("url_encode doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    String result;
    const String inputStr = input.toString();
    const auto inputStrSize = inputStr.size();
    static const char hexchars[] = "0123456789ABCDEF";
    char hexbuf[4] = {0};
    hexbuf[0] = '%';
    for (String::size_type i = 0; i < inputStrSize; ++i) {
        const String::value_type ch = inputStr.at(i);
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
            result += ch;
        } else {
            hexbuf[1] = hexchars[(ch & 0xF0) >> 4];
            hexbuf[2] = hexchars[ch & 0x0F];
            result += String(hexbuf);
        }
    }
    return result;
}

Data url_decode(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("url_decode doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    String result;
    const String inputStr = input.toString();
    const auto inputStrSize = inputStr.size();
    for (String::size_type i = 0; i < inputStrSize; ++i) {
        const String::value_type ch = inputStr.at(i);
        if (ch == '+') {
            result += ' ';
        } else if (ch == '%' && inputStrSize >= 3 && i < (inputStrSize - 2)) {
            try {
                const String::value_type value = static_cast<String::value_type>(std::stoul(inputStr.mid(i + 1, 2).toStdString(), nullptr, 16));
                result += value;
                i += 2;
            } catch (...) {
                // ignore, probably invalid_argument
            }
        } else {
            result += ch;
        }
    }
    return result;
}

Data strip_html(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("strip_html doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    const String inputStr = input.toString();
    const auto inputStrSize = inputStr.size();
    String output;
    bool html = false;
    for (String::size_type i = 0; i < inputStrSize; ++i) {
        const String::value_type ch = inputStr.at(i);
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
        throw syntax_error(String("truncate takes 1 or 2 arguments, but was passed %1.").arg(args.size()));
    }
    const int length = args[0].toInt();
    const String truncateStr = args.size() == 2 ? args[1].toString() : "...";
    auto len = length - truncateStr.size();
    if (static_cast<int>(len) < 0) {
        len = 0;
    }
    return input.toString().left(len) + truncateStr;
}

Data truncatewords(const Data& input, const std::vector<Data>& args)
{
    if (args.size() < 1 || args.size() > 2) {
        throw syntax_error(String("truncatewords takes 1 or 2 arguments, but was passed %1.").arg(args.size()));
    }
    const int numWords = args[0].toInt();
    const String truncateStr = args.size() == 2 ? args[1].toString() : "...";
    const String::value_type delimiter = ' ';
    const auto wordlist = input.toString().split(delimiter);
    int len = numWords - 1;
    if (len < 0) {
        len = 0;
    }
    if (static_cast<int>(wordlist.size()) <= len) {
        return input;
    }
    String result;
    for (int i = 0; i <= len; ++i) {
        result += wordlist[i] + delimiter;
    }
    return trim(result) + truncateStr;
}

Data plus(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw syntax_error(String("plus takes 1 argument, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("minus takes 1 argument, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("times takes 1 argument, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("divided_by takes 1 argument, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("abs takes 0 arguments, but was passed %1.").arg(args.size()));
    }
    if (input.isNumberInt()) {
        return ::abs(input.toInt());
    }
    return ::fabs(input.toFloat());
}

Data ceil(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("ceil takes 0 arguments, but was passed %1.").arg(args.size()));
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    return ::ceil(input.toFloat());
}

Data floor(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("floor takes 0 arguments, but was passed %1.").arg(args.size()));
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    return ::floor(input.toFloat());
}

Data round(const Data& input, const std::vector<Data>& args)
{
    if (args.size() > 1) {
        throw syntax_error(String("round takes 0 or 1 arguments, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("modulo takes 1 argument, but was passed %1.").arg(args.size()));
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() % arg.toInt();
    }
    return ::fmod(input.toFloat(), arg.toFloat());
}

Data split(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw syntax_error(String("split takes 1 argument, but was passed %1.").arg(args.size()));
    }
    Data array(Data::Type::Array);
    const auto items = input.toString().split(args[0].toString());
    for (const auto& item : items) {
        array.push_back(item);
    }
    return array;
}

Data join(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw syntax_error(String("join takes 1 argument, but was passed %1.").arg(args.size()));
    }
    String result;
    const int inputSize = static_cast<int>(input.size());
    const auto joiner = args[0].toString();
    for (int i = 0; i < inputSize; ++i) {
        result += input.at(i).toString();
        if (i < (inputSize - 1)) {
            result += joiner;
        }
    }
    return result;
}

Data uniq(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("uniq takes 0 arguments, but was passed %1.").arg(args.size()));
    }
    Data result(Data::Type::Array);
    std::vector<Data> dupes;
    const int inputSize = static_cast<int>(input.size());
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
        throw syntax_error(String("size doesn't take any arguments, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("first doesn't take any arguments, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("last doesn't take any arguments, but was passed %1.").arg(args.size()));
    }
    return last_imp(input);
}

Data def(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw syntax_error(String("default takes 1 argument, but was passed %1.").arg(args.size()));
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
        throw syntax_error(String("replace takes 2 argument, but was passed %1.").arg(args.size()));
    }
    return input.toString().replace(args[0].toString(), args[1].toString());
}

Data replace_first(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 2) {
        throw syntax_error(String("replace_first takes 2 argument, but was passed %1.").arg(args.size()));
    }
    String inputStr = input.toString();
    const String search = args[0].toString();
    const String replace = args[1].toString();
    const String::size_type index = inputStr.indexOf(search);
    if (index != String::npos) {
        inputStr.replace(index, search.size(), replace);
    }
    return inputStr;
}

Data remove(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw syntax_error(String("remove takes 1 argument, but was passed %1.").arg(args.size()));
    }
    return input.toString().replace(args[0].toString(), "");
}

Data remove_first(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw syntax_error(String("remove_first takes 1 argument, but was passed %1.").arg(args.size()));
    }
    String inputStr = input.toString();
    const String search = args[0].toString();
    const auto index = inputStr.indexOf(search);
    if (index != static_cast<String::size_type>(-1)) {
        inputStr.replace(index, search.size(), "");
    }
    return inputStr;
}

Data slice(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1 && args.size() != 2) {
        throw syntax_error(String("slice takes 1 or 2 arguments, but was passed %1.").arg(args.size()));
    }
    const int offset = args[0].toInt();
    const int length = args.size() == 2 ? args[1].toInt() : 1;
    const String inputStr = input.toString();
    if (offset < 0) {
        return inputStr.mid(inputStr.size() - ::abs(offset), length);
    }
    return inputStr.mid(offset, length);
}

Data reverse(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("reverse takes 0 arguments, but was passed %1.").arg(args.size()));
    }
    Data result(Data::Type::Array);
    const int size = static_cast<int>(input.size());
    for (int i = size - 1; i >= 0; --i) {
        result.push_back(input.at(i));
    }
    return result;
}

Data compact(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw syntax_error(String("compact takes 0 arguments, but was passed %1.").arg(args.size()));
    }
    Data result(Data::Type::Array);
    const int size = static_cast<int>(input.size());
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
        throw syntax_error(String("map takes 1 argument, but was passed %1.").arg(args.size()));
    }
    const Data& property = args[0];
    Data result(Data::Type::Array);
    const int size = static_cast<int>(input.size());
    for (int i = 0; i < size; ++i) {
        result.push_back(input.at(i)[property.toString()]);
    }
    return result;
}

Data concat(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw syntax_error(String("concat takes 1 argument, but was passed %1.").arg(args.size()));
    }
    const Data& arg = args[0];
    if (!arg.isArray()) {
        throw syntax_error(String("concat requires an array argument"));
    }
    Data result(Data::Type::Array);
    int size = static_cast<int>(input.size());
    for (int i = 0; i < size; ++i) {
        result.push_back(input.at(i));
    }
    size = static_cast<int>(arg.size());
    for (int i = 0; i < size; ++i) {
        result.push_back(arg.at(i));
    }
    return result;
}

Data sort_imp(const Data& input, const std::vector<Data>& args, const String& filterName, bool caseSensitive)
{
    if (args.size() > 1) {
        throw syntax_error(String("%1 takes 0 or 1 arguments, but was passed %1.").arg(filterName).arg(args.size()));
    }
    Data::Array objs = input.array();
    if (args.empty()) {
        std::sort(objs.begin(), objs.end(), [caseSensitive](const Data& a, const Data& b) -> bool {
            const String s1 = a.toString();
            const String s2 = b.toString();
            return s1.compare(s2, caseSensitive) < 0;
        });
    } else {
        const String property = args[0].toString();
        std::stable_sort(objs.begin(), objs.end(), [&objs, &property, caseSensitive](const Data& a, const Data& b) -> bool {
            const Data& obj1{a[property]};
            const Data& obj2{b[property]};
            if (!obj1.isNil() && obj2.isNil()) {
                return true;
            } else if (obj1.isNil() && !obj2.isNil()) {
                return false;
            } else {
                const String s1 = obj1.toString();
                const String s2 = obj2.toString();
                return s1.compare(s2, caseSensitive) < 0;
            }
        });
    }
    return objs;
}

Data sort(const Data& input, const std::vector<Data>& args)
{
    return sort_imp(input, args, "sort", true);
}

Data sort_natural(const Data& input, const std::vector<Data>& args)
{
    return sort_imp(input, args, "sort_natural", false);
}

void _gmtime(struct ::tm* tm, const ::time_t t)
{
#ifdef _MSC_VER
    auto err = ::gmtime_s(tm, &t);
    if (err != 0) {
        throw std::runtime_error("gmtime_s failed with error " + std::to_string(err));
    }
#else
    auto gmtime_ptr = ::gmtime_r(&t, tm);
    if (!gmtime_ptr) {
        throw std::runtime_error("gmtime failed with error " + std::to_string(errno));
    }
#endif
}

bool string_to_date(const std::string& input, struct ::tm& tm)
{
    if (input == "now" || input == "today") {
        _gmtime(&tm, std::time(nullptr));
        return true;
    }

    const char *cstr = input.c_str();
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
#ifdef _MSC_VER
#define SSCANF ::sscanf_s
#else
#define SSCANF ::sscanf
#endif
    if (SSCANF(cstr, "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second) == 6) {
        // ok
    } else if (SSCANF(cstr, "%04d-%02d-%02d", &year, &month, &day) == 3) {
        // ok
    } else {
        return false;
    }
#undef SSCANF
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
        throw syntax_error(String("date takes 1 argument, but was passed %1.").arg(args.size()));
    }
    const Data& arg = args.at(0);
    if (!arg.isString() || arg.size() == 0) {
        return input;
    }
    struct ::tm tm;
    if (input.isNumber()) {
        _gmtime(&tm, static_cast<std::time_t>(input.toFloat()));
    } else if (!string_to_date(input.toString().toLower().toStdString(), tm)) {
        return nullptr;
    }
    const auto formatBytes = arg.toString().toStdString();
    const char *formatCstr = formatBytes.c_str();
    size_t bufSize = 100;
    std::vector<char> buf(bufSize + 1, 0);
    size_t numChars;
    int infiniteLoopProtector = 0; // strftime() can return a valid empty string in some cases
    while ((numChars = ::strftime(buf.data(), bufSize, formatCstr, &tm)) == 0 && (infiniteLoopProtector < 100)) {
        bufSize *= 2;
        buf.resize(bufSize + 1, 0);
        ++infiniteLoopProtector;
    }
    return String(std::string(buf.data(), static_cast<int>(numChars)));
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

#include "tests.hpp"

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
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "&#39; &quot; &amp; &lt; &gt; &#39; &quot; &amp; &lt; &gt;");
    }

    SECTION("EscapeOnce") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["what"] = "' \" & < > ' \" & < >";
        Liquid::Data data(hash);
        CHECK(Liquid::StandardFilters::escape_once("' \" & < > ' \" & < >", {}) == "&#39; &quot; &amp; &lt; &gt; &#39; &quot; &amp; &lt; &gt;");
        CHECK(t.parse("{{ what | escape_once }}").render(data) == "&#39; &quot; &amp; &lt; &gt; &#39; &quot; &amp; &lt; &gt;");
        CHECK(t.parse("{{ '1 < 2 & 3' | escape_once }}").render(data) == "1 &lt; 2 &amp; 3");
        CHECK(t.parse("{{ '1 &lt; 2 &amp; 3' | escape_once }}").render(data) == "1 &lt; 2 &amp; 3");
        CHECK(t.parse("{{ '&excl; &#x00021; &#33;' | escape_once }}").render(data) == "&excl; &#x00021; &#33;");
        CHECK(t.parse("{{ '&frac34; &#x000BE; &#190;' | escape_once }}").render(data) == "&frac34; &#x000BE; &#190;");
        CHECK(t.parse("{{ '&ctdot; &#x022EF; &#8943;' | escape_once }}").render(data) == "&ctdot; &#x022EF; &#8943;");
        CHECK(t.parse("{{ '&; &#x; &#;' | escape_once }}").render(data) == "&amp;; &amp;#x; &amp;#;");
        CHECK(t.parse("{{ '&#x0; &#0;' | escape_once }}").render(data) == "&#x0; &#0;");
        CHECK(t.parse("{{ '&#x000BE;&#X000BE;' | escape_once }}").render(data) == "&#x000BE;&#X000BE;");
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

    SECTION("TruncateWords") {
        Liquid::Template t;
        t.parse("{{ 'Ground control to Major Tom.' | truncatewords: 3 }}");
        CHECK(t.render().toStdString() == "Ground control to...");
        t.parse("{{ 'Ground control to Major Tom.' | truncatewords: 3, '--' }}");
        CHECK(t.render().toStdString() == "Ground control to--");
        t.parse("{{ 'Ground control to Major Tom.' | truncatewords: 3, '' }}");
        CHECK(t.render().toStdString() == "Ground control to");
        CHECK_TEMPLATE_RESULT("{{ 'one two three' | truncatewords: 2, 1 }}", "one two1");
    }

    SECTION("Truncate") {
        Liquid::Template t;
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 20 }}");
        CHECK(t.render().toStdString() == "Ground control to...");
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 25, \", and so on\" }}");
        CHECK(t.render().toStdString() == "Ground control, and so on");
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 20, \"\" }}");
        CHECK(t.render().toStdString() == "Ground control to Ma");
        CHECK_TEMPLATE_RESULT("{{ '1234567890' | truncate: 5, 1 }}", "12341");
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
        CHECK(t.parse("{{ 'A1Z' | split: '1' | join: '' }}").render() == "AZ");
        CHECK(t.parse("{{ 'A1Z' | split: 1 | join: '' }}").render() == "AZ");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ what.size }}").render(data).toStdString() == "28");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ names.first }}").render(data).toStdString() == "John");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ names.last }}").render(data).toStdString() == "Ringo");
    }

    SECTION("Default") {
        Liquid::Template t;
        CHECK(t.parse("{{ nil | default: 'test' }}").render().toStdString() == "test");
        CHECK(t.parse("{{ product_price | default: 2.99 }}").render().toStdString() == "2.99");
        Liquid::Data::Hash hash;
        hash["product_price"] = 4.99;
        Liquid::Data data(hash);
        CHECK(t.parse("{{ product_price | default: 2.99 }}").render(data).toStdString() == "4.99");
        hash["product_price"] = "";
        data = Liquid::Data(hash);
        CHECK(t.parse("{{ product_price | default: 2.99 }}").render(data).toStdString() == "2.99");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ array | size }}").render(data).toStdString() == "6");
        CHECK(t.parse("{{ array | compact | size }}").render(data).toStdString() == "2");
        CHECK(t.parse("{{ array | compact | join: ' ' }}").render(data).toStdString() == "hello world");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ pages | size }}").render(data).toStdString() == "5");
        CHECK(t.parse("{{ pages | map: 'category' | size }}").render(data).toStdString() == "5");
        CHECK(t.parse("{{ pages | map: 'category' | join: ' ' }}").render(data).toStdString() == "business celebrities lifestyle sports technology");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ names1 | concat: names2 | join: ',' }}").render(data).toStdString() == "bill,steve,larry,michael");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ array | sort | join: ', ' }}").render(data).toStdString() == "Sally Snake, giraffe, octopus, zebra");
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
        data = Liquid::Data(hash);
        CHECK(t.parse("{{ names | sort: 'name' | map: 'name' | join: ', ' }}").render(data).toStdString() == "Jane, Sally, bob, george");
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
        Liquid::Data data(hash);
        CHECK(t.parse("{{ array | sort_natural | join: ', ' }}").render(data).toStdString() == "giraffe, octopus, Sally Snake, zebra");
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
        data = Liquid::Data(hash);
        CHECK(t.parse("{{ names | sort_natural: 'name' | map: 'name' | join: ', ' }}").render(data).toStdString() == "bob, george, Jane, Sally");
    }
    
    // https://github.com/Shopify/liquid/pull/805
    SECTION("SortMissingProperty") {
        const Liquid::Data::Array input{
            Liquid::Data::Hash{{"price", 4}, {"handle", "alpha"}},
            Liquid::Data::Hash{{"handle", "beta"}},
            Liquid::Data::Hash{{"price", 1}, {"handle", "gamma"}},
            Liquid::Data::Hash{{"handle", "delta"}},
            Liquid::Data::Hash{{"price", 2}, {"handle", "epsilon"}},
        };
        Liquid::Data inputData{input};
        const std::vector<Liquid::Data> args{
            "price",
        };
        const auto result = Liquid::StandardFilters::sort(inputData, args);
        const Liquid::Data::Array expectation{
            Liquid::Data::Hash{{"price", 1}, {"handle", "gamma"}},
            Liquid::Data::Hash{{"price", 2}, {"handle", "epsilon"}},
            Liquid::Data::Hash{{"price", 4}, {"handle", "alpha"}},
            Liquid::Data::Hash{{"handle", "beta"}},
            Liquid::Data::Hash{{"handle", "delta"}},
        };
        CHECK(result == Liquid::Data{expectation});
    }
    
    SECTION("SortMissingProperty2") {
        const Liquid::Data::Array input{
            Liquid::Data::Hash{ { "handle", "gamma" } },
            Liquid::Data::Hash{ { "handle", "alpha" } },
            Liquid::Data::Hash{ { "price", 4 },{ "handle", "alpha" } },
            Liquid::Data::Hash{ { "handle", "delta" } },
            Liquid::Data::Hash{ { "price", 1 },{ "handle", "gamma" } },
            Liquid::Data::Hash{ { "handle", "beta" } },
            Liquid::Data::Hash{ { "price", 2 },{ "handle", "epsilon" } },
        };
        Liquid::Data inputData{ input };
        const std::vector<Liquid::Data> args{
            "price",
        };
        const auto result = Liquid::StandardFilters::sort(inputData, args);
        const Liquid::Data::Array expectation{
            Liquid::Data::Hash{ { "price", 1 },{ "handle", "gamma" } },
            Liquid::Data::Hash{ { "price", 2 },{ "handle", "epsilon" } },
            Liquid::Data::Hash{ { "price", 4 },{ "handle", "alpha" } },
            Liquid::Data::Hash{ { "handle", "gamma" } },
            Liquid::Data::Hash{ { "handle", "alpha" } },
            Liquid::Data::Hash{ { "handle", "delta" } },
            Liquid::Data::Hash{ { "handle", "beta" } },
        };
        CHECK(result == Liquid::Data{ expectation });
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
        CHECK(date("2006-07-05 10:32:11", {"%m/%d/%Y %H.%M.%S"}).toString().toStdString() == "07/05/2006 10.32.11");

        const auto now = ::time(nullptr);
        struct ::tm tm;
#ifdef _MSC_VER
        (void)localtime_s(&tm, &now);
#else
        tm = *::localtime(&now);
#endif
        const auto current_year = Liquid::Data{std::to_string(tm.tm_year + 1900)};
        CHECK(date("now", {"%Y"}) == current_year);
        CHECK(date("today", {"%Y"}) == current_year);
        CHECK(date("Today", {"%Y"}) == current_year);

        CHECK(date(946702800, {"%m/%d/%Y"}).toString().toStdString() == "01/01/2000");
        CHECK(date(10000000000., {"%m/%d/%Y"}).toString().toStdString() == "11/20/2286"); // 64-bit time_t support
        CHECK(date(0, {"%m/%d/%Y"}).toString().toStdString() == "01/01/1970");
    }
}

#endif
