SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 12: STL. Основы...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 12: STL. Основы', 'Обзор стандартной библиотеки шаблонов: контейнеры, итераторы, алгоритмы.')
    RETURNING id INTO v_mod_id;

    -- 12.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.1. Контейнеры', 'STL предоставляет готовые структуры данных: последовательные (vector, list, deque) и адаптеры (stack, queue, priority_queue).
Самый часто используемый контейнер — `std::vector<T>`. Это динамический массив, который сам управляет своей памятью. Если место заканчивается, он автоматически выделяет новый буфер (обычно в 2 раза больше), копирует элементы и удаляет старый.

  #include <vector>
  std::vector<int> v = {1, 2, 3};
  v.push_back(4); // Добавление в конец
  std::cout << v.size(); // Текущий размер');
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 12.1
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Использование std::vector', 
        'Напишите программу, которая считывает число N (количество элементов), затем считывает N целых чисел в `std::vector`. После этого найдите и выведите сумму всех элементов вектора.

Формат ввода:
Целое число N (до 1000). Затем N целых чисел через пробел.

Формат вывода:
Сумма чисел.

Пример:
Ввод: 3\n10 20 30
Вывод: 60', 
        E'#include <iostream>\n#include <vector>\n\nint main() {\n    // Ваш код\n    return 0;\n}',
        'main,return,int,void,#include,iostream,cin,cout,vector,push_back', 
        '#define,goto,asm,__asm__,__asm,scanf,printf,malloc,free', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '3\n10 20 30', '60\n', TRUE),
           (v_task_id, '5\n1 1 1 1 1', '5\n', TRUE);

    -- 12.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.2. Итераторы', 'Итераторы — это абстракция указателей для перебора элементов контейнеров. Они позволяют алгоритмам не зависеть от внутренней структуры контейнера.

  std::vector<int>::iterator it = v.begin();
  for (; it != v.end(); ++it) {
      std::cout << *it << " ";
  }

С выходом C++11 появился range-based for loop, который делает этот код намного чище:
  for (int x : v) {
      std::cout << x << " ";
  }
Если вы хотите изменять элементы, используйте ссылку: `for (int& x : v)`');

    -- 12.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.3. Алгоритмы', 'Библиотека <algorithm> содержит множество готовых функций (sort, find, transform, accumulate) для работы с контейнерами через итераторы.

  #include <algorithm>
  std::sort(v.begin(), v.end()); // Сортировка по возрастанию

Вы также можете передать свой функтор или лямбду третьим аргументом для кастомной сортировки:
  std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; }); // По убыванию');
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 12.3
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Сортировка вектора', 
        'Считайте число N, затем N целых чисел в `std::vector`. Отсортируйте вектор по возрастанию, используя `std::sort` из `<algorithm>`, и выведите элементы через пробел.

Формат ввода:
Число N (до 1000). Затем N чисел.

Формат вывода:
Те же числа, отсортированные по возрастанию, разделенные пробелами.

Пример:
Ввод: 4\n5 1 4 2
Вывод: 1 2 4 5', 
        E'#include <iostream>\n#include <vector>\n#include <algorithm>\n\nint main() {\n    // Ваш код\n    return 0;\n}',
        'main,return,int,void,#include,iostream,cin,cout,vector,algorithm,sort', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '4\n5 1 4 2', '1 2 4 5 \n', TRUE),
           (v_task_id, '3\n-1 -5 0', '-5 -1 0 \n', TRUE);

    -- 12.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.4. Ассоциативные контейнеры', 'Эти контейнеры обеспечивают быстрый поиск по ключу.
1. `std::map<K, V>` и `std::set<K>` — основаны на красно-черных деревьях. Элементы всегда отсортированы по ключу. Поиск, вставка и удаление работают за O(log N).
2. `std::unordered_map<K, V>` и `std::unordered_set<K>` (из C++11) — основаны на хэш-таблицах. Элементы не отсортированы, но доступ к ним осуществляется в среднем за O(1).

Пример словаря:
  std::map<std::string, int> ages;
  ages["Alice"] = 25;
  ages["Bob"] = 30;');
    
    -- 12.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.5. fstream', 'Файловые потоки ifstream (чтение) и ofstream (запись) для работы с текстовыми и бинарными файлами. Они используют тот же синтаксис (<< и >>), что и std::cout / std::cin.

  #include <fstream>
  std::ofstream out("file.txt");
  if (out.is_open()) {
      out << "Hello, World!";
      out.close(); // или просто позволить деструктору закрыть файл
  }');

    -- 12.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.6. stringstream и string_view - чем так хороши?', '`std::stringstream` удобен для парсинга и форматирования строк в памяти. Он позволяет читать числа из строки так же легко, как из cin.

`std::string_view` (C++17) — это "окно" в существующую строку (хранит только указатель и длину). Передача string_view по значению невероятно быстра, так как она не копирует саму строку (в отличие от std::string), но при этом имеет тот же удобный интерфейс. По умолчанию всегда передавайте параметры-строки в функции как `std::string_view`, если вам нужно только чтение.');

    -- 12.7
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.7. any, variant, optional', 'Типы из C++17 для современных архитектурных паттернов:
- `std::optional<T>` — безопасная работа с отсутствующими значениями (вместо возврата -1 или nullptr). Имеет методы `has_value()` и `value()`.
- `std::variant<T1, T2...>` — типобезопасный `union`. Может хранить только один из указанных типов в данный момент времени. Для доступа используется `std::get`.
- `std::any` — может хранить абсолютно любой тип (аналог `void*`, но с проверкой типов). Для доступа используется `std::any_cast`.');

END $$;
