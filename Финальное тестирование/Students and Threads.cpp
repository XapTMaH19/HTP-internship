#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <memory>
#include <conio.h>


using std::string;
using std::unordered_map;
using std::shared_ptr;

// Структура, представляющая студента
struct Student
{
    size_t id;
    string name;
    size_t age;
};

class StudentDatabase {
private:
    // Контейнер для хранения студентов
    std::unordered_map<int, shared_ptr<Student>> students;
    // Базовый мьютекс для синхронизации доступа к базе данных
    std::mutex mutex;

public:

    // Добавление нового студента в базу данных
    void AddStudent(size_t id, const std::string& name, size_t age)
    {
        // Блокировка мьютекса и оборачивание его в обёртку для освобождения, если будет исключение, т.е. когда у нас закончится область видимости lock_guard-а, то будет вызван
        // деструктор, который освободит mutex
        std::lock_guard<std::mutex> lock(mutex);
        students[id] = std::make_shared<Student>(Student{ id, name, age });
    }

    // Удаление студента по идентификатору
    void RemoveStudent(size_t id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        // erase не кидает исключений, если id нету в маpe то ничего не произойдёт
        students.erase(id);
    }

    // Получение информации о студенте по идентификатору
    std::shared_ptr<Student> GetStudentById(int id)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = students.find(id);
        if (it != students.end())
        {
            return it->second;
        }

        // если студент не найден
        return nullptr;
    }

    // Вспомогательная функция для отображения информации о студенте
    void OutStudent(const std::shared_ptr<Student>& student)
    {
        // для красивого выводы
        std::lock_guard<std::mutex> lock(mutex);

        if (student) {
            std::cout << "ID: " << student->id << ", Name: " << student->name << ", Age: " << student->age << std::endl;
        }
        else {
            std::cout << "Student не найден." << std::endl;
        }
    }
};

// Функция для демонстрации работы с базой данных в разных потоках
void DataBaseDemo(StudentDatabase& db)
{
    // Добавление студентов в базу данных
    db.AddStudent(1, "Анастасия", 20);
    db.AddStudent(2, "Артём", 22);
    db.AddStudent(3, "Соня", 21);

    // Получение информации о студенте по идентификатору
    std::shared_ptr<Student> student = db.GetStudentById(2);
    db.OutStudent(student);

    // Удаление студента по идентификатору
    db.RemoveStudent(2);
    db.RemoveStudent(4);
    student = db.GetStudentById(1);
    db.OutStudent(student);
    student = db.GetStudentById(2);
    db.OutStudent(student);
    student = db.GetStudentById(3);
    db.OutStudent(student);

}

int main()
{
    setlocale(LC_ALL, "Russian");

    StudentDatabase db;

    // Создание двух потоков для демонстрации работы с базой данных в разных потоках
    std::thread thread1(DataBaseDemo, std::ref(db));
    std::thread thread2(DataBaseDemo, std::ref(db));

    // Ожидание завершения работы потоков
    thread1.join();
    thread2.join();

    _getch();

    return 0;
}
