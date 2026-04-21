SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 12: STL. Основы...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 12: STL. Основы', 'Обзор стандартной библиотеки шаблонов: контейнеры, итераторы, алгоритмы.')
    RETURNING id INTO v_mod_id;

    -- 12.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.1. Контейнеры', 'STL предоставляет готовые структуры данных: последовательные (vector, list, deque) и адаптеры (stack, queue, priority_queue).');

    -- 12.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.2. Итераторы', 'Итераторы — это абстракция указателей для перебора элементов контейнеров. В C++11 появился range-based for loop.');

    -- 12.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.3. Алгоритмы', 'Библиотека <algorithm> содержит множество готовых функций (sort, find, transform, accumulate) для работы с контейнерами через итераторы.');

    -- 12.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.4. Ассоциативные контейнеры', 'Основаны на деревьях (map, set) или хэш-таблицах (unordered_map, unordered_set) и обеспечивают быстрый поиск по ключу.');

    -- 12.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.5. fstream', 'Файловые потоки ifstream (чтение) и ofstream (запись) для работы с текстовыми и бинарными файлами.');

    -- 12.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.6. stringstream и string_view - чем так хороши?', 'stringstream удобен для парсинга и форматирования строк, а std::string_view (C++17) обеспечивает легковесный (без аллокаций) доступ к подстрокам.');

    -- 12.7
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '12.7. any, variant, optional', 'Типы из C++17 для безопасной работы с отсутствующими значениями (optional), объединениями типов (variant) или любыми типами вообще (any).');

END $$;
