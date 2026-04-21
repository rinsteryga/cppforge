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
    (v_mod_id, '14.1. Роадмапы и дальнейшее изучение', 'Краткий обзор путей развития C++ разработчика: GameDev, Embedded, High-Load системы, HFT, системное программирование.');

    -- 14.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '14.2. Заключение', 'Поздравляем с завершением курса! Советы по поиску первых open-source проектов для практики и подготовке к собеседованиям.');

END $$;
