SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 14: Развитие...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 14: Развитие', 'Дальнейшие шаги и пути развития в мире C++.')
    RETURNING id INTO v_mod_id;

    -- 14.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '14.1. Пути развития C++ разработчика', 'C++ — огромный язык, и никто не знает его на 100%. Выберите специализацию:
1. GameDev (Unreal Engine).
2. Embedded / IoT (Программирование микроконтроллеров).
3. High-Load и HFT (Высокочастотный трейдинг).
4. Системное программирование (Драйвера, базы данных, браузеры).
Каждая сфера требует изучения своих специфичных библиотек и подходов.');

    -- 14.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '14.2. Заключение', 'Поздравляем с прохождением базового курса по C++! 
Следующий шаг — писать код. Много кода. Придумайте свой проект (например, простенький веб-сервер, эмулятор или игру) и реализуйте его. Читайте чужой код на GitHub, изучайте стандарты (cppreference) и готовьтесь к собеседованиям.

Удачи!');

END $$;
