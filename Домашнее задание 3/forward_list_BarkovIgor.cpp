
#include <iostream>
#include <memory>
#include <utility>
#include <forward_list>
#include <conio.h>
#include <chrono>

//            (буду называть односвязный список кратко ЛИСТОМ, чтобы быстрее было объясняться)

/*      Задание Задание 2 (30 баллов):
    Напишите библиотеку, реализующую шаблонный класс для односвязного списка с поддержкой операций вставки, удаления и поиска заданного элемента.

	Делал всё в VS 22, windows 7.

    // Что использовал при решении
        1) Лекции Ильи Мещерина, до сих пор их смотрю, старался делать код по его гайдам
        2) Почитал пару статей в Интернете про то как устроен изнутри std::forward_list ещё искал как реализовать end()
        3) chatGPT подсказывал мне, когда я не мог найти ошибку в коде и давал рекомендации по коду и логике


        Как я понял задание: 
    Цель работы продемонстрировать умение работы с шаблонами, указателями, выделением памяти и итераторами.
    Хоть в задании этого и не было написано, но когда пишешь библиотеку, то нужно серьёзней подходить к её реализации, чтобы человек которому нужно
    было в ней разобраться быстро понял что в ней происходит. Поэтому нужно использоваться стандартные подходы к реализации контейнера.

        Моё решение:
    Нужно реализовать односвязный список. Это такой контейнер, который поддерживает хранение любого типа T, поддерживает forward_iterator-ы
    и устроен как набор связанных между собой нод, но только в одном направлении. Т.е. назад ходить нельзя по итератору, только вперёд. Нода это указатель
    на следующую ноду и значение. Преимущества его в том, что вставка и удаление в него происходят за O(1). В остальном он во всём проигрывает вектору.
    Да ещё момент, по поводу аллокации элементов о-го списка, каждый раз когда вставляем это new, а это очень дорого, хотя и за O(1). Можно придумать такой аллокатор
    который бы выделял на стеке всё, т.е. без new и тогда мы бы заметно увидели прирост в скорости вставки. 

    Так что я делал, внутри листа  определил Nod-у и итератор, использовал аллокаторы из стандартной библиотеки, ну потому что
    new явно вызывать это глупо, когда есть удобные обёртки. Да по поводу итераторов, они должны поддерживать методы (begin() и end()) и (cbegin() и (cend())), поэтому сделаем
    этот класс шаблонным от bool и там метафункцией std::conditional_t<IsConst, const T&, T&>; разделим таким образом два класса, возвращаемый тип будет либо с const либо без
    Так мы избавимся от копипаста. Подсмотрел эту идею у Илья Мещерина.
    По коду, мы определяем стандартные операторы у итераторов, у листа я добавил некоторые стандартные методы от себя.

    Вставка, удаление и поиск.
    вставка и удаление работают по итератору. Вставка добавляет новый элемента ПОСЛЕ!!! итератора
    Удаление удаляет элемент с заданных на вход итератором
    поиск ищем итератор по значению
    !!!! После ошибок во втором ДЗ сразу позаботился о валидности входных переменных и сделал функцию IsValidIterator()
    Это конечно замедляет методы, но зато программа не упадёт (^_^)

    !!!Про end() 
    Есть фэйковая нода head, на неё зацикливается весь лист, она хранит указатель на первую ноду


*/


namespace mystd {

    template <typename T, typename Alloc = std::allocator<T>>
    class forward_list
    {
    private:
        using value_type = T;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        struct Node;

        // Перепривязываем аллокатор на ноду ибо он изначально на T задан, а нам нужно чтобы он ноды выделял
        typename Alloc::template rebind<Node>::other alloc;

    private:
        // Шаблон здесь нужен, для избавления от копипаста, но я это уже писал выше
        template <bool IsConst>
        class base_iterator {
        public:
            friend class forward_list<T, Alloc>;
            using reference_type = std::conditional_t<IsConst, const T&, T&>;
            
        public:
            base_iterator();
            base_iterator(Node* new_node);
            base_iterator(const base_iterator<IsConst>& l) noexcept;
            base_iterator& operator++() noexcept;
            base_iterator operator++(int) noexcept;
            reference operator*() noexcept;
            Node* getNode() noexcept;
            bool operator!=(const base_iterator& l) const noexcept;
            base_iterator& operator=(const base_iterator& l);
            bool operator==(const base_iterator& l) const noexcept;

        private:
            Node* node;

        };

        
    public:
        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;

        forward_list();
        explicit forward_list(std::initializer_list<value_type> const& items);
        forward_list(forward_list&& l);
        forward_list& operator=(forward_list&& l) noexcept;
        ~forward_list();
        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;
        const_reference front() const;
        const_reference back() const;
        bool empty() const noexcept;
        size_type size() const noexcept;
        size_type max_size() const noexcept;
        void clear() noexcept;
        
