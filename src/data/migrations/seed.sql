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

BEGIN
    RAISE NOTICE 'Starting database seeding...';

    INSERT INTO modules (name, description) 
    VALUES ('Знакомство с платформой и первые шаги', 'Введение в курс, основы ОС и первая программа')
    RETURNING id INTO v_mod_intro;

    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_intro, '1.1. О платформе и разработчиках', 'Товарищи программисты, спасибо Вам большое за установку этой образовательной платформы!

Она представляет из себя обширный курс по изучению языка программирования C, плавно переходящий к работе с C++ и современному подходу к разработке ПО. К тому же, разработчики учли последние тренды, благодаря чему появилась идея с PvP-режимом и другими интересными фишками нашего проекта.
Мы не будем погружаться в историю создания этих языков, так как автор считает это излишним в рамках курса.
Основной курс рассчитан на новичков в computer science, а также на тех, кто хочет перейти с сильно абстрактных языков программирования на что-то более близкое к "железу" (в случае с C++ сохраняя возможность ООП-подхода к разработке).

Команда разработчиков платформы состоит из выпускников "КМПО РАНХиГС", которые загорелись идеей того, чтобы сделать по-настоящему "крутой" и большой проект, который может запомниться. 
Конкретнее:
    Сергазиев Руслан (rinster) - автор идеи и главный архитектор проекта, занимающийся core-механиками платформы и текстом курса, который сейчас на Вашем экране;
    Ротницкий Даниил (sernock) - замечательный программист, благодаря которому в жизнь воплотились идеи, связанные с GUI;
    Яковлев Илья (iluha481) - сильный разработчик. Засчёт Ильи были реализованы различные механизмы, такие как: ...;
    Фазылов Арсен (dageour) - ещё один незаменимый участник команды, занимающийся проектированием дизайна и написанием текста выпускной квалификационной работы;  
                
Удачи!

