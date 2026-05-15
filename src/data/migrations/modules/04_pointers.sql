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
(v_mod_id, '4.1. Память и Адреса', 'Память компьютера — это гигантская таблица байтов. У каждого байта есть свой уникальный номер — **адрес**. 

**Операторы**:
- `&` (взятие адреса): позволяет узнать, где в памяти лежит переменная.
- `*` (разыменование): позволяет "зайти" по адресу и изменить данные.

Адрес — это просто число. Но в C адреса имеют типы (например, "адрес целого числа"), чтобы компилятор знал, сколько байт нужно прочитать по этому адресу.', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 4.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '4.2. Указатели', 'Указатель — это переменная, которая хранит адрес.
```cpp
int x = 42;
int *ptr = &x; // ptr теперь указывает на x
*ptr = 10; // x теперь равно 10
```

**Нюанс**: Размер указателя всегда фиксирован для конкретной архитектуры (обычно 8 байт на 64-битных системах), независимо от того, на что он указывает (на `char` или на огромную `struct`). Указатель — это просто "индекс" в оперативной памяти.', 2)
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
(v_mod_id, '4.3. Массивы и Указатели', 'Массив — это блок памяти, где элементы лежат вплотную друг к другу. 
**Важное правило**: в большинстве выражений имя массива превращается в указатель на его первый элемент.

```cpp
int arr[5];
int *p = arr; // То же самое, что &arr[0]
```
Однако массив — это не совсем указатель. У массива есть "память", а указатель — это лишь переменная с адресом. `sizeof(arr)` вернет размер всего массива, а `sizeof(p)` — только размер адреса (8 байт).', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 4.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '4.4. Адресная арифметика', 'Вы можете прибавлять числа к указателям.

```cpp
int arr[] = {10, 20, 30};
int *p = arr; 
printf("%d", *(p + 1)); // Выведет 20
```

**Итерация по массиву**:
```cpp
for (int *ptr = arr; ptr < arr + 3; ptr++) {
    printf("%d ", *ptr);
}
```

**void***: Специальный тип "универсального" указателя. Его нельзя разыменовать без приведения типа: `*(int*)p`.', 4)
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
Ввод: 5 10 -5 42 0 7
Вывод: 42', 
        E'#include <stdio.h>\n\nint main(void) {\n    int n;\n    if (scanf("%d", &n) != 1) return 0;\n    int arr[100];\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, E'5 10 -5 42 0 7', '42', TRUE),
           (v_task_id, E'3 -1 -2 -3', '-1', TRUE);

    -- Lesson 4.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '4.6. Двумерные массивы', 'Двумерный массив — это просто массив, внутри которого лежат другие массивы. В памяти они лежат **линейно** (строка за строкой).

```cpp
int matrix[3][4]; // 3 строки по 4 элемента
```
При передаче в функцию вы **обязаны** указать размер всех измерений, кроме первого, чтобы компилятор мог вычислить смещение в памяти:
`void process(int mat[][4], int rows);`', 6)
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
        'swap', 
        '#define,goto,asm,__asm__,__asm,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5 10', '10 5', TRUE),
           (v_task_id, '-1 99', '99 -1', TRUE);

END $$;
