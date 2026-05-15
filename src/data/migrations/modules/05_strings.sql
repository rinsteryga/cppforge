SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: C-строки. Массивы символов...';

    INSERT INTO modules (name, description) 
    VALUES ('C-строки. Массивы символов', 'Работа с текстом, нуль-терминированные строки и библиотеки string.h / ctype.h')
    RETURNING id INTO v_mod_id;

    -- Lesson 5.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '5.1. Что такое C-строка?', 'В C нет типа "строка". Есть только массивы символов `char`. 

**Пример с терминатором (не Арни!)**:
```cpp
char s[] = {'H', 'i', '\0', '!', '?'};
printf("%s", s); // Выведет только "Hi"
```
Символы после `\0` игнорируются функциями вывода.

**Опасность**: Если вы забудете про `\0`, функции будут читать память дальше конца строки. Это называется **Buffer Overread**.', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 5.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '5.2. Операции и Ввод', 'Поскольку строка — это массив, её нельзя просто скопировать через `=` или сравнить через `==`. Вы лишь скопируете адреса, а не сам текст.

**Безопасный ввод**:
`scanf("%s", str)` — это **дыра в безопасности**. Если пользователь введет слово длиннее массива, программа "сломается" (Buffer Overflow).
Всегда ограничивайте длину: `scanf("%19s", str);` (для массива из 20 байт, 1 байт всегда оставляем под `\0`).', 2)
    RETURNING id INTO v_lesson_id;

    -- Lesson 5.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '5.3. Литералы и Память', 'Строковый литерал `"Hello"` лежит в защищенной области памяти (ReadOnly).

```cpp
char *p = "Hello"; 
p[0] = ''h''; // ошибка! Попытка записи в ReadOnly память.
```
Если вам нужно изменять строку, инициализируйте ей массив:
```cpp
char arr[] = "Hello"; // Создает КОПИЮ данных на стеке, которую можно менять.
```', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 5.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '5.4. Библиотека <string.h>', 'Основные функции:
- `strlen(s)`: длина.
- `strcmp(s1, s2)`: сравнение.
```cpp
if (strcmp(pass, "1234") == 0) {
    printf("Доступ разрешен");
}
```
- `strncpy(dest, src, n)`: безопасное копирование.

**Внимание**: Вы сами должны гарантировать, что в `dest` достаточно места для `src`.', 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 5.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Собственный strlen', 
        'Напишите функцию int my_strlen(const char *str), которая вычисляет длину строки без использования <string.h>. В функции main реализован ввод строки.

Формат ввода:
Одно слово без пробелов (длина не более 99 символов).

Формат вывода:
Одно целое число — длина строки.

Пример:
Ввод: Hello
Вывод: 5', 
        E'#include <stdio.h>\n\nint my_strlen(const char *str) {\n    // Ваш код\n}\n\nint main(void) {\n    char buffer[100];\n    if (scanf("%99s", buffer) == 1) {\n        printf("%d\\n", my_strlen(buffer));\n    }\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,string.h,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, 'Hello', '5', TRUE),
           (v_task_id, 'CPP', '3', TRUE);

    -- Lesson 5.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '5.5. <ctype.h>', 'Для работы с одиночными символами есть полезная библиотека <ctype.h>.
Она предоставляет функции-предикаты и функции конвертации:
- isalpha(c) — проверяет, является ли символ буквой.
- isdigit(c) — является ли цифрой.
- isspace(c) — является ли пробельным символом (пробел, табуляция, \n и т.д.).
- toupper(c) / tolower(c) — переводит символ в верхний/нижний регистр (если это буква).

Нюанс: функции <ctype.h> могут вести себя странно при работе с кириллицей (и другими мультибайтными кодировками, такими как UTF-8), если не настроена локаль через setlocale(). В современном C++ для этого используют другие механизмы, а в C для UTF-8 лучше применять специализированные библиотеки, например, ICU.', 5)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 5.5
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Переворот строки (Reverse)', 
        'Напишите функцию void my_reverse(char *str), которая переворачивает строку на месте (in-place) без создания новых массивов. Использовать string.h запрещено! Функция main уже написана за вас.

Формат ввода:
Одно слово (без пробелов).

Формат вывода:
То же слово, записанное задом наперед.

Пример:
Ввод: Hello
Вывод: olleH', 
        E'#include <stdio.h>\n\nvoid my_reverse(char *str) {\n    // Ваш код\n}\n\nint main(void) {\n    char buffer[100];\n    if (scanf("%99s", buffer) == 1) {\n        my_reverse(buffer);\n        printf("%s\\n", buffer);\n    }\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,string.h,strlen,strrev,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, 'Hello', 'olleH', TRUE),
           (v_task_id, 'racecar', 'racecar', TRUE),
           (v_task_id, '12345', '54321', TRUE);

END $$;