------------------
С уважением,
rinster (rinsterr@yandex.ru)'),
    (v_mod_intro, '1.2. Пару слов про Linux', 'Теория по Linux'),
    (v_mod_intro, '1.3. Базовый bash: навигация и основы работы с оболочкой', 'Теория по bash'),
    (v_mod_intro, '1.4. Структура простой программы на языке C. Функция main', 'Теория по структуре C'),
    (v_mod_intro, '1.5. Компиляция как процесс. Язык ассемблера', 'Теория по компиляции'),
    (v_mod_intro, '1.6. Первый вывод - функция printf', 'Теория по printf');

    INSERT INTO modules (name, description) 
    VALUES ('Основы программирования на языке C', 'Типы данных, арифметика, условия и циклы')
    RETURNING id INTO v_mod_c_basics;

    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_c_basics, '2.1. Базовые типы данных', 'Теория по типам данных'),
    (v_mod_c_basics, '2.2. Арифметика и логика', 'Теория по арифметике'),
    (v_mod_c_basics, '2.3. Первый ввод - функция scanf', 'Теория по scanf'),
    (v_mod_c_basics, '2.4. Условные операторы', 'Теория по условиям'),
    (v_mod_c_basics, '2.5. Тернарный оператор', 'Теория по тернарному оператору'),
    (v_mod_c_basics, '2.6. Циклы...', 'Теория по циклам')
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
    ) RETURNING id INTO v_task_id;

    FOR i IN 1..3 LOOP
        INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
        VALUES (v_task_id, 'Test ' || i, 'Output ' || i, (i % 2 = 1));
    END LOOP;

    INSERT INTO modules (name, description) VALUES ('Функции', 'Объявление, определение и макросы') RETURNING id INTO v_mod_func;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_func, '3.1. Declaration. Definition. Возврат значений из функций', 'Теория по декларациям'),
    (v_mod_func, '3.2. Параметры функций. Области видимости.', 'Теория по параметрам'),
    (v_mod_func, '3.3. Директивы препроцессора. #define', 'Теория по макросам');

    INSERT INTO modules (name, description) VALUES ('Указатели - фундамент. Массивы', 'Работа с указателями и массивами в памяти') RETURNING id INTO v_mod_ptr;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_ptr, '4.1. Понятие адреса. Операторы "&" и "*"', 'Теория по адресам'),
    (v_mod_ptr, '4.2. Указатели на базовые типы данных', 'Теория по базовым указателям'),
    (v_mod_ptr, '4.3. Одномерные статические массивы и их связь с указателями.', 'Теория по массивам'),
    (v_mod_ptr, '4.4. Адресная арифметика', 'Теория по арифметике указателей'),
    (v_mod_ptr, '4.5. Передача массивов в функции', 'Теория по передаче массивов'),
    (v_mod_ptr, '4.6. Многомерные массивы', 'Теория по многомерным массивам'),
    (v_mod_ptr, '4.7. Углубление в привычные scanf и printf', 'Продвинутый ввод/вывод');

    INSERT INTO modules (name, description) VALUES ('C-строки. Массивы символов', 'Строки как массивы char') RETURNING id INTO v_mod_cstr;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_cstr, '5.1. C-строки. ''\0''', 'Nul-терминированные строки'),
    (v_mod_cstr, '5.2. Базовые операции со строками', 'Обработка строк'),
    (v_mod_cstr, '5.3. Строковые литералы', 'Литералы'),
    (v_mod_cstr, '5.4. <string.h>', 'Заголовок string.h'),
    (v_mod_cstr, '5.5. <ctype.h>', 'Заголовок ctype.h');

    INSERT INTO modules (name, description) VALUES ('Структуры и куча', 'Пользовательские типы и динамическое выделение памяти') RETURNING id INTO v_mod_struct;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_struct, '6.1. struct. Объявление, доступ к полям. typedef', 'Основы структур'),
    (v_mod_struct, '6.2. Указатели на структуры, оператор "->"', 'Указатели на структуры'),
    (v_mod_struct, '6.3. Куча. Функции malloc, calloc, free', 'Динамическая память'),
    (v_mod_struct, '6.4. Динамические массивы', 'Динамические массивы'),
    (v_mod_struct, '6.5. Динамические строки', 'Динамические строки');

    INSERT INTO modules (name, description) VALUES ('Файловая система', 'Работа с файлами') RETURNING id INTO v_mod_fs;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_fs, '7.1. Базовая работа с файлами. Функции fopen. fclose', 'Открытие и закрытие'),
    (v_mod_fs, '7.2. Функции fprintf, fscanf, fgets', 'Чтение и запись'),
    (v_mod_fs, '7.3. Бинарный режим', 'Бинарные файлы'),
    (v_mod_fs, '7.4. Argc, argv - что это и зачем?', 'Аргументы командной строки'),
    (v_mod_fs, '7.5. Доп. материал к изучению языка C', 'Дополнительно');

    INSERT INTO modules (name, description) VALUES ('Первые шаги в язык C++', 'Отличия C++ от C') RETURNING id INTO v_mod_cpp_intro;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_cpp_intro, '8.1. g++/clang++ и первая программа на C++', 'Компиляторы C++'),
    (v_mod_cpp_intro, '8.2. Новые базовые типы данных', 'bool, wchar_t и т.д.'),
    (v_mod_cpp_intro, '8.3. Псевдонимы. using. namespace', 'Пространства имен'),
    (v_mod_cpp_intro, '8.4. Ссылки...', 'Ссылки'),
    (v_mod_cpp_intro, '8.5. Новые возможности const. constexpr', 'const/constexpr'),
    (v_mod_cpp_intro, '8.6. new / delete', 'Динамическая память в C++');

    INSERT INTO modules (name, description) VALUES ('Функции и ООП-подход', 'Классы и ООП') RETURNING id INTO v_mod_oop_func;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_oop_func, '9.1. Function overloading', 'Перегрузка функций'),
    (v_mod_oop_func, '9.2. Параметры по умолчанию', 'Дефолтные параметры'),
    (v_mod_oop_func, '9.3. inline. Некоторые особенности компиляторов', 'inline функции'),
    (v_mod_oop_func, '9.4. class. Основы инкапсуляции', 'Синтаксис class'),
    (v_mod_oop_func, '9.5. Неявный указатель this', 'Указатель this'),
    (v_mod_oop_func, '9.6. Лямбда-выражения', 'Лямбды'),
    (v_mod_oop_func, '9.7. static', 'Ключевое слово static');

    INSERT INTO modules (name, description) VALUES ('ООП', 'Наследование, полиморфизм') RETURNING id INTO v_mod_oop;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_oop, '10.1. Конструкторы', 'Конструкторы классов'),
    (v_mod_oop, '10.2. Operator overloading. Функторы', 'Перегрузка операторов'),
    (v_mod_oop, '10.3. Наследование. Базовый и производный классы', 'Основы наследования'),
    (v_mod_oop, '10.4. Защита при наследовании', 'public/protected/private наследование'),
    (v_mod_oop, '10.5. Полиморфизм в C++. virtual', 'Виртуальные функции'),
    (v_mod_oop, '10.6. Чисто виртуальные функции. override', 'Абстрактные классы'),
    (v_mod_oop, '10.7. Вызов в обход механизма виртуальности', 'Раннее связывание');

    INSERT INTO modules (name, description) VALUES ('Современный подход', 'Шаблоны, исключения, стандарты') RETURNING id INTO v_mod_modern;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_modern, '11.1. Шаблоны...', 'Основы шаблонов'),
    (v_mod_modern, '11.2. Немного про try/catch/throw. noexcept', 'Обработка исключений'),
    (v_mod_modern, '11.3. Атрибуты', 'Атрибуты C++'),
    (v_mod_modern, '11.4. namespace std', 'Пространство имен std');

    INSERT INTO modules (name, description) VALUES ('STL. Основы', 'Стандартная библиотека шаблонов') RETURNING id INTO v_mod_stl;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_stl, '12.1. Контейнеры', 'Sequence containers'),
    (v_mod_stl, '12.2. Итераторы', 'Итераторы STL'),
    (v_mod_stl, '12.3. Алгоритмы', 'Алгоритмы STL'),
    (v_mod_stl, '12.4. Ассоциативные контейнеры', 'map/set'),
    (v_mod_stl, '12.5. fstream', 'Файловые потоки'),
    (v_mod_stl, '12.6. stringstream и string_view - чем так хороши?', 'Потоковые строки и вьюхи'),
    (v_mod_stl, '12.7. any, variant, optional', 'Обертки C++17');

    INSERT INTO modules (name, description) VALUES ('Лучшие практики', 'Как писать хороший код на C++') RETURNING id INTO v_mod_best;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_best, '13.1. Smart-pointers', 'Умные указатели (unique_ptr, shared_ptr)'),
    (v_mod_best, '13.2. RAII', 'Идиома RAII'),
    (v_mod_best, '13.3. Move-семантика', 'Move-семантика и rvalue-ссылки'),
    (v_mod_best, '13.4. Правило пяти', 'Правило 3 / 5 / 0'),
    (v_mod_best, '13.5. SOLID', 'Принципы SOLID в C++'),
    (v_mod_best, '13.6. Паттерны проектирования', 'Паттерны (Singleton, Factory, Builder)'),
    (v_mod_best, '13.7. Системы сборки', 'CMake, Ninja, Make'),
    (v_mod_best, '13.8. Отладка и профилирование', 'GDB, Valgrind, профилировщики'),
    (v_mod_best, '13.9. Тестирование', 'Google Test, Catch2');

    INSERT INTO modules (name, description) VALUES ('Развитие', 'Что делать дальше') RETURNING id INTO v_mod_dev;
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_dev, '14.1. Роадмапы и дальнейшее изучение', 'Книги, курсы, open-source'),
    (v_mod_dev, '14.2. Заключение', 'Слова напутствия');

    INSERT INTO achievements (name, description, icon_path) VALUES 
    ('First Blood', 'Completed the first lesson', '/icons/achievements/first_blood.png'),
    ('Pointers Master', 'Completed pointers and memory mapping', '/icons/achievements/pointers_master.png'),
    ('CPP Pioneer', 'Entered the C++ Realm', '/icons/achievements/cpp_pioneer.png'),
    ('Clean Coder', 'Obtained all best practices', '/icons/achievements/clean_coder.png')
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
