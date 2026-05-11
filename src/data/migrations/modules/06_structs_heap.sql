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
    (v_mod_id, '6.1. struct. Объявление, доступ к полям. typedef', 'Структуры (struct) позволяют объединять разные типы данных в один логический объект. Это первый шаг к объектно-ориентированному программированию (в C это называется абстракцией данных).

  struct Player {
      int id;
      int health;
      char name[50];
  };

Доступ к полям переменной-структуры осуществляется через оператор точка (.):
  struct Player p1;
  p1.health = 100;

Нюанс языка C: чтобы не писать каждый раз слово "struct", используют typedef — создание псевдонима типа:
  typedef struct {
      int x, y;
  } Point;
Теперь можно объявлять переменные просто как Point p;.', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 6.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '6.2. Указатели на структуры, оператор "->"', 'При передаче большой структуры в функцию по значению происходит копирование всех её байтов. Это неэффективно. Лучше передавать указатель на структуру.

Если у нас есть указатель Point *ptr = &p;, чтобы обратиться к полю, мы должны сначала разыменовать указатель, а потом использовать точку: (*ptr).x = 10. Скобки обязательны, так как приоритет точки выше, чем у звездочки.

В C для этого есть специальный оператор-стрелочка ->:
  ptr->x = 10;
Это абсолютно эквивалентно (*ptr).x, но читается гораздо легче.', 2)
    RETURNING id INTO v_lesson_id;

    -- Lesson 6.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '6.3. Куча. Функции malloc, calloc, free', 'До сих пор все наши переменные жили на стеке (Stack). Стек быстр, но его размер ограничен (обычно несколько мегабайт), и переменные уничтожаются при выходе из функции.
Для хранения больших объемов данных (или данных с динамическим временем жизни) используется "Куча" (Heap) — большая область памяти, предоставляемая операционной системой.

В заголовочном файле <stdlib.h> есть функции:
- void* malloc(size_t size) — выделяет size байт неинициализированной ("грязной") памяти.
- void* calloc(size_t num, size_t size) — выделяет память и заполняет её нулями.

Функции возвращают бестиповый указатель (void*). Если память не удалось выделить (например, кончилась ОЗУ), вернется NULL. Нюанс: всегда проверяйте результат malloc на NULL!

После использования динамическую память нужно ОБЯЗАТЕЛЬНО вернуть ОС с помощью функции free(ptr). Если этого не сделать, произойдет утечка памяти (Memory Leak) — программа будет потреблять всё больше ОЗУ, пока не упадет.', 3)
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
        'main,return,int,void,#include,stdio.h,stdlib.h,printf,scanf,malloc,free,struct,NULL', 
        '#define,goto,asm,__asm__,__asm,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '42', '42\n', TRUE),
           (v_task_id, '-99', '-99\n', TRUE);

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
        'main,return,int,void,#include,stdio.h,printf,scanf,stdlib.h,malloc,free', 
        '#define,goto,asm,__asm__,__asm,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '10', '10\n', TRUE),
           (v_task_id, '1', '1\n', TRUE);

    -- Lesson 6.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '6.5. Динамические строки', 'Строки тоже можно выделять динамически, если мы не знаем их размер.
Например, мы можем скопировать строку в кучу функцией strdup() (которая появилась в стандарте POSIX и включена в некоторые стандарты C). Под капотом она делает malloc(strlen(s) + 1) и strcpy().

Нюанс: не забудьте про +1 байт для нулевого терминатора! Если вы хотите выделить память под строку из 10 символов, вам нужно malloc(11 * sizeof(char)). Игнорирование этого факта — самая популярная причина уязвимостей типа "переполнение буфера в куче" (Heap Overflow).', 5)
    RETURNING id INTO v_lesson_id;

END $$;
