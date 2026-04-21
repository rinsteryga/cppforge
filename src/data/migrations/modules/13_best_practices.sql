SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 13: Лучшие практики...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 13: Лучшие практики', 'Правильное управление ресурсами, архитектура, сборка и отладка программ.')
    RETURNING id INTO v_mod_id;

    -- 13.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.1. Smart-pointers', 'Умные указатели (std::unique_ptr, std::shared_ptr) автоматически управляют временем жизни динамически выделенной памяти, предотвращая утечки.');

    -- 13.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.2. RAII', 'RAII (Resource Acquisition Is Initialization) — ключевая идиома C++. Ресурс захватывается в конструкторе объекта и гарантированно освобождается в его деструкторе.');

    -- 13.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.3. Move-семантика', 'Семантика перемещения (std::move и rvalue-ссылки) позволяет эффективно "красть" ресурсы у временных объектов вместо дорогостоящего копирования.');

    -- 13.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.4. Правило пяти', 'Если класс определяет деструктор, конструктор копирования, оператор присваивания копированием, конструктор перемещения или оператор присваивания перемещением, скорее всего, ему нужны все пять.');

    -- 13.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.5. SOLID', 'Пять базовых принципов объектно-ориентированного программирования и проектирования, делающих программные системы более понятными, гибкими и поддерживаемыми.');

    -- 13.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.6. Паттерны проектирования', 'Типовые решения часто возникающих архитектурных проблем: Singleton, Factory, Observer, Decorator и другие.');

    -- 13.7
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.7. Системы сборки', 'Обзор инструментов автоматизации сборки C++ проектов (Make, CMake, Ninja) и управления зависимостями (vcpkg, Conan).');

    -- 13.8
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.8. Отладка и профилирование', 'Работа с отладчиками (GDB/LLDB, встроенные средства IDE) и использование профилировщиков для поиска узких мест производительности (Valgrind, perf).');

    -- 13.9
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.9. Тестирование', 'Принципы написания юнит-тестов (Google Test, Catch2) и использования Test-Driven Development (TDD) в C++.');

END $$;
