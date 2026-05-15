SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
    v_matching_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: Основы программирования на языке C...';

    INSERT INTO modules (name, description) 
    VALUES ('Основы программирования на языке C', 'Типы данных, операции, условия и циклы')
    RETURNING id INTO v_mod_id;

    -- Lesson 2.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '2.1. Базовые типы данных', $content$C — это язык с "ручным" управлением данными. Тип переменной определяет, как биты в памяти превращаются в числа.

**Примеры размеров**:
```cpp
printf("%zu", sizeof(int));       // Обычно 4
printf("%zu", sizeof(char));      // Всегда 1
printf("%zu", sizeof(double));    // Обычно 8
```

**Переполнение (Overflow)**:
Если в `char` (макс 127) записать 128, вы получите -128. Это "эффект кольца".

**Важно**: Размер типов зависит от архитектуры процессора. Используйте оператор `sizeof(тип)`, чтобы узнать точный размер в байтах на вашей системе.$content$, 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 2.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '2.2. Арифметика и логика', $content$В C есть всё: от сложения до битовых сдвигов.

**Инкремент: разница заметна в выражениях**:
```cpp
int a = 5, b = 5;
int x = ++a; // x = 6, a = 6 (сначала увеличили)
int y = b++; // y = 5, b = 6 (сначала отдали, потом увеличили)
```

**Логика**:
В C нет встроенного типа `bool` (до C99). Любое число `!= 0` — это **ИСТИНА**, а `0` — это **ЛОЖЬ**.
`if (42)` — выполнится. `if (0)` — нет.$content$, 2)
    RETURNING id INTO v_lesson_id;

    -- Quiz for 2.2
    INSERT INTO quizzes (lesson_id, title, question) VALUES (v_lesson_id, 'Целочисленное деление', 'Каков результат вычисления выражения: 7 / 2 ?') RETURNING id INTO v_quiz_id;
    INSERT INTO quiz_options (quiz_id, option_text, is_correct) VALUES 
    (v_quiz_id, '3', TRUE),
    (v_quiz_id, '3.5', FALSE),
    (v_quiz_id, '4', FALSE),
    (v_quiz_id, 'Ошибка компиляции', FALSE);

    -- Lesson 2.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '2.3. Ввод данных: scanf', $content$Функция `scanf` читает данные и записывает их по адресу переменной. Именно поэтому мы ставим оператор `&` (взять адрес).

**Спецификаторы**:
- `%d` — `int`
- `%f` — `float`
- `%lf` — `double` (long float)
- `%c` — `char`

**Профессиональный совет**: `scanf` возвращает количество успешно считанных элементов. Если вы ждете 2 числа, а пользователь ввел текст, `scanf` вернет значение меньше 2. Проверка этого значения — ключ к написанию надежных программ.$content$, 3)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 2.3
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Сумма двух чисел', 
        $description$Напишите программу, которая считывает два целых числа (через пробел) и выводит их сумму.

Формат ввода:
Два целых числа, разделенных пробелом.

Формат вывода:
Одно целое число — сумма введенных чисел.

Пример:
Ввод: 5 7
Вывод: 12$description$, 
        E'#include <stdio.h>\n\nint main(void) {\n    int a, b;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,while,goto,do,asm,__asm__,__asm,stdlib.h,struct,class,*,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5 7', '12', TRUE),
           (v_task_id, '-10 20', '10', TRUE);

    -- Lesson 2.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '2.4. Условный оператор и switch', $content$Оператор `if-else` направляет поток выполнения программы. 

**Пример switch**:
```cpp
int day = 2;
switch (day) {
    case 1: printf("Пн"); break;
    case 2: printf("Вт"); break;
    default: printf("Другой день"); break;
}
```
**Важно**: Если убрать `break`, после "Вт" программа напечатает и "Другой день"! Это называется fall-through.$content$, 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 2.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Високосный год', 
        $description$Напишите программу, которая проверяет, является ли введенный год високосным. Год високосный, если он кратен 4, но не кратен 100, либо кратен 400.
Выведите "YES", если год високосный, и "NO", если нет.

Формат ввода:
Одно целое положительное число (год).

Формат вывода:
Строка YES или NO.

Пример:
Ввод: 2024
Вывод: YES$description$, 
        E'#include <stdio.h>\n\nint main(void) {\n    int year;\n    scanf("%d", &year);\n    // Ваш код\n    return 0;\n}',
        'if,else', 
        '#define,while,goto,do,asm,__asm__,__asm,stdlib.h,struct,class,*,new,delete', 
        2000, 
        136, 
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '2024', 'YES', TRUE),
           (v_task_id, '1900', 'NO', TRUE),
           (v_task_id, '2000', 'YES', TRUE),
           (v_task_id, '2023', 'NO', TRUE);

    -- Lesson 2.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '2.5. Тернарный оператор', $content$Тернарный условный оператор ?: — это сокращенная форма if-else, которая возвращает значение.

Синтаксис: условие ? значение_если_истина : значение_если_ложь;

Пример поиска максимума:
  int max = (a > b) ? a : b;

Используйте его для простых выражений. Не стоит злоупотреблять вложенными тернарными операторами — это сильно ухудшает читаемость кода.$content$, 5)
    RETURNING id INTO v_lesson_id;

    -- Lesson 2.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '2.6. Циклы: заставляем компьютер работать', $content$Циклы — это сердце автоматизации. 

1. **`while`**: Сначала проверяет, потом делает.
2. **`do-while`**: Сначала делает, потом проверяет. Тело выполнится **минимум один раз**.
3. **`for`**: Компактная запись для циклов со счетчиком.

**Управление циклами**:
- `break`: Немедленный выход из цикла.
- `continue`: Пропуск текущей итерации и переход к проверке условия.

**Совет**: Избегайте бесконечных циклов (`while(1)`), если у вас нет четкого условия выхода внутри. Они могут "завесить" вашу программу или систему.$content$, 6)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 2.6
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Сумма от 1 до N', 
        $description$Напишите программу, которая считывает натуральное число N и выводит сумму всех целых чисел от 1 до N включительно. Используйте цикл.

Формат ввода:
Одно целое положительное число N (до 10^4).

Формат вывода:
Одно целое число — искомая сумма.

Пример:
Ввод: 5
Вывод: 15$description$, 
        E'#include <stdio.h>\n\nint main(void) {\n    int n;\n    scanf("%d", &n);\n    \n    // Ваш код\n    \n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', '15', TRUE),
           (v_task_id, '10', '55', TRUE),
           (v_task_id, '100', '5050', TRUE);

END $$;
