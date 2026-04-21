SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 9: Функции и ООП-подход...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 9: Функции и ООП-подход', 'Продвинутые аспекты работы с функциями и инкапсуляция.')
    RETURNING id INTO v_mod_id;

    -- 9.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.1. Function overloading', 'Перегрузка функций позволяет создавать несколько функций с одним именем, но разным набором параметров.');

    -- 9.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.2. Параметры по умолчанию', 'В C++ можно задавать значения по умолчанию для параметров. Они должны указываться строго с конца (справа налево).');

    -- 9.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.3. inline. Некоторые особенности компиляторов', 'Ключевое слово inline является подсказкой компилятору встроить код функции в место вызова, чтобы ускорить выполнение.');

    -- 9.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.4. class. Основы инкапсуляции', 'Классы в C++ — это фундаментальная концепция ООП. Инкапсуляция скрывает внутреннюю реализацию за спецификатором private и открывает интерфейс через public.');

    -- 9.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.5. Неявный указатель this', 'Внутри нестатических методов класса доступен неявный константный указатель this, указывающий на текущий экземпляр класса.');

    -- 9.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.6. Лямбда-выражения', 'Лямбда-выражения позволяют создавать анонимные функции по месту их использования с возможностью захвата переменных контекста: [захват](параметры){тело}.');

    -- 9.7
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.7. static', 'Статические переменные внутри функций сохраняют свое значение между вызовами. Статические члены класса принадлежат самому классу, а не его объектам.');

END $$;
