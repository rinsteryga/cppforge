SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: Первые шаги в язык C++...';

    INSERT INTO modules (name, description) 
    VALUES ('Первые шаги в язык C++', 'Переход от C к C++, новые концепции и философия')
    RETURNING id INTO v_mod_id;

    -- Lesson 8.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '8.1. g++/clang++ и первая программа на C++', 'C++ создавался Бьёрном Страуструпом как "C с классами". Он почти полностью обратно совместим с C, но пропагандирует другой стиль: "ноль накладных расходов" (Zero-cost abstractions) и сильную типизацию.

Для компиляции кода (.cpp) используется компилятор g++ (из коллекции GCC) или clang++.
Вместо stdio.h в C++ принято использовать мощную библиотеку потоков <iostream>.

  #include <iostream>
  int main() {
      std::cout << "Hello, C++!" << std::endl;
      return 0;
  }

Нюанс: std::cout не требует спецификаторов формата ("%d"), потому что в C++ реализована перегрузка операторов — компилятор сам определяет нужный тип! Оператор << (сдвиг битов в C) переопределен для вывода в поток.', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '8.2. Новые базовые типы данных', 'В C++ появились встроенные типы, которых не было в C (или которые подключались костылями):

1. bool — логический тип, принимающий true или false. (В памяти занимает обычно 1 байт, так как процессор не умеет адресовать отдельные биты).
2. nullptr — специальное ключевое слово для нулевых указателей. НИКОГДА больше не используйте макрос NULL из C. NULL — это просто число 0, что может вызвать неоднозначность в C++. nullptr имеет собственный тип (std::nullptr_t).
3. auto — (начиная с C++11) позволяет компилятору самому вывести тип переменной при инициализации:
   auto x = 10; // x будет int
   auto d = 3.14; // d будет double', 2)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '8.3. Псевдонимы. using. namespace', 'Пространства имен (namespace) решают проблему коллизии имен. В больших проектах на C имена функций приходилось делать длинными: my_lib_do_something(), чтобы они не пересекались.
В C++ можно обернуть код:
  namespace Game {
      void run() {}
  }
И вызывать как Game::run(). 

Вся стандартная библиотека C++ (STL) лежит в пространстве std.
Конструкция "using namespace std;" открывает его целиком. Нюанс: никогда не пишите это в заголовочных файлах (.h или .hpp)! Это отравит глобальное пространство имен всех, кто включит ваш заголовок.

Вместо typedef из C в современном C++ используют using:
  using Point = struct { int x, y; }; // Гораздо читабельнее!', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '8.4. Ссылки...', 'Ссылка (&) — это синтаксический сахар над указателями, представленный в C++.
Ссылка — это псевдоним (альтернативное имя) для существующей переменной.

  int a = 10;
  int &ref = a;
  ref = 20; // Теперь a равно 20!

Нюансы ссылок:
1. Обязательна инициализация при создании. Нельзя создать "пустую" ссылку (как NULL указатель).
2. Ссылку нельзя переназначить на другой объект после создания. Она навечно привязана.
3. Под капотом компилятор реализует ссылки через указатели (константные указатели int * const), но синтаксис не требует разыменования (*). Это делает код намного чище при передаче в функции: void update(Player &p) {...}', 4)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '8.5. Новые возможности const. constexpr', 'C++ гораздо строже относится к константности (const-correctness), чем C. Использование const защищает от случайных изменений.
  void print(const std::string& str); // Передача по константной ссылке — стандарт в C++ для передачи больших объектов (быстро и безопасно).

С выходом C++11 появилось ключевое слово constexpr.
constexpr указывает компилятору вычислить выражение во время компиляции (Compile-time evaluation), а не в рантайме.
  constexpr int get_max() { return 100 * 2; }
  int arr[get_max()]; // В C++ так можно, результат вычислен заранее!', 5)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '8.6. new / delete', 'Хотя в C++ можно использовать malloc/free, это считается антипаттерном.
Для динамической памяти используются операторы new и delete.

Разница критична:
- malloc просто выделяет сырую память. new выделяет память И вызывает конструктор объекта (инициализирует его).
- free просто освобождает байты. delete вызывает деструктор (очищает ресурсы объекта) И освобождает память.

  int *ptr = new int(42); // Выделение памяти под одно число
  delete ptr;

Для массивов:
  int *arr = new int[100];
  delete[] arr; // Нюанс: обязательно использовать delete[], иначе вызовется деструктор только для первого элемента!', 6)
    RETURNING id INTO v_lesson_id;

END $$;
