SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 13: Лучшие практики...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 13: Лучшие практики', 'Правильное управление ресурсами, архитектура, сборка и отладка программ.')
    RETURNING id INTO v_mod_id;

    -- 13.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.1. Smart-pointers (Умные указатели)', 'В современном C++ ручное управление памятью (new / delete) считается моветоном. Вместо сырых указателей используют "умные", которые автоматически очищают память (полагаясь на идиому RAII).

`std::unique_ptr<T>` — эксклюзивно владеет объектом. Копировать его нельзя, можно только перемещать. Когда `unique_ptr` выходит из области видимости, объект удаляется.
  auto ptr = std::make_unique<int>(42);

`std::shared_ptr<T>` — реализует совместное владение через подсчет ссылок (Reference Counting). Объект удалится, когда будет уничтожен последний `shared_ptr`, указывающий на него.');
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 13.1
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Умный указатель', 
        'Напишите функцию `int get_val(std::unique_ptr<int>& ptr)`, которая возвращает значение по указателю, если он не пуст, иначе возвращает 0. Функция `main` уже написана.

Формат ввода:
Одно целое число (если 0, создается пустой указатель).

Формат вывода:
То же число или 0.

Пример:
Ввод: 42
Вывод: 42', 
        E'#include <iostream>\n#include <memory>\n\nint get_val(const std::unique_ptr<int>& ptr) {\n    // Ваш код\n}\n\nint main() {\n    int n;\n    if (std::cin >> n) {\n        std::unique_ptr<int> p = nullptr;\n        if (n != 0) p = std::make_unique<int>(n);\n        std::cout << get_val(p) << "\\n";\n    }\n    return 0;\n}',
        'main,return,int,void,#include,iostream,memory,unique_ptr,cin,cout', 
        '#define,goto,asm,__asm__,__asm,new,delete,malloc,free', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '42', '42\n', TRUE),
           (v_task_id, '0', '0\n', TRUE);

    -- 13.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.2. RAII', 'RAII (Resource Acquisition Is Initialization) — ключевая идиома C++. Ресурс (память, файл, мьютекс) захватывается в конструкторе объекта (на стеке) и гарантированно освобождается в его деструкторе, даже если происходит выход из функции по return или выброс исключения.
Это делает код безопасным и защищенным от утечек.');

    -- 13.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.3. Move-семантика', 'Семантика перемещения (std::move и rvalue-ссылки &&) позволяет эффективно "красть" ресурсы у временных объектов вместо их дорогостоящего копирования.
Если вектор возвращается из функции по значению, раньше он полностью копировался. Теперь его внутренний буфер просто перепривязывается к новому владельцу!');

    -- 13.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.4. Правило пяти', 'Если класс управляет ресурсом (например, делает `new` в конструкторе), ему нужно определить все пять специальных методов:
1. Деструктор (чтобы сделать `delete`).
2. Конструктор копирования.
3. Оператор присваивания копированием.
4. Конструктор перемещения.
5. Оператор присваивания перемещением.

Если вы можете избежать ручного управления ресурсом, используя `std::vector` или `std::unique_ptr` как поля класса, компилятор сам сгенерирует правильные версии этих методов (Правило нуля).');

    -- 13.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '13.5. SOLID и Паттерны', 'SOLID — это пять принципов (Single Responsibility, Open-Closed, Liskov Substitution, Interface Segregation, Dependency Inversion), которые делают архитектуру гибкой.
В сочетании с паттернами проектирования (GoF: Singleton, Factory, Observer) они позволяют строить легко поддерживаемые enterprise-приложения на C++.');

END $$;
