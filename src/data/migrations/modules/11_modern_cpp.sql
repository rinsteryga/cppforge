SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 11: Современный подход...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 11: Современный подход', 'Шаблоны, обработка исключений и новые стандарты C++.')
    RETURNING id INTO v_mod_id;

    -- 11.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '11.1. Шаблоны (Templates)', 'Шаблоны позволяют писать обобщенный код. Вы создаете "каркас", а компилятор генерирует конкретные функции для каждого используемого типа.

**Особенности**:
1. **Инстанцирование**: код шаблона генерируется только тогда, когда вы его вызываете с конкретным типом.
2. **Binary Bloat**: злоупотребление шаблонами может сильно увеличить размер исполняемого файла.
3. **Тьюринг-полнота**: метапрограммирование на шаблонах позволяет выполнять сложные вычисления прямо во время компиляции.

```cpp
template <typename T>
T add(T a, T b) { return a + b; }

int res = add<int>(5, 10); // Явное указание
auto res2 = add(5.5, 3.1); // Автовывод типа (CTAD)
```', 1)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 11.1
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Шаблон функции', 
        'Напишите шаблонную функцию `T get_min(T a, T b)`, которая возвращает минимальное из двух значений. Функция `main` проверит ее работу на целых и вещественных числах.

Формат ввода:
Четыре числа (два целых, два вещественных).

Формат вывода:
Два числа — минимальное из первой пары и минимальное из второй пары.

Пример:
Ввод: 10 5 3.14 2.71
Вывод: 5
2.71', 
        E'#include <iostream>\n\n// Напишите шаблон get_min здесь\n\nint main() {\n    int i1, i2;\n    double d1, d2;\n    if (std::cin >> i1 >> i2 >> d1 >> d2) {\n        std::cout << get_min(i1, i2) << "\\n";\n        std::cout << get_min(d1, d2) << "\\n";\n    }\n    return 0;\n}',
        'template,typename', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '10 5 3.14 2.71', E'5\n2.71', TRUE),
           (v_task_id, '-1 -5 0.5 0.9', E'-5\n0.5', TRUE);

    -- 11.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '11.2. Исключения и noexcept', 'Исключения — это способ обработки ошибок "в обход" возвращаемых значений.
 
 **Механизм**:
 1. `throw`: выбрасывает объект ошибки.
 2. `try-catch`: ловит её.
 
 **Раскрутка стека (Stack Unwinding)**: При выбросе исключения C++ гарантированно вызывает деструкторы всех локальных объектов на пути к `catch`. Это делает исключения безопасными при использовании RAII.
 
 **`noexcept`**: Гарантия того, что функция не бросает исключений. Это критически важно для оптимизаций (например, `std::vector` может перемещать объекты вместо копирования только если их конструктор перемещения помечен `noexcept`).', 2);

    -- 11.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '11.3. Атрибуты (Attributes)', 'Атрибуты в `[[ ]]` — это подсказки компилятору, которые не меняют логику программы, но помогают находить ошибки.
 
 **Полезные атрибуты**:
 - `[[nodiscard]]`: Компилятор выдаст предупреждение, если результат функции проигнорирован. Полезно для функций, возвращающих код ошибки.
 - `[[maybe_unused]]`: Подавляет предупреждение о неиспользуемой переменной.
 - `[[deprecated]]`: Помечает функцию как устаревшую.
 - `[[fallthrough]]`: Сообщает, что отсутствие `break` в `switch` — это осознанный выбор программиста.', 3);

    -- 11.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '11.4. namespace std и плохие практики', 'Стандартная библиотека C++ (STL) целиком находится в пространстве имен std. 
Мы часто пишем `using namespace std;` в начале файлов (.cpp), чтобы сэкономить время на наборе текста.

Однако это считается плохой практикой в серьезном (production) коде. 
Почему? Потому что пространство `std` огромно! Если вы подключите `<algorithm>` и напишете `using namespace std;`, а затем создадите свою функцию `min()`, `count()` или `sort()`, возникнет конфликт имен (Name Clash), который очень тяжело отлаживать.
Ещё хуже — писать `using namespace std;` в заголовочных файлах (.h или .hpp). Это "отравит" глобальное пространство имен для всех других файлов, которые заинклудят ваш заголовок!', 4);

END $$;
