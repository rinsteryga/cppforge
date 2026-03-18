-- ===========================================================
-- CPPFORGE Database Seed Script
-- ===========================================================

BEGIN;

TRUNCATE TABLE 
    users, 
    modules, 
    lessons, 
    coding_tasks, 
    test_cases, 
    quizzes, 
    quiz_options, 
    matching_tasks, 
    matching_pairs, 
    achievements, 
    user_achievements, 
    submissions, 
    execution_results, 
    user_progress 
RESTART IDENTITY CASCADE;

DO $$
DECLARE
    v_mod_intro BIGINT;
    v_mod_c_basics BIGINT;
    v_mod_func BIGINT;
    v_mod_ptr BIGINT;
    v_mod_cstr BIGINT;
    v_mod_struct BIGINT;
    v_mod_fs BIGINT;
    v_mod_cpp_intro BIGINT;
    v_mod_oop_func BIGINT;
    v_mod_oop BIGINT;
    v_mod_modern BIGINT;
    v_mod_stl BIGINT;
    v_mod_best BIGINT;
    v_mod_dev BIGINT;

    i INT;
    v_user_id BIGINT;
    v_achievement_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
    v_matching_task_id BIGINT;

BEGIN
    RAISE NOTICE 'Starting database seeding...';

    INSERT INTO modules (name, description) 
    VALUES ('Знакомство с платформой и первые шаги', 'Введение в курс, основы ОС и первая программа')
    RETURNING id INTO v_mod_intro;

    -- Lesson 1.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_intro, '1.1. О платформе и разработчиках', 'Товарищи программисты, спасибо Вам большое за установку этой образовательной платформы!

Она представляет из себя обширный курс по изучению языка программирования C, плавно переходящий к работе с C++ и современному подходу к разработке ПО. К тому же, разработчики учли последние тренды, благодаря чему появилась идея с PvP-режимом и другими интересными фишками нашего проекта.
Мы не будем погружаться в историю создания этих языков, так как автор считает это излишним в рамках курса.
Основной курс рассчитан на новичков в computer science, а также на тех, кто хочет перейти с сильно абстрактных языков программирования на что-то более близкое к "железу" (в случае с C++ сохраняя возможность ООП-подхода к разработке).

Команда разработчиков платформы состоит из выпускников "КМПО РАНХиГС", которые загорелись идеей того, чтобы сделать по-настоящему "крутой" и большой проект, который может запомниться. 
Конкретнее:
  Сергазиев Руслан (rinster) - автор идеи и архитектор проекта, занимающийся core-механиками платформы и текстом курса, который сейчас на Вашем экране;
  Ротницкий Даниил (sernock) - замечательный программист, благодаря которому в жизнь воплотились идеи, связанные с GUI;
  Яковлев Илья (iluha481) - сильный разработчик. Засчёт Ильи были реализованы различные механизмы, такие как: ...;
  Фазылов Арсен (dageour) - ещё один незаменимый участник команды, занимающийся проектированием дизайна и написанием текста выпускной квалификационной работы;         
        
Благодарности:

Удачи!

------------------
С уважением,
rinster (rinsterr@yandex.ru)');

    -- Lesson 1.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_intro, '1.2. Пару слов про операционные системы POSIX', 'Добро пожаловать в мир системной разработки! Прежде чем мы напишем строчку кода на C, нам нужно подготовить "почву". Исторически языки C и Unix-подобные операционные системы (ОС) тесно связаны — язык C во многом создавался для написания ОС Unix. Сегодня Unix-подобные системы (включая Linux: Ubuntu, Debian, Fedora и macOS) объединены общим стандартом POSIX, и именно они являются де-факто стандартом для профессиональной разработки на C/C++.


Почему именно Linux? Во-первых, философия UNIX гласит: "Всё есть файл". Практически любые ресурсы ОС (жесткие диски, сетевые соединения, терминалы) представлены как файловые дескрипторы. Это делает работу с вводом-выводом кристально прозрачной для программиста. Во-вторых, в Linux изначально встроены (или легко устанавливаются) нативные инструменты (toolchains) для разработчика: компиляторы (GCC — GNU Compiler Collection, Clang), системы сборки (Make, CMake) и мощные отладчики (GDB). В-третьих, работая в Linux, вы получаете прямой и предсказуемый контроль над управлением памятью и системными вызовами (syscalls), минуя лишние абстрактные слои.

