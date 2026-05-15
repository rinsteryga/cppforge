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
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '13.1. Smart-pointers (Умные указатели)', 'Ручное управление памятью через `new` и `delete` — это главный источник багов (утечки, двойное освобождение). В современном C++ мы используем умные указатели из `<memory>`:

1. **`std::unique_ptr<T>`**: Эксклюзивное владение. 
   - Нельзя копировать, можно только перемещать (`std::move`). 
   - Нулевые накладные расходы (Zero-cost abstraction).
```cpp
auto ptr = std::make_unique<int>(42);
// do_something(ptr); // Ошибка!
do_something(std::move(ptr)); // Теперь ок, ptr пуст
```

2. **`std::shared_ptr<T>`**: Совместное владение.
   - Использует счетчик ссылок. Объект удаляется, когда счетчик обнуляется.
   - Имеет накладные расходы на атомарный счетчик.
3. **`std::weak_ptr<T>`**: "Слабая" ссылка на объект, которым владеет `shared_ptr`. Не увеличивает счетчик. Нужен для разрыва циклических зависимостей.', 1)
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
        'unique_ptr', 
        '#define,goto,asm,__asm__,__asm,new,delete,malloc,free', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '42', '42', TRUE),
           (v_task_id, '0', '0', TRUE);

    -- 13.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '13.2. RAII — основа безопасности', 'RAII (Resource Acquisition Is Initialization) — это концепция, на которой держится весь C++. 
**Суть проста**: захват ресурса (память, файл, соединение, мьютекс) происходит в конструкторе, а освобождение — в деструкторе.

Поскольку деструкторы объектов на стеке вызываются **гарантированно** (даже при исключении), ресурс никогда не утечет.
Примеры RAII в стандарте: `std::vector`, `std::string`, `std::unique_ptr`, `std::lock_guard`.', 2);

    -- 13.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '13.3. Move-семантика и rvalue', 'До C++11 возврат больших объектов из функций приводил к дорогому копированию. Семантика перемещения позволяет "забрать" внутренности временного объекта вместо их копирования.

- **lvalue**: объект, у которого есть имя и адрес (можно взять `&`).
- **rvalue**: временный объект без имени (результат выражения).

Функция `std::move` преобразует lvalue в rvalue-ссылку (`T&&`), сообщая компилятору: "я больше не использую этот объект, его данные можно забрать".
```cpp
std::string s1 = "Very long string";
std::string s2 = std::move(s1); // s1 теперь пуста, s2 владеет данными
```', 3);

    -- 13.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '13.4. Правило пяти (и Rule of Zero)', 'Если ваш класс вручную управляет ресурсом (указателем), вам нужно определить:
1. **Деструктор** (освобождение).
2. **Конструктор копирования** (глубокое копирование).
3. **Оператор присваивания копированием**.
4. **Конструктор перемещения** (move constructor).
5. **Оператор присваивания перемещением**.

**Rule of Zero**: Старайтесь проектировать классы так, чтобы они использовали готовые RAII-объекты (`std::vector`, `std::unique_ptr`). Тогда вам не придется писать ни один из этих методов — компилятор сделает всё правильно сам.', 4);

    -- 13.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '13.5. Принципы SOLID', 'Чтобы код не превратился в спагетти, следуйте принципам SOLID:
- **S** (Single Responsibility): Один класс — одна задача.
- **O** (Open-Closed): Класс открыт для расширения, но закрыт для модификации.
- **L** (Liskov Substitution): Наследник должен полностью заменять родителя.
- **I** (Interface Segregation): Лучше много маленьких интерфейсов, чем один огромный.
- **D** (Dependency Inversion): Зависьте от абстракций, а не от реализаций.

Знание паттернов (Singleton, Factory, Observer) помогает решать типовые задачи проектирования на "стандартном" языке архитекторов.', 5);

END $$;
