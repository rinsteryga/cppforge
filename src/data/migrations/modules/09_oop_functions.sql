SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 9: Функции и ООП-подход...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 9: Функции и ООП-подход', 'Продвинутые аспекты работы с функциями и инкапсуляция.')
    RETURNING id INTO v_mod_id;

    -- 9.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '9.1. Перегрузка функций', $content$Перегрузка (Overloading) позволяет создавать функции с одним именем, но разными параметрами. 
**Как это работает?** Компилятор применяет "декорирование имен" (Name Mangling), создавая уникальные внутренние имена для каждой версии функции на основе типов её аргументов.

**Важно**: возвращаемый тип **не участвует** в перегрузке. Нельзя создать две функции `int foo()` и `void foo()`, так как компилятор не поймет, какую вызывать при обычном вызове `foo();`.

Пример:
```cpp
void print(int x);
void print(double x);
```$content$, 1)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 9.1
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Перегрузка print', 
        $description$Реализуйте две функции `print` (используя `std::cout`): одна принимает `int` и выводит строку "int: X", вторая принимает `double` и выводит "double: X". В функции `main` обе будут вызваны с разными типами.

Формат ввода:
Одно число (целое или вещественное).

Формат вывода:
Соответствующая строка.

Пример:
Ввод: 42
Вывод: int: 42$description$, 
        E'#include <iostream>\n\n// Ваши функции print\n\nint main() {\n    int a;\n    double b;\n    if (std::cin >> a >> b) {\n        print(a);\n        print(b);\n    }\n    return 0;\n}',
        'print', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '10 3.14', E'int: 10\ndouble: 3.14', TRUE);

    -- 9.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '9.2. Параметры по умолчанию', $content$C++ позволяет задавать значения "по умолчанию". Если аргумент не передан при вызове, используется это значение.
 
 **Правило**: параметры со значениями по умолчанию должны идти **строго в конце** списка (справа налево).
 
 ```cpp
 void log(std::string msg, int level = 0); 
 log("Hello"); // level будет 0
 ```$content$, 2);

    -- 9.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '9.3. inline-функции', $content$Ключевое слово `inline` — это рекомендация компилятору встроить код функции прямо в место вызова вместо создания отдельного прыжка (jump) в памяти.
 
 **Зачем это нужно?**
  1. **Производительность**: убирает накладные расходы на вызов функции (актуально для очень маленьких функций).
  2. **ODR (One Definition Rule)**: `inline` позволяет определять функцию в заголовочных файлах без ошибок "multiple definition" при линковке.$content$, 3);

    -- 9.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '9.4. Инкапсуляция и классы', $content$Класс — это чертеж объекта. Инкапсуляция позволяет скрыть детали реализации и защитить данные.
 
 **Спецификаторы доступа**:
  - `public`: доступно всем.
  - `private`: доступно только внутри класса.
   
 **Разница между class и struct**:
  Единственное отличие в C++ — это доступ по умолчанию. В `class` всё `private`, в `struct` — `public`. 
 
 ```cpp
 class Player {
     int health; // private по умолчанию
 public:
     void heal(int hp) { health += hp; }
 };
 ```$content$, 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 9.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Класс Counter', 
        $description$Напишите класс `Counter` с приватным полем `value` (инициализированным 0). Реализуйте публичные методы `void increment()` (увеличивает value на 1) и `int get()` (возвращает value). В функции `main` создайте объект класса и вызовите `increment` N раз.

Формат ввода:
Целое число N (от 0 до 1000).

Формат вывода:
Число N (значение счетчика).

Пример:
Ввод: 5
Вывод: 5$description$, 
        E'#include <iostream>\n\n// Ваш класс Counter\n\nint main() {\n    int n;\n    if (std::cin >> n) {\n        Counter c;\n        for(int i = 0; i < n; ++i) c.increment();\n        std::cout << c.get() << "\\n";\n    }\n    return 0;\n}',
        'class,private,public', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', '5', TRUE),
           (v_task_id, '0', '0', TRUE);

    -- 9.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '9.5. Неявный указатель this', $content$Внутри нестатических методов класса всегда доступен неявный указатель `this`. Он содержит адрес объекта, для которого был вызван метод.

Когда вы пишете:
```cpp
void setValue(int value) {
    this->value = value;
}
```
Компилятор понимает, что `this->value` относится к полю класса, а `value` — к параметру функции. Это полезно для разрешения конфликта имен.$content$, 5);

    -- 9.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '9.6. Лямбда-выражения', $content$Лямбды — это анонимные функции, которые можно создавать "на лету". Они незаменимы для алгоритмов STL.
 
 **Синтаксис**: `[захват](параметры) { тело }`
 
 **Захват переменных**:
  - `[=]` — копирует внешние переменные внутрь лямбды (по значению).
  - `[&]` — передает переменные по ссылке.
  - `[this]` — захватывает текущий объект класса.
 
 ```cpp
 int limit = 10;
 auto is_over = [limit](int x) { return x > limit; };
 ```$content$, 6);

    -- 9.7
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '9.7. Ключевое слово static', $content$`static` в C++ имеет два лица:
 
  1. **Статические поля**: переменная принадлежит не объекту, а всему классу. Она существует в единственном экземпляре для всех созданных объектов.
  2. **Статические методы**: методы, которые можно вызывать без создания объекта (`ClassName::method()`). У них нет неявного указателя `this`.
 
  Это полезно для создания глобальных счетчиков или утилит.$content$, 7);

END $$;