В рамках нашей платформы мы предполагаем, что вы работаете в Linux-подобной среде или используете подсистему WSL (Windows Subsystem for Linux). Не пугайтесь терминала — скоро интерфейс командной строки (CLI) станет вашим основным профессиональным инструментом.');

    -- Lesson 1.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_intro, '1.3. Базовый bash: интерфейс командной строки (CLI)', 'Оболочка (Shell) — это командный интерпретатор, программа, которая обеспечивает интерфейс между пользователем и ядром ОС. Одной из самых популярных оболочек является bash (Bourne Again Shell). В отличие от графических интерфейсов (GUI), CLI позволяет взаимодействовать с системой напрямую с помощью текстовых команд.

Рассмотрим несколько базовых утилит (команд) для навигации по файловой системе:
  pwd (print working directory) — выводит абсолютный путь к текущей рабочей директории. Абсолютный путь берет начало от корневого каталога системы (/).
  ls (list) — выводит список файлов и поддиректорий текущего каталога. Утилиты принимают аргументы (флаги/опции). Например, флаг -l покажет детали (права доступа, владельца, размер файла), а -a отобразит скрытые файлы (в Linux скрытые файлы начинаются с точки .). Опции можно комбинировать: ls -la.
  cd (change directory) — изменяет текущую рабочую директорию. Можно использовать абсолютный путь (cd /var/log) или относительный (cd folder). Специальные обозначения: .. — родительская директория (подняться на уровень выше), ~ — домашняя директория пользователя.
  mkdir (make directory) — создает новую директорию. Например, mkdir my_project.
  touch — утилита, предназначенная для обновления временных меток файла, но если файла не существует, она создаст новый пустой файл: touch main.c.')
    RETURNING id INTO v_lesson_id;

    INSERT INTO matching_tasks (lesson_id, title, description) VALUES (v_lesson_id, 'Команды bash', 'Сопоставьте команду bash с её назначением') RETURNING id INTO v_matching_task_id;
    INSERT INTO matching_pairs (matching_task_id, left_item, right_item) VALUES 
    (v_matching_task_id, 'pwd', 'Вывод абсолютного пути текущей рабочей директории'),
    (v_matching_task_id, 'ls -la', 'Форматированный вывод содержимого директории, включая скрытые файлы'),
    (v_matching_task_id, 'cd ..', 'Переход в родительскую директорию относительно текущей'),
    (v_matching_task_id, 'mkdir src', 'Создание новой директории в текущем каталоге');

    INSERT INTO quizzes (lesson_id, title, question) VALUES (v_lesson_id, 'Создание файлов', 'Что технически делает команда touch file.txt, если файл еще не существует?') RETURNING id INTO v_quiz_id;
    INSERT INTO quiz_options (quiz_id, option_text, is_correct) VALUES 
    (v_quiz_id, 'Создает новый пустой файл с указанным именем', TRUE),
    (v_quiz_id, 'Открывает файл для текстового редактирования', FALSE),
    (v_quiz_id, 'Запускает процесс переименования директории', FALSE),
    (v_quiz_id, 'Компилирует код в исполняемый файл', FALSE);

    -- Lesson 1.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_intro, '1.4. Структура программы на языке C. Функция main (точка входа)', 'Любая исполняемая программа на языке C должна иметь точку входа (Entry point) — функцию с именем main. При запуске скомпилированной программы операционная система передает управление именно этой функции, предварительно инициализировав окружение и среду выполнения (C Runtime, CRT).


Базовый каркас минимальной программы выглядит следующим образом (но можно и короче написать):
  int main(void) {
    return 0;
  }
  
int (от слова integer) — это возвращаемый тип функции. После завершения своей работы, программа обязана вернуть операционной системе код возврата (Exit status).
main — строго зарезервированное имя для главной функции. Других имен быть не может (по стандарту).
(void) — ключевое слово void в списке параметров явно указывает компилятору, что функция не принимает никаких аргументов. Позже мы узнаем, что main может принимать аргументы командной строки в виде (int argc, char *argv[]).
Блок кода, заключенный в фигурные скобки { }, определяет область видимости функции main.
Инструкция return 0; завершает работу функции и передает код 0 родительскому процессу (ОС). По конвенции систем POSIX код 0 (ноль) означает успешное завершение (Success). Любое ненулевое значение (например, 1 или -1) расценивается операционной системой как индикатор ошибки.
И не забывайте: точка с запятой ; в C — это терминатор инструкции, она обозначает логический конец выражения (при желании можно пропатчить компилятор и избавиться от него или заменить на другой символ, но никому так делать не рекомендуется).')
    RETURNING id INTO v_lesson_id;

    INSERT INTO quizzes (lesson_id, title, question) VALUES (v_lesson_id, 'Код возврата', 'Какой смысл несет инструкция return 0; в функции main для операционной системы?') RETURNING id INTO v_quiz_id;
    INSERT INTO quiz_options (quiz_id, option_text, is_correct) VALUES 
    (v_quiz_id, 'Процесс завершился штатно, без ошибок (Exit Success).', TRUE),
    (v_quiz_id, 'Процессор выполнил 0 тактов.', FALSE),
    (v_quiz_id, 'Программа требует перезапуска ОС.', FALSE),
    (v_quiz_id, 'Оперативная память успешно освобождена сборщиком мусора.', FALSE);

    -- Lesson 1.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_intro, '1.5. Трансляция кода (компиляция). Язык ассемблера', 'Процессор компьютера "понимает" только машинные инструкции (последовательности битов), специфичные для его архитектуры (ISA, например x86_64 или ARM64). Язык C — это язык высокого уровня (хотя по меркам современных языков его часто называют среднеуровневым и даже низкоуровневным). Для того, чтобы превратить текст программы в исполняемый бинарный файл, необходим транслятор, называемый компилятором.
  
