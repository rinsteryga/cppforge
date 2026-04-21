SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 11: Современный подход...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 11: Современный подход', 'Шаблоны, обработка исключений и новые стандарты C++.')
    RETURNING id INTO v_mod_id;

    -- 11.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.1. Шаблоны...', 'Шаблоны (templates) позволяют писать обобщенный код для любых типов данных, перекладывая генерацию конкретных функций или классов на компилятор.');

    -- 11.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.2. Немного про try/catch/throw. noexcept', 'Исключения обеспечивают механизм обработки ошибок времени выполнения. Ключевое слово noexcept гарантирует, что функция не бросит исключение.');

    -- 11.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.3. Атрибуты', 'Атрибуты, такие как [[nodiscard]], [[maybe_unused]] или [[deprecated]], дают компилятору дополнительные подсказки для генерации предупреждений.');

    -- 11.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.4. namespace std', 'Стандартная библиотека C++ (STL) целиком находится в пространстве имен std. Использование using namespace std; в заголовочных файлах считается плохой практикой.');

END $$;
