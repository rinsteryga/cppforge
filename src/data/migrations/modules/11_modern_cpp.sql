SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 11: Современный C++...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 11: Современный C++', 'Возможности стандартов C++11/14/17/20.')
    RETURNING id INTO v_mod_id;

    -- 11.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '11.1. Вывод типов: auto и decltype', $content$Современный C++ избавляет от необходимости писать длинные имена типов.
 
 - **`auto`**: компилятор сам определяет тип переменной по значению, которым она инициализируется.
 - **`decltype(expr)`**: позволяет узнать тип выражения без его вычисления.
 
 ```cpp
 auto x = 5; // int
 auto y = 3.14; // double
 std::vector<int> v;
 auto it = v.begin(); // вместо std::vector<int>::iterator
 ```
 **Важно**: `auto` нельзя использовать без инициализации, так как компилятору не из чего выводить тип.$content$, 1)
    RETURNING id INTO v_lesson_id;

    -- 11.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '11.2. Единообразная инициализация', $content$До C++11 существовало много способов инициализации. Теперь есть один универсальный — фигурные скобки `{}`.
 
 ```cpp
 int x{5};
 std::vector<int> v{1, 2, 3};
 ```
 **Преимущество**: Фигурные скобки запрещают "сужающие" преобразования (narrowing conversions). Например, `int x = 3.14;` скомпилируется (с потерей данных), а `int x{3.14};` — выдаст ошибку.$content$, 2);

    -- 11.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '11.3. Атрибуты: [[nodiscard]] и другие', $content$Атрибуты дают компилятору подсказки, чтобы он лучше проверял ваш код.
 
 **Популярные атрибуты**:
 - `[[nodiscard]]`: указывает, что результат функции **обязательно** должен быть использован. Помогает найти ошибки, когда забыли проверить код возврата.
 - `[[maybe_unused]]`: подавляет предупреждение о неиспользуемой переменной.
 - `[[deprecated]]`: помечает функцию как устаревшую.
 - `[[fallthrough]]`: указывает, что отсутствие `break` в `switch` было преднамеренным.$content$, 3);

    -- 11.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '11.4. Пространства имен (namespace)', $content$Библиотеки C++ (STL) живут в пространстве имен `std`.
Хотя вы можете написать `using namespace std;`, это считается плохой практикой в больших проектах.
 
 **Почему?** 
 Если вы подключите две библиотеки и в обеих будет функция `sort`, возникнет конфликт имен (Name Clash).
 Явное указание `std::sort` гарантирует, что вы вызываете именно то, что хотели.$content$, 4);

END $$;