В наборе инструментов GCC (GNU Compiler Collection) процесс получения исполняемого файла из исходного кода (.c) проходит строго в 4 этапа:
  Препроцессинг (gcc -E): Текстовая обработка исходного кода до синтаксического анализа. Препроцессор удаляет комментарии, разворачивает макросы (строки, начинающиеся с #define) и включает (копирует) текстовое содержимое заголовочных файлов (директивы #include). Результатом является "расширенный" C-файл (.i).
  Компиляция (gcc -S): Самый сложный этап. Синтаксический, семантический анализ структуры кода и его оптимизация. На этом этапе абстрактное синтаксическое дерево (AST) преобразуется в промежуточное представление, а затем — в язык Ассемблера (человекочитаемое представление машинных команд). Результат — ассемблерный листинг (.s).
  Ассемблирование (gcc -c): Утилита as (ассемблер) переводит ассемблерный код непосредственно в машинные инструкции. Создается бинарный объектный файл (.o или .obj). Он содержит машинный код, но еще не может стать независимым процессом, так как его адреса относительны и ссылки на внешние функции (например, на print) не разрешены.
  Компоновка / Линковка (утилита ld): Связывание. Линковщик берет один или несколько объектных файлов, объединяет их в единый блок, разрешает перекрестные ссылки и "привязывает" код из стандартной библиотеки (например libc для языка C). Итогом является готовый к загрузке в память исполняемый файл в формате ELF (Executable and Linkable Format) для Linux, PE (Portable Executable) для Windows или Mach-O для macOS.         
Команда gcc main.c -o program выполнит все 4 этапа автоматически, скрыв от программиста промежуточные файлы, и создаст бинарный файл program.')
    RETURNING id INTO v_lesson_id;


    INSERT INTO matching_tasks (lesson_id, title, description) VALUES (v_lesson_id, 'Этапы трансляции кода', 'Сопоставьте этап трансляции трансляции (toolchain) исходного C-кода с его технической сутью:') RETURNING id INTO v_matching_task_id;
    INSERT INTO matching_pairs (matching_task_id, left_item, right_item) VALUES 
    (v_matching_task_id, 'Препроцессинг', 'Текстовая макроподстановка и включение заголовков без синтаксического анализа'),
    (v_matching_task_id, 'Компиляция', 'Трансляция расширенного исходного кода в ассемблерные директивы для целевой архитектуры'),
    (v_matching_task_id, 'Ассемблирование', 'Генерация перемещаемого машинного кода (создание объектного файла .o)'),
    (v_matching_task_id, 'Линковка (Компоновка)', 'Разрешение внешних символов и склеивание объектных файлов с библиотеками');

    INSERT INTO quizzes (lesson_id, title, question) VALUES (v_lesson_id, 'Этапы трансляции', 'Какой формат файлов производит этап ассемблирования перед передачей их линковщику?') RETURNING id INTO v_quiz_id;
    INSERT INTO quiz_options (quiz_id, option_text, is_correct) VALUES 
    (v_quiz_id, 'Бинарные объектные файлы (.o или .obj)', TRUE),
    (v_quiz_id, 'Исполняемые образы процессов (.exe или бинарники ELF)', FALSE),
    (v_quiz_id, 'Текстовые файлы макрорасширений препроцессора (.i)', FALSE),
    (v_quiz_id, 'Ассемблерные человекочитаемые листинги (.s)', FALSE);

    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit)
    VALUES (
        v_lesson_id, 
        'Ваша первая пустая программа.', 
        'Напишите минимально возможную программу на языке C, которая компилируется и завершается успешно. Помните про правильное определение сигнатуры точки входа (без параметров командной строки) и код возврата ОС. Никакого текста выводить не нужно.', 
        'int main(void) {\n    return 0;\n}',
        'main,return,int,void', 
        '#include,#define,while,goto,do,asm,__asm__,__asm,FILE,fopen,fclose,fread,fwrite,system,exec', 
        2000, 
        256
    ) RETURNING id INTO v_task_id; -- ИСПРАВЛЕНО (добавлено возвращение в v_task_id)

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '', '', TRUE);

    -- Lesson 1.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_intro, '1.6. Первый вывод - функция printf', 'Программы без ввода/вывода (I/O, Input/Output) имеют мало практической ценности. Мы уже условились, что в POSIX-системах "всё есть файл". Когда ОС запускает вашу программу, она автоматически открывает и привязывает к вашему процессу три стандартных файловых дескриптора:
  stdin (Standard Input, дескриптор 0) — стандартный поток ввода (по умолчанию привязан к вашей клавиатуре).
  stdout (Standard Output, дескриптор 1) — стандартный поток вывода (привязан к вашему терминалу).
  stderr (Standard Error, дескриптор 2) — стандартный поток для вывода сообщений об ошибках (также выводится в терминал, но логически разделен с stdout).
      
