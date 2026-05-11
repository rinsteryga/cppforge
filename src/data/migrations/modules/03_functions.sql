SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: Функции...';

    INSERT INTO modules (name, description) 
    VALUES ('Функции', 'Объявление, определение, параметры и директивы препроцессора')
    RETURNING id INTO v_mod_id;

    -- Lesson 3.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '3.1. Declaration. Definition. Возврат значений из функций', 'Функция — это обособленный блок кода, решающий определенную задачу. Использование функций позволяет избежать дублирования (принцип DRY - Don''t Repeat Yourself).

В C важно различать объявление (Declaration) и определение (Definition).
Объявление (сигнатура, прототип) лишь сообщает компилятору о существовании функции:
  int add(int a, int b);

Определение содержит само тело функции:
  int add(int a, int b) {
      return a + b;
  }

Компилятор языка C читает файл сверху вниз. Если вы вызываете функцию до её определения, вы обязаны предоставить её объявление выше места вызова, иначе компилятор выдаст предупреждение или ошибку.', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 3.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '3.2. Параметры функций. Области видимости.', 'Функции могут принимать параметры. В C аргументы передаются ПО ЗНАЧЕНИЮ (by value). Это значит, что функция получает копию переменной. Изменение этой копии внутри функции никак не повлияет на оригинальную переменную.

  void foo(int x) {
      x = 42; // Оригинал не изменится!
  }

Область видимости (Scope) переменной, объявленной внутри функции (включая параметры), ограничена блоком { } этой функции. Такие переменные называются локальными. Жизненный цикл локальной переменной завершается при выходе из функции — память, выделенная под нее на стеке (Stack), освобождается.', 2)
    RETURNING id INTO v_lesson_id;

    -- Quiz
    INSERT INTO quizzes (lesson_id, title, question) VALUES (v_lesson_id, 'Передача параметров', 'Что произойдет с переменной "a", если мы передадим её в функцию foo(a), которая внутри себя умножает аргумент на 10?') RETURNING id INTO v_quiz_id;
    INSERT INTO quiz_options (quiz_id, option_text, is_correct) VALUES 
    (v_quiz_id, 'Останется неизменной', TRUE),
    (v_quiz_id, 'Умножится на 10', FALSE),
    (v_quiz_id, 'Вызовет ошибку сегментации', FALSE),
    (v_quiz_id, 'Превратится в глобальную переменную', FALSE);

    -- Lesson 3.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '3.3. Рекурсия и стек вызовов', 'Функция может вызывать саму себя. Этот процесс называется рекурсией.
Важное правило рекурсии: всегда должно быть базовое условие (условие выхода), иначе функция будет вызывать себя бесконечно.

Каждый вызов функции создает новый кадр (frame) в специальной области памяти, называемой стеком вызовов (Call Stack). В кадре хранятся локальные переменные этого вызова. Если рекурсия будет бесконечной (или слишком глубокой), стек переполнится, и программа упадет с ошибкой Stack Overflow.', 3)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 3.3
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Числа Фибоначчи', 
        'Напишите рекурсивную функцию int fib(int n), которая возвращает n-е число Фибоначчи.
F(0) = 0, F(1) = 1.
F(n) = F(n-1) + F(n-2) для n >= 2.
Функция main написана за вас.

Формат ввода:
Одно целое число N (от 0 до 30).

Формат вывода:
Одно целое число — N-е число Фибоначчи.

Пример:
Ввод: 5
Вывод: 5', 
        E'#include <stdio.h>\n\n// Напишите функцию fib здесь\n\nint main(void) {\n    int n;\n    scanf("%d", &n);\n    printf("%d\\n", fib(n));\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf', 
        '#define,goto,asm,__asm__,__asm,struct,class,*,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', '5\n', TRUE),
           (v_task_id, '0', '0\n', TRUE),
           (v_task_id, '10', '55\n', TRUE);

    -- Lesson 3.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '3.4. Директивы препроцессора. #define', 'До начала компиляции исходный код обрабатывается препроцессором. Директивы препроцессора начинаются с символа #.

Мы уже знаем #include, который буквально копирует содержимое заголовочного файла в наш код.
Другая важная директива — #define. Она используется для определения макросов (текстовой замены).

  #define PI 3.14159
  #define MAX_SIZE 100

Везде, где препроцессор встретит слово PI, он тупо заменит его на 3.14159 еще до того, как компилятор начнет анализ синтаксиса.
Макросы могут принимать аргументы, но с ними нужно быть очень осторожными из-за отсутствия проверки типов и возможных побочных эффектов:
  #define SQUARE(x) ((x) * (x))

Современный стиль C++ (с которым мы познакомимся позже) рекомендует избегать #define в пользу констант и inline функций.', 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 3.1-3.2
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Собственная функция', 
        'Определите функцию int is_even(int num), которая возвращает 1, если число четное, и 0, если нечетное. Функция main уже написана за вас (она считывает число и выводит результат работы is_even). Вам нужно только написать функцию.

Формат ввода:
Одно целое число.

Формат вывода:
Число 1, если четное, иначе 0.

Пример:
Ввод: 4
Вывод: 1', 
        E'#include <stdio.h>\n\n// Напишите функцию is_even здесь\n\nint main(void) {\n    int n;\n    if (scanf("%d", &n) == 1) {\n        printf("%d\\n", is_even(n));\n    }\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf', 
        '#define,goto,asm,__asm__,__asm,struct,class,*,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '4', '1\n', TRUE),
           (v_task_id, '7', '0\n', TRUE);

END $$;
