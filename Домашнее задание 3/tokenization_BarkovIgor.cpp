#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <unordered_map>
#include <variant>
#include <string>
#include <conio.h>

using std::string;

// На этой неделе в качестве домашнего задания необходимо решить 2 задачи. 
/*Задание 1 (15 баллов) : на основании кода с вебинара(файл лежит здесь : https://phoenixcs.notion.site/16-04-24-43659f3faac347c0ad1b6efaa85e54ca?pvs=4) 
1. Дополнить решение токенами : max, sqr
2. Добавить все необходимые классы в определение Token
3. Реализовать ParseName, который читает последовательность букв и возвращает токен соответствующей функции или UnknownToken
4. Закончить Tokenize*/

// Платформа выполнения VS 2022, операционная система Windows 7

//      МОЁ РЕШЕНИЕ
// Тут в общем то решение расписывать сильно не буду
// И так думаю по коду понятно будет, тут немного делать нужно было
// Там где я добавлял своё, прокомментировал

//      Чем пользовался при решении
// Статья на хабре как пользоваться std::variant, лекция Ильи Мещерина https://www.youtube.com/watch?v=Z-v0Lf0b1DE&t=5227s
// по вариантам и юнионам
// Chat gpt помог написать функцию для вывода на консоль токенов

// По идее это в отделный файл можно вынести и потом подключить, но для удобства проверющего оставлю здесь
namespace Tokens 
{
    struct OpeningBracket {};

    struct ClosingBracket {};

    struct Number {
        int value;
    };

    struct UnknownToken {
        std::string value;
    };

    struct MinToken {};

    struct MaxToken {};

    struct SqrToken {};

    struct AbsToken {};

    struct Plus {};

    struct Minus {};

    struct Multiply {};

    struct Modulo {};

    struct Divide {};
   
    struct Comma {};
}

using namespace Tokens;

using Token = std::variant<OpeningBracket, ClosingBracket, Number, UnknownToken, MinToken, MaxToken, SqrToken, AbsToken, Plus, Minus, Divide, Modulo, Multiply, Comma>;

const std::unordered_map<char, Token> kSymbol2Token = { {'+', Plus{}},
                                                        {'-', Minus{}},
                                                        {'*', Multiply{}},
                                                        {'/', Divide{}},
                                                        {'%', Modulo{}},
                                                        {'(', OpeningBracket{}},
                                                        {')', ClosingBracket{}},
                                                        {',', Comma{}}
};

// Добавил аналогичную хеш-таблцу для имён функций
const std::unordered_map<std::string, Token> kName2Token{ {"min", MinToken{}},
                                                          {"max", MaxToken{}},
                                                          {"sqr", SqrToken{}},
                                                          {"abs", AbsToken{}} };


int ToDigit(unsigned char symbol) {
    return symbol - '0';
}


Number ParseNumber(const std::string& input, size_t& pos) 
{
    int value = 0;
    auto symbol = static_cast<unsigned char>(input[pos]);
    while (std::isdigit(symbol))
    {
        value = value * 10 + ToDigit(symbol);
        if (pos == input.size() - 1) 
        {
            // На лекции подзабыто увеличение здесь, путём дебага устраняем багу
            ++pos;
            break;
        }
        symbol = static_cast<unsigned char>(input[++pos]);
    }
    return Number{ value };
}

// В общем алгоритм такой: идём пока встречаются символы алфавита, как только закончились
// Проверяем есть ли такое имя в мапе с именами функций, если нет то возвращяем аннаун токен
Token ParseName(const std::string& input, size_t& pos) {
    std::string name;
    auto symbol = static_cast<unsigned char>(input[pos]);
    while (std::isalpha(symbol)) 
    {
        // Время работы за O(1) амортизированное, как в вектор короче
        name += symbol;
        if (pos == input.size() - 1) {
            break;
        }
        symbol = static_cast<unsigned char>(input[++pos]);
    }
    if (auto it = kName2Token.find(name); it != kName2Token.end())
    {
        return it->second;
    }
    else {
        return UnknownToken{ name };
    }
}

// функция для вывод токенов в консоль, чат Gpt сгенерил мне её, напишу построчно, что делает каждая строка
// разобрался со всем что он мне подкинул
void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        // Используем std::visit для обработки каждого типа токена
        // специальная функция для обработки значения типа std::variant<...>
        // вызывает анонимную функцию, которая обрабатывает значение типа std::variant<...>
        // в данном случае мы просто хотим вывести в консоль наши токены
        std::visit([](const auto& t) {
            //Функция std::decay_t отбрасывает все ссылки и константы, а деклтайп даёт нам тип переменной, причём со всеми ссылками и константами
            using T = std::decay_t<decltype(t)>;
            // is_same_v стандартная метафункция воврщает тип bool
            // Сравнивает тип t с образцом, если совпадает выводим, если нет
            // Сравниваем следующий
            if constexpr (std::is_same_v<T, OpeningBracket>) {
                std::cout << "OpeningBracket ";
            }
            else if constexpr (std::is_same_v<T, ClosingBracket>) {
                std::cout << "ClosingBracket ";
            }
            else if constexpr (std::is_same_v<T, UnknownToken>) {
                std::cout << "UnknownToken: " << t.value << " ";;
            }
            else if constexpr (std::is_same_v<T, SqrToken>) {
                std::cout << "SqrToken ";
            }
            else if constexpr (std::is_same_v<T, MaxToken>) {
                std::cout << "MaxToken ";
            }
            else if constexpr (std::is_same_v<T, Multiply>) {
                std::cout << "Multiply ";
            }
            else if constexpr (std::is_same_v<T, MinToken>) {
                std::cout << "MaxToken ";
            }
            else if constexpr (std::is_same_v<T, AbsToken>) {
                std::cout << "AbsToken ";
            }
            else if constexpr (std::is_same_v<T, Number>) {
                std::cout << "Number: " << t.value << " ";
            }
            else if constexpr (std::is_same_v<T, Comma>) {
                std::cout << "Comma " << " ";
            }
            else {
                // Обработка других типов токенов
            }
            }, token);
    }
    std::cout << std::endl;
}

std::vector<Token> Tokenize(const std::string& input) {
    std::vector<Token> tokens;
    const size_t size = input.size();
    size_t pos = 0;

    while (pos < size) 
    {
        const auto symbol = static_cast<unsigned char>(input[pos]);
        if (std::isspace(symbol)) {
            ++pos;
        }
        else if (std::isdigit(symbol)) {
            tokens.emplace_back(ParseNumber(input, pos));
        } // Добавлена проверка на символ алфавита
        else if (std::isalpha(symbol)) {
            tokens.emplace_back(ParseName(input, pos));
        }
        else if (auto it = kSymbol2Token.find(symbol); it != kSymbol2Token.end()) {
            tokens.emplace_back(it->second);

            // эта строчка забыта у лектора
            ++pos;
        }

    }
    return tokens;
}

// Попробуем затестить как работает вообще, как разбивает на токены выражения
int main()
{
    // Вроде строчка теперь корректно токенизируется так сказать, тестов бы навалить ещё (^_^), ну это на будущее
    string testStr = "asfd 5sdfasdfas+5  abs(3, 4) *  5 max   () 000 22 min(1) , 212 ** 1 ";

    auto vtokens = Tokenize(testStr);

    printTokens(vtokens);

    _getch();
}