Для записи байтов в стандартный поток stdout существует сложная, но невероятно мощная функция стандартной библиотеки C (libc) — printf (от англ. "print formatted", форматированный вывод).

Поскольку printf не встроена в ядро языка, мы должны объявить её (задекларировать), чтобы компилятор знал о её существовании и сигнатуре. Для этого с помощью директивы препроцессора #include мы подключаем системный заголовочный файл stdio.h (Standard Input/Output Header).


Программа, приветствующая программиста:
  #include <stdio.h> // Системные заголовки принято обрамлять в угловые скобки
  int main(void) {
    printf("Hello, Platform!\n");
    return 0;
  }
      
"Hello, Platform!\n" — это строковый литерал (массив символов char), завершающийся нуль-терминатором(''\0''), который записываем в двойных кавычках ("").
\n (Line Feed, Перевод строки) — это специальная управляющая escape-последовательность символов, которая является директивой для вашего терминала перевести курсор (каретку) на следующую строку. Без неё вывод следующей команды прилип бы вплотную к вашему "Hello".')
    RETURNING id INTO v_lesson_id;

    INSERT INTO matching_tasks (lesson_id, title, description) VALUES (v_lesson_id, 'Элементы программы и их роль', 'Сопоставьте элементы кода с их технической ролью:') RETURNING id INTO v_matching_task_id;
    INSERT INTO matching_pairs (matching_task_id, left_item, right_item) VALUES 
    (v_matching_task_id, '#include <stdio.h>', 'Директива препроцессора для внедрения прототипов функций буферизованного ввода-вывода стандартной библиотеки'),
    (v_matching_task_id, 'int main(void)', 'Определение точки входа в процесс без поддержки аргументов командной строки'),
    (v_matching_task_id, 'printf("Text");', 'Вызов функции форматированного вывода в стандартный поток stdout (дескриптор 1)'),
    (v_matching_task_id, '\n', 'Управляющая escape-последовательность (Line Feed) для перевода буфера терминала на новую строку');

    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit)
    VALUES (
        v_lesson_id, 
        'Базовый буферизованный вывод.', 
        'Напишите программу, которая выводит в стандартный поток вывода (stdout) ровно одну строку: Hello, cppforge! и выполняет нормативный перевод курсора на новую строку терминала через \n. Подсказка: В языке C необходимо использовать директиву препроцессора для подключения заголовочного файла с объявлениями ввода-вывода, а затем вернуть код успешного завершения системе.', 
        '#include <stdio.h>\n\nint main(void) {\n    // Ваш код\n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf', 
        '#define,while,goto,do,asm,__asm__,__asm,FILE,fopen,fclose,fread,fwrite,system,exec,stdlib.h,string.h,math.h,ctype.h,stdbool.h', 
        2000, 
        256
    ) RETURNING id INTO v_task_id; -- ИСПРАВЛЕНО

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '', 'Hello, cppforge!\n', TRUE);

    INSERT INTO modules (name, description) 
    VALUES ('Основы программирования на языке C', 'Типы данных, арифметика, условия и циклы')
    RETURNING id INTO v_mod_c_basics;

    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_c_basics, '2.1. Базовые типы данных', 'Теория...'),
    (v_mod_c_basics, '2.2. Арифметика и логика', 'Теория...'),
    (v_mod_c_basics, '2.3. Первый ввод - функция scanf', 'Теория...'),
    (v_mod_c_basics, '2.4. Условные операторы', 'Теория...'),
    (v_mod_c_basics, '2.5. Тернарный оператор', 'Теория...');

    INSERT INTO lessons (module_id, title, content) 
    VALUES (v_mod_c_basics, '2.6. Циклы...', 'Теория по циклам')
    RETURNING id INTO v_lesson_id;

    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit)
    VALUES (
        v_lesson_id, 
        'Использование цикла for', 
        'Напишите программу, которая выводит числа от 1 до 10.', 
        '#include <stdio.h>\n\nint main() {\n    // Ваш код здесь\n    return 0;\n}',
        'for,printf', 
        'while,goto,do,asm,__asm__,__asm,stdlib.h,string.h,math.h,ctype.h,stdbool.h,FILE,fopen,fclose,fread,fwrite,malloc,calloc,realloc,free,system,exec', 
        2000, 
        256
    ) RETURNING id INTO v_task_id; -- ИСПРАВЛЕНО

    FOR i IN 1..3 LOOP
        INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
        VALUES (v_task_id, 'Test ' || i, 'Output ' || i, (i % 2 = 1));
    END LOOP;


    INSERT INTO modules (name, description) VALUES ('Функции', 'Объявление, определение и макросы') RETURNING id INTO v_mod_func;
    INSERT INTO modules (name, description) VALUES ('Указатели - фундамент. Массивы', 'Работа с указателями и массивами в памяти') RETURNING id INTO v_mod_ptr;
    INSERT INTO modules (name, description) VALUES ('C-строки. Массивы символов', 'Строки как массивы char') RETURNING id INTO v_mod_cstr;
    INSERT INTO modules (name, description) VALUES ('Структуры и куча', 'Пользовательские типы и динамическое выделение памяти') RETURNING id INTO v_mod_struct;
    INSERT INTO modules (name, description) VALUES ('Файловая система', 'Работа с файлами') RETURNING id INTO v_mod_fs;
    INSERT INTO modules (name, description) VALUES ('Первые шаги в язык C++', 'Отличия C++ от C') RETURNING id INTO v_mod_cpp_intro;
    INSERT INTO modules (name, description) VALUES ('Функции и ООП-подход', 'Классы и ООП') RETURNING id INTO v_mod_oop_func;
    INSERT INTO modules (name, description) VALUES ('ООП', 'Наследование, полиморфизм') RETURNING id INTO v_mod_oop;
    INSERT INTO modules (name, description) VALUES ('Современный подход', 'Шаблоны, исключения, стандарты') RETURNING id INTO v_mod_modern;
    INSERT INTO modules (name, description) VALUES ('STL. Основы', 'Стандартная библиотека шаблонов') RETURNING id INTO v_mod_stl;
    INSERT INTO modules (name, description) VALUES ('Лучшие практики', 'Как писать хороший код на C++') RETURNING id INTO v_mod_best;
    INSERT INTO modules (name, description) VALUES ('Развитие', 'Что делать дальше') RETURNING id INTO v_mod_dev;

    -- ИСПРАВЛЕНО (Удален RETURNING id INTO v_achievement_id из массовой вставки)
    INSERT INTO achievements (name, description, icon_path) VALUES 
    ('First Blood', 'Completed the first lesson', '/icons/achievements/first_blood.png'),
    ('Pointers Master', 'Completed pointers and memory mapping', '/icons/achievements/pointers_master.png'),
    ('CPP Pioneer', 'Entered the C++ Realm', '/icons/achievements/cpp_pioneer.png');

    -- Вставляем последнее достижение отдельно, чтобы получить ID для цикла ниже
    INSERT INTO achievements (name, description, icon_path) 
    VALUES ('Clean Coder', 'Obtained all best practices', '/icons/achievements/clean_coder.png')
    RETURNING id INTO v_achievement_id;

    FOR i IN 1..50 LOOP
        INSERT INTO users (username, email, password_hash, avatar_path, bio)
        VALUES (
            'student_' || i, 
            'student' || i || '@example.com', 
            '$2y$10$wE0qO/T7M19I/A.M.xO..e...v1hQ.m40pSWeK0T9rY6a',
            '/avatars/default.png', 
            'I am a C/C++ student eager to learn!'
        ) RETURNING id INTO v_user_id;

        IF (i % 10 = 0) THEN
            INSERT INTO user_achievements (user_id, achievement_id) 
            VALUES (v_user_id, v_achievement_id);
        END IF;
    END LOOP;

    RAISE NOTICE 'Database seeding completed successfully.';
END $$;

COMMIT;