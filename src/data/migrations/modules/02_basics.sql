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
    (v_mod_id, '2.1. Базовые типы данных', 'Программа оперирует данными. В C каждая переменная должна иметь определенный тип, который указывает компилятору, сколько памяти выделить и как интерпретировать биты.

Основные типы данных:
- Целочисленные: char (обычно 1 байт), short (обычно 2 байта), int (обычно 4 байта), long long (обычно 8 байт).
- Вещественные (с плавающей точкой): float (4 байта), double (8 байт).

Модификаторы знака: unsigned (без знака, только неотрицательные) и signed (со знаком). По умолчанию целочисленные типы имеют знак.
Например:
  int a = -10;
  unsigned int b = 42;
  double pi = 3.14159;

Важно понимать: размер типов в C не фиксирован строго стандартом, он зависит от архитектуры (Data Model, например LP64 или ILP32). Гарантируется лишь соотношение: sizeof(char) == 1, sizeof(short) <= sizeof(int) <= sizeof(long). При выборе типа всегда помните про возможность переполнения (integer overflow), если значение превысит лимит памяти типа!', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 2.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '2.2. Арифметика и логика', 'В C присутствуют стандартные арифметические операторы: +, -, *, /, % (остаток от деления).
Внимание: деление целого числа на целое дает ЦЕЛОЕ число (дробная часть отбрасывается).
Пример: 5 / 2 даст 2. Чтобы получить 2.5, нужно использовать вещественные числа: 5.0 / 2.0.

Логические операторы используются для условий:
- && (логическое И)
- || (логическое ИЛИ)
- ! (логическое НЕ)

Операторы сравнения: == (равно), != (не равно), >, <, >=, <=.
Обратите внимание: оператор присваивания = отличается от проверки на равенство ==. Это самая частая ошибка новичков!', 2)
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
    (v_mod_id, '2.3. Первый ввод - функция scanf', 'Для чтения данных со стандартного ввода (stdin) используется функция scanf из <stdio.h>.

Пример чтения целого числа:
  int number;
  scanf("%d", &number);

Разбор:
"%d" — это спецификатор формата. Он говорит функции scanf: "ожидай десятичное целое число".
&number — оператор взятия адреса (&). Функция scanf должна знать, КУДА в памяти записать считанное значение. Мы подробнее разберем адреса в модуле про указатели, но пока запомните: при считывании базовых типов через scanf перед переменной нужно ставить & (амперсанд).

Для double используется "%lf" (long float), для char "%c", для float "%f".', 3)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 2.3
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Сумма двух чисел', 
        'Напишите программу, которая считывает два целых числа (через пробел) и выводит их сумму.

Формат ввода:
Два целых числа, разделенных пробелом.

Формат вывода:
Одно целое число — сумма введенных чисел.

Пример:
Ввод: 5 7
Вывод: 12', 
        E'#include <stdio.h>\n\nint main(void) {\n    int a, b;\n    // Ваш код\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf', 
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
    (v_mod_id, '2.4. Условные операторы', 'Условные операторы позволяют программе принимать решения. Основная конструкция — if-else.

  if (условие) {
      // Выполнится, если условие истинно (не равно нулю)
  } else if (другое_условие) {
      // Выполнится, если первое ложно, а это истинно
  } else {
      // Выполнится, если все вышеперечисленные ложны
  }

В C нет логического типа данных (bool появился в C99 через <stdbool.h>, но на низком уровне это всё равно числа). Любое ненулевое значение считается истиной (true), а 0 — ложью (false).', 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 2.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Високосный год', 
        'Напишите программу, которая проверяет, является ли введенный год високосным. Год високосный, если он кратен 4, но не кратен 100, либо кратен 400.
Выведите "YES", если год високосный, и "NO", если нет.

Формат ввода:
Одно целое положительное число (год).

Формат вывода:
Строка YES или NO.

Пример:
Ввод: 2024
Вывод: YES', 
        E'#include <stdio.h>\n\nint main(void) {\n    int year;\n    scanf("%d", &year);\n    // Ваш код\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf,if,else', 
        '#define,while,goto,do,asm,__asm__,__asm,stdlib.h,struct,class,*,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '2024', 'YES', TRUE),
           (v_task_id, '1900', 'NO', TRUE),
           (v_task_id, '2000', 'YES', TRUE),
           (v_task_id, '2023', 'NO', TRUE);

    -- Lesson 2.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '2.5. Тернарный оператор', 'Тернарный условный оператор ?: — это сокращенная форма if-else, которая возвращает значение.

Синтаксис: условие ? значение_если_истина : значение_если_ложь;

Пример поиска максимума:
  int max = (a > b) ? a : b;

Используйте его для простых выражений. Не стоит злоупотреблять вложенными тернарными операторами — это сильно ухудшает читаемость кода.', 5)
    RETURNING id INTO v_lesson_id;

    -- Lesson 2.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '2.6. Циклы...', 'Циклы позволяют выполнять блок кода многократно.
В C есть три вида циклов:
1. Цикл while: выполняется, пока условие истинно.
  while (n > 0) {
      printf("%d ", n);
      n--;
  }

2. Цикл do-while: сначала выполняет тело цикла, затем проверяет условие (гарантирует хотя бы одно выполнение).
  do {
      // тело
  } while (условие);

3. Цикл for: идеален для счетчиков.
  for (инициализация; условие; инкремент) {
      // тело
  }
  for (int i = 0; i < 10; ++i) {
      printf("%d ", i);
  }

Ключевое слово break досрочно прерывает выполнение цикла. Ключевое слово continue переходит к следующей итерации, пропуская оставшийся код в текущей.', 6)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 2.6
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Сумма от 1 до N', 
        'Напишите программу, которая считывает натуральное число N и выводит сумму всех целых чисел от 1 до N включительно. Используйте цикл.

Формат ввода:
Одно целое положительное число N (до 10^4).

Формат вывода:
Одно целое число — искомая сумма.

Пример:
Ввод: 5
Вывод: 15', 
        E'#include <stdio.h>\n\nint main(void) {\n    int n;\n    scanf("%d", &n);\n    \n    // Ваш код\n    \n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf', 
        '#define,goto,asm,__asm__,__asm,struct,class,*,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', '15', TRUE),
           (v_task_id, '10', '55', TRUE),
           (v_task_id, '100', '5050', TRUE);

END $$;