        void push_front(const_reference value);
        void pop_front();

        iterator find(const_reference value) noexcept;
        iterator insert_after(iterator pos, const_reference value);
        void erase(iterator pos) noexcept;

    private:
        struct Node
        {
            Node* next;
            value_type data;
            Node();
            Node(Node* next, const_reference data);
        };
        Node* head;
        size_type sz;
        using AllocTraits = std::allocator_traits<typename std::allocator_traits<Alloc>::template rebind_alloc<Node>>;
        bool IsValidIterator(iterator pos) noexcept;
    };

    // Определяем методы итератора
    template <typename T, typename Alloc>
    forward_list<T, Alloc>::Node::Node() : next(nullptr), data(T()) {}

    template <typename T, typename Alloc>
    forward_list<T, Alloc>::Node::Node(Node* next, const_reference data)
        : next(next), data(data) {}

    template <typename T, typename Alloc>
    template <bool IsConst>
    forward_list<T, Alloc>::base_iterator<IsConst>::base_iterator() : node(nullptr) {}

    template <typename T, typename Alloc>
    template <bool IsConst>
    forward_list<T, Alloc>::base_iterator<IsConst>::base_iterator(Node* new_node)
        : node(new_node) {}

    template <typename T, typename Alloc>
    template <bool IsConst>
    forward_list<T, Alloc>::base_iterator<IsConst>::base_iterator(const base_iterator<IsConst>& li) noexcept
        : node(li.node) {}

    template <typename T, typename Alloc>
    template <bool IsConst>
    typename forward_list<T, Alloc>::template base_iterator<IsConst>&
    forward_list<T, Alloc>::base_iterator<IsConst>::operator++() noexcept
    {
        node = node->next;
        return *this;
    }

    template <typename T, typename Alloc>
    template <bool IsConst>
    typename forward_list<T, Alloc>::template base_iterator<IsConst>
    forward_list<T, Alloc>::base_iterator<IsConst>::operator++(int) noexcept
    {
        base_iterator copy = *this;
        copy.node = node->next;
        return copy;
    }

    template <typename T, typename Alloc>
    template <bool IsConst>
    typename forward_list<T, Alloc>::reference
    forward_list<T, Alloc>::base_iterator<IsConst>::operator*() noexcept
    {
        return node->data;
    }

    template <typename T, typename Alloc>
    template <bool IsConst>
    typename forward_list<T, Alloc>::Node*
    forward_list<T, Alloc>::base_iterator<IsConst>::getNode() noexcept
    {
        return node;
    }

    template <typename T, typename Alloc>
    template <bool IsConst>
    bool forward_list<T, Alloc>::base_iterator<IsConst>::operator!=(const base_iterator& l) const noexcept
    {
        return node != l.node;
    }

    template <typename T, typename Alloc>
    template <bool IsConst>
    typename forward_list<T, Alloc>::template base_iterator<IsConst>&
    forward_list<T, Alloc>::base_iterator<IsConst>::operator=(const base_iterator& l) 
    {
        node = l.node;
        return *this;
    }

    template <typename T, typename Alloc>
    template <bool IsConst>
    bool forward_list<T, Alloc>::base_iterator<IsConst>::operator==(const base_iterator& l) const noexcept 
    {
        return node == l.node;
    }
   
