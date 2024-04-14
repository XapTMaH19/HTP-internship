#include <iostream>
#include <vector>
#include <bitset>
#include <thread>
#include <conio.h>
#include <iomanip>
#include <random>
#include <numeric>

#define SIZE_BYTE 8

using std::vector;
using std::cout;
using std::endl;
using std::bitset;
using std::thread;
using std::setw;

// Использовал Visual Studio для написания кода, Операционная система Windows 7
// Код сразу говорю сам писал, chatGPT и джарвис не помогали его писать, только стаковерфлоу и лекции, по старинке в общем

// Задание
/*На этой неделе в качестве домашнего задания мы пишем код.
Задание звучит так: Напишите функцию, которая инвертирует только выбранные биты в участке памяти, заданном указателем и длиной в байтах.
Способ выбора битов для инвертирования на ваше усмотрение.*/

// Мои рассуждения
/*
    -- Про указатель
    Участок памяти это просто последовательность 0-ей и 1-иц, и не так сильно важно что под ними таится и наша задача инвертировать из
    этого участка выбранные биты. Можно взять на вход функции void* и потом преобразовывать его кастовать к uint8_t, но
    я сразу буду работать с uint8_t.

    -- Зачем нужен n?
    Длина участка ограничена в n байт. Я так понял, это ну чтобы чужую память не похерить.
    Поэтому нужно будет проверку делать, при попытке изменить биты, причём каждый раз, так как я на вход подаю массив из
    различных индексов БИТОВ, и мы не знаем будет ли следующий бит выходить за границы или нет.
    0 бит означает первый бит первого байта, и дальше по порядку.

    -- Решение задачи
    Будет основываться на побитовой операции xor (исключающее или). т.е.
    надо проинвертировать какой-то бит байта, тогда мы создаём байт в котором за 1 примем бит, который надо проинвертировать,
    а за 0 бит, который не нужно троготь, тогда
    1 с 1 даст нам 0, то что нужно (проинвертировали)
    1 с 0 даст нам 1, то что нужно (проинвертировали)
    0 с 1 даст нам 1, то что нужно (не тронули 1)
    0 с 0 даст нам 0, то что нужно (не тронули 0)

    -- Что взять за выбранные биты?
    Мне пришло в голову два способа как можно выбирать биты для инвертирование
    1) Просто задать набор чисел, где номер будет соответствовать номеру бита, который нужно проинвентировать
        Здесь я сделаю две функции одну для вектора, другую для С-ого массива
        P.S. можно сделать шаблонную функцию, куда будет приходить какой-нибудь T = Container контейнер, и дальше по нему нужно
            проитерироваться и также всё проинвертировать, но задание не про это, поэтому не буду усложнять.
        - не понятно как распараллелить, ибо числа могут идти хаотично и повторятся, там уже нужно синхронизировать потоки будет,
        это может стать неоправданным усложнением. Потоки ради потоков нам не нужны.

    2) На вход приходит определённая маска для каждого байта памяти и мы проходимся по всему участку с этой маской
        и инвертируем в соответствии с ней все байты, к пример маска 10100000 проинвертирует только 0-ой и 2-ой бит каждого байта
        P.S. есть возможность распараллелить алгоритм, что мы и сделаем

*/


// Версия функции с набором выбранных битов на основе вектора
// Время работы за O(vec.size()) входной параметр задающий биты vector<size_t>
void BitInvert(uint8_t* ptr, size_t n, const vector<size_t>& vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] / SIZE_BYTE <= n)
        {
            ptr[vec[i] / SIZE_BYTE] ^= (1u << SIZE_BYTE - (vec[i] % SIZE_BYTE) - 1);
        } 
    }
}

// Версия функции с набором выбранных битов на основе C-ого массива, нужно дополнительно подать ещё длину этого массива
// Время работы за O(lenarr) входной параметр задающий биты, которые надо проинвертировать size_t arr[]
void BitInvert(uint8_t* ptr, size_t n, const size_t arr[], size_t lenarr)
{
    for (int i = 0; i < lenarr; i++)
    {
        if (arr[i] / SIZE_BYTE <= n)
        {
            ptr[arr[i] / SIZE_BYTE] ^= (1u << SIZE_BYTE - (arr[i] % SIZE_BYTE) - 1);
        }
    }
}

// Версия функции с инвертированием всего участка по маске
void BitInvert(uint8_t* ptr, size_t n, uint8_t mask)
{
    for (int i = 0; i < n; i++)
    {
        ptr[i] ^= mask;
    }
}

// Версия функции с инвертированием подучастка для параллельного выполнения по маске
void BitInvertforParallel(uint8_t* begin, uint8_t* end, uint8_t mask)
{
    auto ptr = begin;
    for (size_t i = 0; begin != end; ++begin, i++)
    {
        ptr[i] ^= mask;
    }
}
// P.S. можно наверное было бы обобщить 2-е функции выше, но я решил не запутывать и сделал разные сигнатуры

