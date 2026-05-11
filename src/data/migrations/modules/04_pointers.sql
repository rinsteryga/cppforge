SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: Указатели - фундамент. Массивы...';

    INSERT INTO modules (name, description) 
    VALUES ('Указатели - фундамент. Массивы', 'Понимание памяти, адресов и массивов')
    RETURNING id INTO v_mod_id;

    -- Lesson 4.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '4.1. Понятие адреса. Операторы "&" и "*"', 'Память компьютера можно представить как огромный массив байтов, где у каждого байта есть свой уникальный порядковый номер — адрес. 
Когда вы объявляете переменную (например, int x = 10;), компилятор и ОС выделяют под неё ячейки памяти.

В C мы можем узнать адрес любой переменной с помощью оператора взятия адреса "&":
  printf("Адрес переменной x: %p\n", (void*)&x);

Оператор разыменования "*" (звездочка) делает обратное: имея адрес памяти, он позволяет прочитать или записать значение по этому адресу. Это называется "обращение по указателю".', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 4.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '4.2. Указатели на базовые типы данных', 'Указатель — это переменная, которая хранит адрес другой переменной.
Объявление указателя:
  int *ptr; // ptr — это указатель на тип int

Свяжем указатель с переменной:
  int var = 42;
  int *p = &var;

Теперь p содержит адрес var. Если мы напишем *p = 100;, то значение var изменится на 100! 
Размер самого указателя не зависит от того, на что он указывает. На 64-битной архитектуре любой указатель занимает 8 байт, так как адрес представляет собой 64-битное число.', 2)
    RETURNING id INTO v_lesson_id;

    -- Quiz
    INSERT INTO quizzes (lesson_id, title, question) VALUES (v_lesson_id, 'Изменение по указателю', 'Если int a = 5; int *p = &a; *p = 20; чему будет равно a?') RETURNING id INTO v_quiz_id;
    INSERT INTO quiz_options (quiz_id, option_text, is_correct) VALUES 
    (v_quiz_id, '5', FALSE),
    (v_quiz_id, '20', TRUE),
    (v_quiz_id, 'Неопределенное поведение', FALSE),
    (v_quiz_id, 'Адресу памяти', FALSE);

    -- Lesson 4.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '4.3. Одномерные статические массивы и их связь с указателями.', 'Массив — это непрерывная область памяти, хранящая элементы одного типа.
  int arr[5] = {10, 20, 30, 40, 50};

В C имя массива (arr) при большинстве операций "распадается" (decays) в указатель на его первый элемент (&arr[0]).
Это означает, что массивы и указатели тесно связаны. Однако массив — это не указатель! У массива есть фиксированный размер на этапе компиляции, и вы не можете "перенаправить" имя массива на другой участок памяти.', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 4.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '4.4. Адресная арифметика и void*', 'К указателям можно прибавлять и отнимать целые числа. 
Если p указывает на элемент массива, то p + 1 будет указывать не на следующий байт, а на следующий ЭЛЕМЕНТ. Компилятор сам умножает прибавляемое число на размер типа (sizeof).

Поэтому следующие записи абсолютно эквивалентны:
  arr[3]
  *(arr + 3)

Оператор индексации [] — это просто синтаксический сахар для адресной арифметики.

Специальный тип указателя `void*` используется для хранения адреса без информации о типе данных. К нему нельзя применять адресную арифметику или разыменовывать без предварительного приведения (cast) к конкретному типу указателя.', 4)
    RETURNING id INTO v_lesson_id;

    -- Lesson 4.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '4.5. Передача массивов в функции', 'Так как массивы распадаются в указатели, при передаче массива в функцию он передается как указатель на первый элемент. Размер массива при этом теряется!

Поэтому в C функции, принимающие массивы, обычно требуют передачи размера отдельным аргументом:
  void print_array(int *arr, int size) { ... }
  // или эквивалентно:
  void print_array(int arr[], int size) { ... }

Поскольку функция получает указатель на оригинальные данные, изменения массива внутри функции отразятся на оригинале. Это исключение из правила "передачи по значению" — по значению передается лишь АДРЕС (сам указатель), но память, на которую он указывает, остается общей.', 5)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 4.5
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Поиск максимума в массиве', 
        'Напишите программу, которая считывает число N, затем N целых чисел в массив (N гарантированно <= 100), и выводит максимальное из них.

Формат ввода:
Одно целое число N (до 100), затем N целых чисел, разделенных пробелами.

Формат вывода:
Одно целое число — максимальное значение.

Пример:
Ввод: 5\n10 -5 42 0 7
Вывод: 42', 
        E'#include <stdio.h>\n\nint main(void) {\n    int n;\n    if (scanf("%d", &n) != 1) return 0;\n    int arr[100];\n    // Ваш код\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf', 
        '#define,goto,asm,__asm__,__asm,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5\n10 -5 42 0 7\n', '42\n', TRUE),
           (v_task_id, '3\n-1 -2 -3\n', '-1\n', TRUE);

    -- Lesson 4.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '4.6. Многомерные массивы', 'Многомерный массив — это "массив массивов".
В памяти двумерный массив int matrix[3][4] располагается линейно, строка за строкой.

Индексация происходит двумя парами скобок: matrix[row][col].
При передаче двумерного массива в функцию компилятору необходимо знать длину всех измерений, кроме первого, чтобы правильно вычислять смещение:
  void process(int mat[][4], int rows) { ... }', 6)
    RETURNING id INTO v_lesson_id;

    -- Lesson 4.7
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '4.7. Углубление в привычные scanf и printf. Передача по указателю', 'Теперь, когда мы знаем про указатели, функция scanf становится полностью понятной.
Почему мы писали &n? Функция scanf должна была изменить значение переменной n. Поскольку в C аргументы передаются по значению, если бы мы написали scanf("%d", n), функция получила бы лишь копию значения n и не смогла бы его изменить.
Передавая адрес &n, мы даем scanf возможность записать считанное значение напрямую в область памяти нашей переменной.

Этот прием позволяет функциям "возвращать" несколько значений, принимая указатели и модифицируя данные по ним.', 7)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 4.7
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Обмен значений', 
        'Напишите функцию void swap(int *a, int *b), которая меняет местами значения переменных, на которые указывают a и b. Функция main уже написана за вас.

Формат ввода:
Два целых числа через пробел.

Формат вывода:
Те же числа, но поменянные местами.

Пример:
Ввод: 5 10
Вывод: 10 5', 
        E'#include <stdio.h>\n\n// Напишите функцию swap здесь\n\nint main(void) {\n    int x, y;\n    if (scanf("%d %d", &x, &y) == 2) {\n        swap(&x, &y);\n        printf("%d %d\\n", x, y);\n    }\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf,*', 
        '#define,goto,asm,__asm__,__asm,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5 10', '10 5\n', TRUE),
           (v_task_id, '-1 99', '99 -1\n', TRUE);

END $$;