    // Конструкторы листа
    template <typename T, typename Alloc>
    forward_list<T, Alloc>::forward_list() : head(nullptr), sz(0) 
    {
        head = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, head);
        head->next = head;
    }

    template <typename T, typename Alloc>
    forward_list<T, Alloc>::forward_list(std::initializer_list<value_type> const& items) : forward_list() 
    {
        Node* cur_node = head;
        for (size_type i = 0; i < items.size(); ++i) {
            Node* node = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, node, node, *(items.begin() + i));
            cur_node->next = node;
            cur_node = node;
            ++sz;
        }
        cur_node->next = head;
    }

    // Сделать мув конструктор и оператор присваивая
    template <typename T, typename Alloc>
    forward_list<T, Alloc>::forward_list(forward_list&& l) : alloc(std::move(l.alloc)), head(l.head), sz(l.sz)
    {
        l.head = nullptr; 
        l.sz = 0;
    }

    template <typename T, typename Alloc>
    forward_list<T, Alloc>& forward_list<T, Alloc>::operator=(forward_list&& l) noexcept
    {
        alloc = std::move(l.alloc);
        return *this;
    }

    // Деструктор
    template <typename T, typename Alloc>
    forward_list<T, Alloc>::~forward_list() 
    {
        while (sz)
        {
            erase(begin());
        }
        AllocTraits::deallocate(alloc, head, 1);
    }

    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::iterator forward_list<T, Alloc>::begin() noexcept 
    {
        return iterator(head->next);
    }

    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::iterator forward_list<T, Alloc>::end() noexcept 
    { 
        return iterator(head);
    }

    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::const_iterator forward_list<T, Alloc>::cbegin() const noexcept 
    {
        return const_iterator(head->next);
    }

    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::const_iterator forward_list<T, Alloc>::cend() const noexcept 
    {
        return const_iterator(head);
    }

    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::const_reference forward_list<T, Alloc>::front() const 
    {
        return head->next->data;
    }  

    template <typename T, typename Alloc>
    bool forward_list<T, Alloc>::IsValidIterator(iterator it) noexcept
    {
        Node* cur = it.getNode();
        Node* prev = head;

        while (prev->next != nullptr)
        {
            if (prev->next == cur)
            {
                return true;
            }
            prev = prev->next;
        }

        return false;

    }

    template <typename T, typename Alloc>
    bool forward_list<T, Alloc>::empty() const noexcept 
    {
        return sz == 0 ? true : false;
    }

    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::size_type forward_list<T, Alloc>::size() const noexcept 
    {
        return sz;
    }

    // Говорит нам сколько максимально может быть выделено элементов в текущем аллокаторе
    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::size_type forward_list<T, Alloc>::max_size() const noexcept 
    {
        return AllocTraits::max_size(alloc);
    }

    template <typename T, typename Alloc>
    void forward_list<T, Alloc>::clear() noexcept 
    {
        while (sz) erase(begin());
    }

    // Функция вставки элемента по итератору после него
    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::iterator forward_list<T, Alloc>::insert_after(iterator pos, const_reference value) 
    {
        if (IsValidIterator(pos))
        {
            Node* node = AllocTraits::allocate(alloc, 1);
            try {
                AllocTraits::construct(alloc, node, node, value);
            }
            catch (...) {
                AllocTraits::deallocate(alloc, node, 1);
                throw;
            }

            Node* cur = pos.getNode();
            node->next = cur->next;
            cur->next = node;
            ++sz;
            return iterator(node);
        }
        else 
        {
            return end();
        }

    }
 
    // Вставка и удаление с начала
    template <typename T, typename Alloc>
    void forward_list<T, Alloc>::push_front(const_reference value) {
        insert_after(end(), value);
    }

    template <typename T, typename Alloc>
    void forward_list<T, Alloc>::pop_front() {
        erase(begin());
    }

    // Удаление элемента по итератору
    template <typename T, typename Alloc>
    void forward_list<T, Alloc>::erase(iterator pos) noexcept
    {
        if (pos == end())
        {
            return;
        }
        if (IsValidIterator(pos))
        {
            Node* cur = pos.getNode();
            Node* prev = head;

            while (prev->next != cur) {
                prev = prev->next;
            }

            prev->next = cur->next;

            AllocTraits::destroy(alloc, cur);
            AllocTraits::deallocate(alloc, cur, 1);
            --sz;
        }

    }

    // Поиск итератора на элемент по значению
    template <typename T, typename Alloc>
    typename forward_list<T, Alloc>::iterator forward_list<T, Alloc>::find(const_reference value) noexcept
    {
        for (auto it = begin(); it != end(); ++it) 
        {
            if (*it == value)
            {
                return it;
            }
        }
        // Если ничего нет возвращаем end(), итератор на фиктивную ноду
        return end(); 
    }
}  

// Бенчмарки между forward_list-ом из std и моим

int main()
{
    setlocale(LC_ALL, "RU");

    // мой лист работает гораздо медленнее чем стандартный, с проверкой на валидность итератора
    // !!!!!!!!!НО если убрать проверку на валидность, то в два раза быстрее работает вставка
    const size_t SIZElIST = 10;

    auto start = std::chrono::high_resolution_clock::now();
    mystd::forward_list<int> l;
    for (int i = 0; i < SIZElIST; i++)
    {
        l.push_front(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Время добавления в mylist : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "mc" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    std::forward_list<int> lstd;
    for (int i = 0; i < SIZElIST; i++)
    {
        lstd.push_front(i);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Время добавления в std::forward_list : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "mc" << std::endl;




    // Проверим erase и find
    for (auto& v : l)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    auto itbuf1 = l.begin();
    const size_t numErase = 0;
    for (int i = 0; i < numErase; ++i)
    {
        ++itbuf1;
    }
    l.erase(itbuf1);
    if (l.find(l.size() - numErase) == l.end())
    {
        std::cout << "элемент " << l.size() - numErase << " удалён!" << std::endl;
    }
    // Проверим erase
    for (auto& v : l)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    // удалим всё
    l.clear();
    if (l.empty())
    {
        std::cout << "my_forward_list - пустой" << std::endl;
    }


    _getch();
    return 0;
}