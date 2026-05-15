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
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '12.1. Контейнеры', 'STL предоставляет богатый набор структур данных. Основные категории:
1. **Последовательные**: `std::vector` (динамический массив), `std::list` (двусвязный список), `std::deque` (очередь с двумя концами).
2. **Ассоциативные**: `std::map`, `std::set`.
3. **Адаптеры**: `std::stack`, `std::queue`.

Король контейнеров — `std::vector<T>`. Это обёртка над сырым динамическим массивом. 
**Ключевые особенности**:
- Доступ по индексу: **O(1)**.
- Добавление в конец: **O(1)** (амортизированное).
- Память всегда лежит одним сплошным блоком, что очень "дружелюбно" к кэшу процессора.

Совет: используйте `v.reserve(N)`, если заранее знаете примерный размер. Это избавит вектор от лишних переаллокаций и копирований при расширении.

```cpp
#include <vector>
std::vector<int> v = {1, 2, 3};
v.push_back(4); // Добавление в конец
std::cout << "Размер: " << v.size() << ", Емкость: " << v.capacity();
```', 1)
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
Ввод: 3 10 20 30
Вывод: 60', 
        E'#include <iostream>\n#include <vector>\n\nint main() {\n    // Ваш код\n    return 0;\n}',
        'vector', 
        '#define,goto,asm,__asm__,__asm,scanf,printf,malloc,free', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, E'3 10 20 30', '60', TRUE),
           (v_task_id, E'5 1 1 1 1 1', '5', TRUE);

    -- 12.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '12.2. Итераторы', 'Итераторы — это мост между контейнерами и алгоритмами. Они работают как обобщенные указатели. Любой контейнер STL предоставляет метод `begin()` (указывает на первый элемент) и `end()` (указывает на позицию **ЗА** последним элементом). Этот интервал `[begin, end)` называется полуоткрытым.

```cpp
std::vector<int>::iterator it = v.begin();
for (; it != v.end(); ++it) {
    std::cout << *it << " "; // Разыменовываем итератор как указатель
}
```

В современном C++ (C++11+) мы используем `range-based for`:
```cpp
for (const auto& x : v) { // const& — чтобы не копировать данные
    std::cout << x << " ";
}
```
Типы итераторов бывают разными: от простых (Forward) до продвинутых (Random Access, как у вектора), которые позволяют прыгать сразу на N элементов вперед: `it + 5`.', 2);

    -- 12.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '12.3. Алгоритмы', 'Философия STL: "Алгоритмы отделены от данных". Библиотека `<algorithm>` содержит более 100 функций, которые принимают итераторы.
Основные категории:
- **Немодифицирующие**: `std::find`, `std::count`, `std::all_of`.
- **Модифицирующие**: `std::sort`, `std::reverse`, `std::transform` (аналог map из других языков).

```cpp
#include <algorithm>
#include <vector>

std::sort(v.begin(), v.end()); // Быстрая сортировка O(N log N)
auto it = std::find(v.begin(), v.end(), 42); // Линейный поиск O(N)
```

Сила алгоритмов — в предикатах. Вы можете передать лямбду, чтобы изменить поведение:
```cpp
// Сортировка по убыванию
std::sort(v.begin(), v.end(), [](int a, int b) { 
    return a > b; 
});
```', 3)
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
Ввод: 4 5 1 4 2
Вывод: 1 2 4 5', 
        E'#include <iostream>\n#include <vector>\n#include <algorithm>\n\nint main() {\n    // Ваш код\n    return 0;\n}',
        'vector,algorithm,sort', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, E'4 5 1 4 2', '1 2 4 5', TRUE),
           (v_task_id, E'3 -1 -5 0', '-5 -1 0', TRUE);

    -- 12.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '12.4. Ассоциативные контейнеры', 'Когда нужен поиск быстрее, чем O(N), на сцену выходят ассоциативные контейнеры:

1. **`std::map` / `std::set`**: Реализованы как сбалансированные деревья (обычно красно-черные). 
   - Поиск/вставка: **O(log N)**.
   - Элементы всегда отсортированы по ключу.
2. **`std::unordered_map` / `std::unordered_set`**: Хэш-таблицы.
   - Поиск/вставка: **O(1)** в среднем.
   - Элементы не имеют порядка. Требуют наличия хэш-функции для ключа.

Пример:
```cpp
#include <map>
#include <string>

std::map<std::string, int> scores;
scores["Player1"] = 100; // Если ключа нет, он будет создан
if (scores.count("Player1")) {
    std::cout << "Нашли!";
}
```', 4);
    
    -- 12.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '12.5. fstream и потоки', 'Работа с файлами в C++ строится на иерархии потоков. Основные классы: `ifstream` (Input), `ofstream` (Output) и `fstream` (оба сразу).
Они работают по принципу RAII: файл открывается в конструкторе и **автоматически закрывается** в деструкторе, когда объект выходит из области видимости.

```cpp
#include <fstream>
#include <string>

std::ofstream out("data.txt");
out << "Score: " << 500 << "\n"; // Запись как в std::cout

std::ifstream in("data.txt");
std::string line;
if (std::getline(in, line)) { // Чтение целой строки
    std::cout << "Прочитали: " << line;
}
```', 5);

    -- 12.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '12.6. stringstream и string_view', 'Эти два инструмента радикально упрощают работу со строками.

**`std::stringstream`** — позволяет рассматривать строку как поток ввода/вывода. Идеально для сложного парсинга.
```cpp
std::stringstream ss("42 3.14");
int i; double d;
ss >> i >> d; // Вытащили числа из строки!
```

**`std::string_view`** (C++17) — это некопирующий "срез" строки. Он хранит только указатель на начало и длину.
- Передача `std::string` в функцию вызывает копирование (дорого).
- Передача `std::string_view` — это передача двух чисел (быстро).
**Золотое правило**: если функция только читает строку, принимайте её по `std::string_view`.', 6);

    -- 12.7
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '12.7. Типы-помощники: optional, variant, any', 'Современный C++ (17) добавил типы, которые делают код чище и безопаснее:

- **`std::optional<T>`**: используется, когда функция может не вернуть значение. Больше никаких "магических чисел" вроде -1 или `nullptr`.
```cpp
std::optional<int> find_id(std::string name) {
    if (name == "Admin") return 0;
    return std::nullopt; // Ничего не нашли
}
```
- **`std::variant<T1, T2...>`**: типобезопасное объединение (`union`). Может хранить один из перечисленных типов.
- **`std::any`**: контейнер для значения абсолютно любого типа. Используется редко, когда нужна максимальная динамика.', 7);

END $$;