// Код многопоточной функции брал с лекции и немного подредактировал его под нашу задачку
template <typename Function>
void ParallelBitInvert(uint8_t* ptr, size_t n, uint8_t mask, Function func)
{
    if (n == 0)
    {
        return;
    }
    const size_t min_job_size = 1000;
    const size_t hardware_threads = std::thread::hardware_concurrency();
    const size_t max_threads = hardware_threads == 0 ? 2 : hardware_threads;
    const size_t num_threads = std::min(max_threads, (n + min_job_size - 1) / min_job_size);
    

    if (num_threads < 3)
    {
        BitInvert(ptr, n, mask);
    }
    else {
        const size_t block_size = n / (num_threads - 1);
        vector<thread> threads;
        threads.reserve(num_threads - 1);
        auto first = ptr;
        for (size_t i = 0; i < num_threads - 1; ++i)
        {
            // На последний поток ещё остаток добавиться (^_^)
            if (i == num_threads - 2)
            {
                auto last = first + block_size + n % (num_threads - 1);
                threads.emplace_back(func, first, last, mask);
                first = last;
            }
            else
            {
                auto last = first + block_size;
                threads.emplace_back(func, first, last, mask);
                first = last;
            }

        }
        for (auto& t : threads)
        {
            t.join();
        }
    }

}


int main()
{
    setlocale(LC_ALL, "RU");

    const size_t smallsize = 10;
    uint8_t arr[smallsize] = { 1, 2, 3, 5, 7, 12, 234, 223 };

    cout << setw(60) << "До инверсии :";
    for (int i = 0; i < smallsize; i++)
    {
        cout << std::bitset<8>(arr[i]);
    }
    cout << endl;

    // Заполняем с 0 до smallsize * 8, тобишь выбираем все биты
    size_t arrInd[smallsize * 8];
    for (size_t i = 0; i < smallsize * 8; ++i) {
        arrInd[i] = i;
    }
    vector<size_t> vecInd(smallsize * 8);
    std::iota(vecInd.begin(), vecInd.end(), 0);

    // Запускаем несколько вариантов функций на небольшом участке памяти
    cout << setw(60) << "После инверсии BitInvert(-||-, vector<size_>):";
    BitInvert(arr, smallsize, vecInd);
    for (int i = 0; i < smallsize; i++)
    {
        cout << std::bitset<8>(arr[i]);
    }
    cout << endl;

    BitInvert(arr, smallsize, arrInd, smallsize * 8);
    cout << setw(60) << "После инверсии BitInvert(-||-, arrInd, lenInd): ";
    for (int i = 0; i < smallsize; i++)
    {
        cout << std::bitset<8>(arr[i]);
    }
    cout << endl;

    BitInvert(arr, smallsize, 0b11110000);
    cout << setw(60) << "После инверсии BitInvert(-||-, mask): ";
    for (int i = 0; i < smallsize; i++)
    {
        cout << std::bitset<8>(arr[i]);
    }
    cout << endl;
    ParallelBitInvert(arr, smallsize, 0b11110000, BitInvertforParallel);
    cout << setw(60) << "После инверсии ParallelBitInvert(-||-, mask):";
    for (int i = 0; i < smallsize; i++)
    {
        cout << std::bitset<8>(arr[i]);
    }
    cout << endl;

    // Бенчмарки с многопоточной версией и однопоточноый, сравение работы.

    // Приходится очень много выделять памяти чтобы увидеть преимущество многопоточки,
    // так как стека не хватает чтобы продемонстрировать это.
    // Сделаем два массива, чтобы проверить, что многопоточка нам всё правильно проинвертировала,
    // Однопоточную версию возьмём за образец
    const size_t bigsize = 100000000;
    uint8_t* arrbig1 = new uint8_t[bigsize];
    uint8_t* arrbig2 = new uint8_t[bigsize];
    //uint8_t arrbig[bigsize];
    for (int i = 0; i < bigsize; ++i)
    {
        arrbig1[i] = rand() % 256;
    }
    for (int i = 0; i < bigsize; ++i)
    {
        arrbig2[i] = arrbig1[i];
    }

    auto start = std::chrono::high_resolution_clock::now();
    BitInvert(arrbig1, bigsize, 0b11110000);
    auto end = std::chrono::high_resolution_clock::now();
    cout << setw(30) << "Без многопоточки : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "mc" << endl;

    start = std::chrono::high_resolution_clock::now();
    ParallelBitInvert(arrbig2, bigsize, 0b11110000, BitInvertforParallel);
    end = std::chrono::high_resolution_clock::now();
    cout << setw(30) << "C многопоточкой : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "mc" << endl;

    bool flag = true;
    for (size_t i = 0; i < bigsize; i++)
    {
        if (arrbig1[i] != arrbig2[i]) 
        {
            cout << "i: " << i << " arrbig1[i] = " << arrbig1[i] << "arrbig2[i] = " << arrbig2[i] << endl;
            flag = false;
            break;
        }
    }

    if (flag) 
    {
        cout << "OK: arrbig1 == arrbig2 побитно" << endl;
    }
    else
    {
        cout << "Ошибка!!! : arrbig1 != arrbig2 побитно" << endl;
    }

    delete[] arrbig1;
    delete[] arrbig2;

    _getch();
}