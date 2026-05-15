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
(v_mod_id, '8.1. g++/clang++ и первая программа на C++', 'C++ — это компилируемый язык со строгой типизацией. Процесс превращения кода в программу состоит из 4 этапов:
1. **Препроцессинг**: обработка директив `#include` и макросов.
2. **Компиляция**: превращение кода в ассемблер.
3. **Ассемблирование**: создание объектных файлов (`.o` или `.obj`).
4. **Линковка**: сборка всех файлов и библиотек в один исполняемый файл.

Вместо `stdio.h` в C++ используется `<iostream>`. Основные инструменты:
- `std::cout` — поток вывода (buffered).
- `std::endl` — перевод строки + **принудительная очистка буфера** (flush).
- `std::cin` — поток ввода.

```cpp
#include <iostream>
int main() {
    std::cout << "Hello, C++!" << std::endl;
    return 0;
}
```', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '8.2. Новые базовые типы данных', 'В C++ появились типы, делающие код безопаснее и выразительнее:

1. **`bool`**: Логический тип (`true`/`false`). Занимает 1 байт.
2. **`nullptr`**: (C++11) Специальный тип для "нулевого указателя". В отличие от макроса `NULL` (который просто 0), `nullptr` нельзя случайно передать в функцию, ожидающую `int`. Это повышает безопасность при перегрузке функций.
3. **`auto`**: Автоматический вывод типа компилятором.
```cpp
auto x = 10; // int
auto y = 5.5f; // float
```
Используйте `auto`, когда тип очевиден или слишком сложен (например, для итераторов), но не злоупотребляйте им.', 2)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '8.3. Пространства имен и using', 'Пространства имен (`namespace`) предотвращают коллизии имен в больших проектах.
Вся стандартная библиотека C++ живет в пространстве `std`. Чтобы вызвать функцию, мы используем оператор разрешения области видимости `::` — например, `std::cout`.

**Конструкция `using namespace std;`**:
Она позволяет не писать префикс `std::`. Однако это **плохая практика** для использования в заголовочных файлах (`.h`), так как она "отравляет" глобальное пространство имен всех файлов, которые подключат ваш заголовок.

Вместо `typedef` в современном C++ используется `using`:
```cpp
using Score = int; // Псевдоним типа
```', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '8.4. Ссылки — это не указатели', 'Ссылка (`&`) — это "второе имя" для уже существующей переменной.
**Отличия от указателей**:
1. Ссылку нельзя создать "в пустоту" (нет `NULL` ссылки).
2. Ссылку нельзя переназначить на другой объект после инициализации.
3. Синтаксис не требует разыменования (`*`).

```cpp
int a = 10;
int &ref = a; // ref и a — это одно и то же место в памяти
ref = 20; // a теперь тоже 20
```

**Главное применение**: передача больших объектов в функции без копирования. 
`void process(const LargeObject& obj)` — стандартный способ передачи данных в C++ (быстро как указатель, удобно как значение, безопасно из-за `const`).', 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 8.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Возведение в квадрат (по ссылке)', 
        'Напишите функцию `void square(int &num)`, которая возводит переданное число в квадрат. Используйте ссылку, чтобы изменить оригинал. Для ввода и вывода используйте `std::cin` и `std::cout` вместо `scanf` / `printf`.

Формат ввода:
Одно целое число.

Формат вывода:
Квадрат этого числа.

Пример:
Ввод: 5
Вывод: 25', 
        E'#include <iostream>\n\n// Напишите функцию square здесь\n\nint main() {\n    int n;\n    if (std::cin >> n) {\n        square(n);\n        std::cout << n << "\\n";\n    }\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,scanf,printf,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', '25', TRUE),
           (v_task_id, '-4', '16', TRUE);

    -- Lesson 8.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '8.5. const и constexpr', 'C++ помешан на константности. Это помогает компилятору оптимизировать код, а программисту — избегать ошибок.

- **`const`**: Обещание не изменять значение в рантайме.
- **`constexpr`**: (C++11) Указание вычислить значение **во время компиляции**. Это позволяет использовать результат там, где требуются константы времени компиляции (например, размеры массивов).

```cpp
constexpr int get_size() { return 10 * 5; }
int arr[get_size()]; // Вычисляется на этапе сборки!
```', 5)
    RETURNING id INTO v_lesson_id;

    -- Lesson 8.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '8.6. Операторы new и delete', 'В C++ мы не используем `malloc`/`free`. Для работы с кучей (heap) есть операторы `new` и `delete`.

**Почему это важно?**
- `new` выделяет память **И** вызывает конструктор объекта.
- `delete` вызывает деструктор **И** освобождает память.

```cpp
int *ptr = new int(42); 
delete ptr;

int *arr = new int[100]; // Массив
delete[] arr; // Важно: для массивов всегда используйте delete[]!
```
Неправильное использование `delete` вместо `delete[]` приведет к неопределенному поведению и утечкам памяти.', 6)
    RETURNING id INTO v_lesson_id;

END $$;
