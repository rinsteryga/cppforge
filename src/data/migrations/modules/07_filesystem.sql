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
    (v_mod_id, '7.1. Базовая работа с файлами. Функции fopen. fclose', 'Для работы с файлами в C используется тип FILE (определенный в <stdio.h>), который инкапсулирует файловый дескриптор ОС и внутренние буферы.

  FILE *f = fopen("data.txt", "r");

Второй аргумент — это режим открытия:
- "r" (read) — чтение. Файл должен существовать.
- "w" (write) — запись. Создаст файл, а если он есть — очистит его!
- "a" (append) — дозапись в конец файла.

Нюанс: fopen может вернуть NULL (нет прав, файл не найден). Всегда проверяйте это!
После работы файл ОБЯЗАТЕЛЬНО нужно закрыть: fclose(f). ОС имеет лимит на количество одновременно открытых файловых дескрипторов (в Linux часто 1024). Забудете закрыть в цикле — программа "упадёт" с ошибкой "Too many open files".', 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 7.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '7.2. Функции fprintf, fscanf, fgets', 'Чтение и запись в файлы работают аналогично stdin и stdout (кстати, stdin и stdout — это тоже указатели типа FILE*!).

- fprintf(f, "%d", x); — отформатированный вывод в файл.
- fscanf(f, "%d", &x); — форматированный ввод. Нюанс: fscanf плох для чтения строк, так как спотыкается о пробелы.

Для безопасного чтения текста по строкам используйте fgets:
  char buffer[256];
  fgets(buffer, sizeof(buffer), f);
Она читает до переноса строки (\n) или конца буфера, гарантированно добавляя нуль-терминатор. Идеально для парсинга конфигураций.', 2)
    RETURNING id INTO v_lesson_id;

    -- Lesson 7.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '7.3. Бинарный режим', 'Текстовый режим ("r", "w") хорош для строк, но ОС Windows делает "магию" с переносами строк (заменяет \n на \r\n при записи и обратно при чтении). В Linux такого нет, \n там всегда один байт.

Если мы хотим записать в файл "сырые" байты памяти (например, саму структуру или картинку), мы должны использовать бинарный режим: "rb" или "wb".
Для чтения и записи блоков байт используют:
- fread(buffer, size, count, file)
- fwrite(buffer, size, count, file)

Это намного быстрее форматированного текстового вывода, так как нет затрат на парсинг текста в числа и обратно (функциям не нужно переводить байт 42 в строку "42").', 3)
    RETURNING id INTO v_lesson_id;

    -- Lesson 7.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '7.4. Argc, argv - что это и зачем?', 'Мы наконец добрались до параметров функции main!
Программы (например, утилита gcc или ls) принимают флаги из терминала. Они попадают в main:
  int main(int argc, char *argv[])

- argc (argument count) — количество аргументов.
- argv (argument vector) — массив строк (C-строк). 

Нюанс: argv[0] ВСЕГДА содержит имя самой запускаемой программы (как вы ее вызвали из терминала). Поэтому ваши реальные аргументы начинаются с argv[1].
Например: gcc main.c -o program
argc будет 4. argv[0] = "gcc", argv[1] = "main.c", и т.д.
Массив заканчивается NULL указателем (argv[argc] == NULL).', 4)
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
