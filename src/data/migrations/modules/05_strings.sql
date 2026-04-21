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
    (v_mod_id, '5.1. C-строки. ''\0''', 'В языке C нет встроенного типа "строка" (как string в Python или C++). Строка в C — это просто одномерный массив символов (char), который обязательно заканчивается специальным нуль-терминатором: символом с ASCII-кодом 0, записываемым как ''\0''.

Нюанс: символ ''0'' (цифра ноль) имеет код 48, а терминатор ''\0'' имеет код 0. Это критически важное различие!

Благодаря терминатору функции из стандартной библиотеки знают, где заканчивается строка, даже если размер самого массива больше длины строки. 
Если вы забудете добавить ''\0'' в конец массива символов, и передадите его в функцию вроде printf("%s", ...), она будет читать память дальше границ массива, пока случайно не встретит нулевой байт. Это классическая уязвимость (Buffer Overread), приводящая к чтению "мусора" или падению программы (Segmentation fault).', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 5.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '5.2. Базовые операции со строками', 'Поскольку строки — это массивы, мы не можем применять к ним операторы присваивания (==, =).
Например, нельзя написать:
  char str[10];
  str = "Hello"; // ОШИБКА КОМПИЛЯЦИИ! Массивам нельзя переприсваивать адреса.

Ввод строк через scanf("%s", str) тоже имеет нюансы: scanf читает только до первого пробела (или символа табуляции/переноса строки). Кроме того, если слово длиннее размера массива, произойдет переполнение буфера (Buffer Overflow). Чтобы этого избежать, всегда указывайте максимальную ширину: scanf("%19s", str); (оставив 1 байт под ''\0'').

Для чтения целой строки с пробелами раньше использовали функцию gets(), но она была признана опасной и удалена из стандарта C11! Вместо нее следует использовать fgets() (мы разберем ее в модуле про файлы).', 2)
    RETURNING id INTO v_lesson_id;

    -- Lesson 5.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '5.3. Строковые литералы', 'Строковый литерал (например, "Hello") — это константный массив символов, который компилятор размещает в секции памяти "только для чтения" (обычно .rodata).

Нюанс:
  char arr[] = "Hello"; // Создает копию литерала на стеке. Вы можете менять arr[0] = ''h''.
  char *ptr = "Hello";  // Указывает напрямую на read-only память.

Если вы попытаетесь изменить строку по указателю ptr (ptr[0] = ''h''), программа упадет с ошибкой Segmentation fault, потому что ОС запретит запись в защищенную секцию памяти. Поэтому строковые литералы через указатель всегда следует помечать как const: const char *ptr = "Hello";', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 5.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '5.4. <string.h>', 'Для работы со строками в стандарте есть заголовочный файл <string.h>.
Самые важные функции (и их безопасные аналоги):
- strlen(str): возвращает длину строки (количество символов до ''\0'').
- strcpy(dest, src): копирует строку src в буфер dest. Опасна переполнением.
- strncpy(dest, src, n): копирует не более n символов. Нюанс: если длина src больше или равна n, терминатор ''\0'' НЕ будет добавлен автоматически!
- strcmp(s1, s2): сравнивает две строки лексикографически. Возвращает 0, если строки равны.

Большинство функций string.h написаны очень эффективно (с использованием ассемблерных оптимизаций и векторных инструкций), поэтому "изобретать велосипед" не стоит, за исключением учебных целей.', 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 5.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Собственный strlen', 
        'Напишите функцию int my_strlen(const char *str), которая вычисляет длину строки без использования <string.h>. В функции main реализован ввод строки.', 
        E'#include <stdio.h>\n\nint my_strlen(const char *str) {\n    // Ваш код\n}\n\nint main(void) {\n    char buffer[100];\n    if (scanf("%99s", buffer) == 1) {\n        printf("%d\\n", my_strlen(buffer));\n    }\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf', 
        '#define,goto,asm,__asm__,__asm,strlen,string.h', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, 'Hello', '5\n', TRUE),
           (v_task_id, 'CPP', '3\n', TRUE);

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

END $$;
