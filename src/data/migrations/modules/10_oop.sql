SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 10: ООП...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 10: ООП', 'Глубокое погружение в объектно-ориентированное программирование на C++.')
    RETURNING id INTO v_mod_id;

    -- 10.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.1. Конструкторы', 'Конструкторы инициализируют объекты. В C++ также есть деструкторы, конструкторы копирования и списки инициализации членов.');

    -- 10.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.2. Operator overloading. Функторы', 'Перегрузка операторов (operator+) позволяет использовать стандартные операторы для пользовательских типов. Функторы — это классы с перегруженным operator().');

    -- 10.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.3. Наследование. Базовый и производный классы', 'Наследование позволяет создавать новые классы на основе существующих, расширяя или изменяя их функциональность.');

    -- 10.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.4. Защита при наследовании', 'Спецификатор protected разрешает доступ к членам класса для производных классов, но закрывает его снаружи. Также существуют public, protected и private типы наследования.');

    -- 10.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.5. Полиморфизм в C++. virtual', 'Ключевое слово virtual разрешает позднее связывание, позволяя вызывать метод производного класса через указатель на базовый класс (динамический полиморфизм).');

    -- 10.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.6. Чисто виртуальные функции. override', 'Чисто виртуальная функция (virtual void foo() = 0;) делает класс абстрактным. Ключевое слово override заставляет компилятор проверять правильность переопределения метода.');

    -- 10.7
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.7. Вызов в обход механизма виртуальности', 'Принудительный вызов реализации базового класса осуществляется путем явного указания области видимости: base_ptr->Base::method().');

END $$;
