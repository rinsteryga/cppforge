SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: Файловая система...';

    INSERT INTO modules (name, description) 
    VALUES ('Файловая система', 'Работа с файлами, потоки ввода-вывода и аргументы командной строки')
    RETURNING id INTO v_mod_id;

    -- Lesson 7.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '7.1. Работа с файлами', 'Для работы с файлами используется структура `FILE`.

**Открытие и проверка**:
```cpp
FILE *f = fopen("data.txt", "r");
if (f == NULL) {
    perror("Ошибка открытия");
    return 1;
}
```

**Режимы**: `"r"`, `"w"`, `"a"`.

**Важно**: Всегда вызывайте `fclose(f)`, чтобы сохранить изменения и закрыть дескриптор.', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 7.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '7.2. Чтение и Запись', 'Файловые потоки работают почти так же, как `printf` и `scanf`.

- `fprintf(f, ...)`: Пишет текст в файл.
- `fscanf(f, ...)`: Читает по словам.

**`fgets` — лучший выбор для строк**:
`fgets(buffer, sizeof(buffer), f);`
Эта функция читает целую строку (до `\n`). Она безопасна, так как никогда не запишет больше байт, чем размер вашего буфера. Это золотой стандарт для чтения текстовых файлов в C.', 2)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 7.1-7.2
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Чтение чисел из файла', 
        'Напишите функцию `int sum_from_file(FILE *f)`, которая считывает из переданного файла два целых числа и возвращает их сумму. Функция `main` (которая открывает файл и вызывает вашу функцию) уже написана. Считайте, что файл корректен и числа там есть.

Формат ввода:
(Имитация файла) Два целых числа, разделенных пробелом.

Формат вывода:
Одно целое число — сумма.

Пример:
Ввод: 10 20
Вывод: 30', 
        E'#include <stdio.h>\n\nint sum_from_file(FILE *f) {\n    // Ваш код\n}\n\nint main(void) {\n    // В тестовой системе stdin используется как файл\n    printf("%d\\n", sum_from_file(stdin));\n    return 0;\n}',
        'FILE,fscanf', 
        '#define,goto,asm,__asm__,__asm,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '10 20', '30', TRUE),
           (v_task_id, '-5 5', '0', TRUE);

    -- Lesson 7.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '7.3. Текст vs Бинарные данные', 'В текстовом режиме ОС может "исправлять" переносы строк (например, в Windows `\n` превращается в два байта `\r\n`). 

Если вам нужно сохранить "сырые" байты (картинку, массив структур), используйте бинарный режим (`"rb"`, `"wb"`).

**Функции прямого доступа**:
`fread` и `fwrite` копируют блоки памяти напрямую на диск. Это в разы быстрее, чем `fprintf`, потому что компьютеру не нужно превращать числа в текст.', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 7.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '7.4. Параметры запуска (argc, argv)', 'Любая программа может принимать команды при запуске.

```cpp
int main(int argc, char *argv[]) {
    printf("Программа: %s\n", argv[0]);
    for (int i = 1; i < argc; i++) {
        printf("Аргумент %d: %s\n", i, argv[i]);
    }
}
```
**Важно**: `argv[0]` — это имя программы. Реальные данные начинаются с `argv[1]`.', 4)
    RETURNING id INTO v_lesson_id;

    -- Lesson 7.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '7.5. Доп. материал к изучению языка C', 'Язык C лаконичен, но таит много глубин. Вот что стоит изучить дальше самостоятельно, если вы хотите стать Senior C Engineer:
1. Union и Bit-fields — тонкая работа на уровне битов, используется в драйверах и сетевых протоколах (разбор IP-пакетов).
2. Указатели на функции — основа для коллбэков, полиморфизма в C (в ядре Linux так реализована виртуальная файловая система VFS).
3. Volatile и restrict — ключевые слова, критически важные для написания драйверов и оптимизации.
4. Multithreading (<pthread.h> в POSIX или <threads.h> в C11) и атомарные операции (<stdatomic.h>).

А мы переходим к C++!', 5)
    RETURNING id INTO v_lesson_id;

END $$;
