SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: Структуры и куча...';

    INSERT INTO modules (name, description) 
    VALUES ('Структуры и куча', 'Пользовательские типы данных и динамическое выделение памяти')
    RETURNING id INTO v_mod_id;

    -- Lesson 6.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '6.1. Структуры (struct)', 'Структуры позволяют объединять разные типы данных в один логический объект.

```cpp
struct Player {
    int id;
    int health;
    char name[50];
};
```

**Особенности памяти**:
Поля в структуре лежат в памяти последовательно. Но будьте осторожны: компилятор может добавлять "дырки" (padding) между полями для выравнивания данных. Это значит, что `sizeof(struct)` может быть больше, чем сумма размеров его полей.

**typedef**: Чтобы не писать каждый раз `struct Player`, можно создать псевдоним:
`typedef struct Player Player;` или сразу при объявлении.', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 6.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '6.2. Указатели на структуры и "->"', 'Передавать структуру в функцию "целиком" — плохая идея, так как она будет полностью скопирована. Лучше передать указатель.

Для работы с полями через указатель используется оператор стрелочка `->`.
```cpp
Player *ptr = &p1;
ptr->health = 80; // То же самое, что (*ptr).health = 80
```
Стрелочка — это просто удобный способ разыменовать указатель и сразу обратиться к полю.', 2)
    RETURNING id INTO v_lesson_id;

    -- Lesson 6.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '6.3. Динамическая память (Куча)', 'Для хранения данных с динамическим временем жизни используется **Куча**.

**Полный цикл работы**:
```cpp
int *arr = (int*)malloc(10 * sizeof(int));
if (arr == NULL) {
    return 1; // Ошибка выделения
}

arr[0] = 42; // Используем
free(arr);   // ОБЯЗАТЕЛЬНО освобождаем
```

**Memory Leak**: Если не вызвать `free`, память останется занятой до конца работы программы.', 3)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 6.1-6.3
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Простой узел списка', 
        'Создайте структуру `Node`, которая хранит целое число `value` и указатель `next` на следующий элемент такого же типа. Затем в функции `main` выделите память под один узел в куче, считайте число из стандартного ввода, запишите его в `value`, а указатель `next` сделайте равным `NULL`. Выведите `value` и освободите память.

Формат ввода:
Одно целое число.

Формат вывода:
Одно целое число — значение из структуры.

Пример:
Ввод: 42
Вывод: 42', 
        E'#include <stdio.h>\n#include <stdlib.h>\n\n// Определите структуру Node\n\nint main(void) {\n    // Ваш код\n    return 0;\n}',
        'struct,malloc,free', 
        '#define,goto,asm,__asm__,__asm,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '42', '42', TRUE),
           (v_task_id, '-99', '-99', TRUE);

    -- Lesson 6.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '6.4. Динамические массивы', 'Главное применение malloc — динамические массивы, размер которых неизвестен на этапе компиляции (например, он вводится пользователем).

  int n;
  scanf("%d", &n);
  int *arr = (int*)malloc(n * sizeof(int));
  if (arr == NULL) return 1;

В памяти этот кусок будет выглядеть так же, как статический массив, и мы сможем обращаться к элементам: arr[0], arr[1] и т.д.
Важный нюанс: при передаче динамического массива в функцию он передается точно так же — как указатель int *arr. Функция не знает, где живет массив — на стеке или в куче.', 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 6.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Динамическое выделение', 
        'Напишите программу, которая считывает число N, затем выделяет динамический массив целых чисел размера N. Заполните массив числами от 1 до N и выведите последний элемент. Обязательно освободите память с помощью free!

Формат ввода:
Одно положительное целое число N (до 10^5).

Формат вывода:
Одно целое число — последний элемент массива (число N).

Пример:
Ввод: 10
Вывод: 10', 
        E'#include <stdio.h>\n#include <stdlib.h>\n\nint main(void) {\n    int n;\n    if (scanf("%d", &n) != 1) return 0;\n    // Ваш код\n    return 0;\n}',
        'malloc,free', 
        '#define,goto,asm,__asm__,__asm,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '10', '10', TRUE),
           (v_task_id, '1', '1', TRUE);

    -- Lesson 6.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '6.5. Динамические строки', 'Строки тоже можно выделять динамически, если мы не знаем их размер.
Например, мы можем скопировать строку в кучу функцией strdup() (которая появилась в стандарте POSIX и включена в некоторые стандарты C). Под капотом она делает malloc(strlen(s) + 1) и strcpy().

Нюанс: не забудьте про +1 байт для нулевого терминатора! Если вы хотите выделить память под строку из 10 символов, вам нужно malloc(11 * sizeof(char)). Игнорирование этого факта — самая популярная причина уязвимостей типа "переполнение буфера в куче" (Heap Overflow).', 5)
    RETURNING id INTO v_lesson_id;

END $$;
