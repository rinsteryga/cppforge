SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 10: ООП...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 10: ООП', 'Глубокое погружение в объектно-ориентированное программирование на C++.')
    RETURNING id INTO v_mod_id;

    -- 10.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '10.1. Жизненный цикл объекта', 'Конструктор инициализирует объект. В C++ крайне важно использовать **список инициализации членов** (`member initializer list`).

**Почему это лучше тела конструктора?**
1. **Производительность**: данные инициализируются сразу нужными значениями, а не сначала по умолчанию, а потом присваиванием.
2. **Обязательность**: константы и ссылки можно инициализировать **только** так.

```cpp
class Entity {
    const int id;
public:
    Entity(int i) : id(i) {} // Список инициализации
    ~Entity() { /* Очистка ресурсов */ }
};
```', 1)
    RETURNING id INTO v_lesson_id;

    -- 10.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '10.2. Перегрузка операторов. Функторы', 'C++ позволяет переопределять поведение стандартных операторов (+, -, *, ==) для ваших собственных классов. Это делает код более выразительным: `vec3 = vec1 + vec2;` вместо `vec3 = vec1.add(vec2);`.

```cpp
Vector operator+(const Vector& other) const {
    return Vector(x + other.x, y + other.y);
}
```

Если вы перегрузите оператор вызова функции `operator()`, ваш класс превратится в функтор (функциональный объект). Функторы можно передавать в алгоритмы STL так же, как и обычные функции или лямбды.', 2);

    -- 10.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '10.3. Наследование (IS-A)', 'Наследование позволяет строить иерархии. Если `Dog` наследуется от `Animal`, мы говорим: "Dog **IS-A** Animal" (Собака — это Животное).
 
 ```cpp
 class Animal {
 public:
     void breathe() {}
 };
 
 class Dog : public Animal { // Публичное наследование
 public:
     void bark() {}
 };
 ```
 Объекты `Dog` получают все публичные и защищенные методы `Animal`.', 3)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 10.3
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Наследование Shape', 
        'Реализуйте базовый класс `Shape` с публичным методом `void printType()`, который выводит строку "Shape". Затем реализуйте производный класс `Circle` (наследующийся от `Shape`), который добавляет метод `void printRadius()`, выводящий "Radius: X", где X — переданное в конструктор число.

Формат ввода:
Одно целое число — радиус.

Формат вывода:
Две строки. Сначала вывод `printType()`, затем `printRadius()`.

Пример:
Ввод: 5
Вывод: Shape
Radius: 5', 
        E'#include <iostream>\n\n// Базовый класс Shape\n\n// Производный класс Circle\n\nint main() {\n    int r;\n    if (std::cin >> r) {\n        Circle c(r);\n        c.printType();\n        c.printRadius();\n    }\n    return 0;\n}',
        'class,public', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', E'Shape\nRadius: 5', TRUE),
           (v_task_id, '10', E'Shape\nRadius: 10', TRUE);

    -- 10.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '10.4. Защита при наследовании', 'Мы знаем про public и private. При наследовании появляется потребность в спецификаторе protected. 
Члены класса, помеченные как protected, недоступны "снаружи" класса (как private), но ДОСТУПНЫ в производных классах!

Само наследование тоже имеет спецификатор (например, `class Dog : public Animal`).
- public наследование: protected члены остаются protected, public остаются public (интерфейс сохраняется, работает отношение "IS-A").
- private наследование: всё унаследованное становится private внутри производного класса.', 4);

    -- 10.5
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '10.5. Полиморфизм и virtual', 'Полиморфизм — это способность кода работать с объектами разных типов через один интерфейс.
 
 В C++ для этого используются **виртуальные функции**. Когда вы помечаете метод как `virtual`, компилятор создает **VTable** (таблицу виртуальных функций).
 
 **Как это работает?**
 При вызове метода через указатель на базовый класс, программа заглядывает в VTable конкретного объекта в рантайме и находит там правильный адрес функции (например, `Dog::speak` вместо `Animal::speak`).
 
 ```cpp
 virtual void speak() { std::cout << "???"; }
 ```', 5);
    
    -- 10.6
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '10.6. Абстрактные классы и override', 'Чисто виртуальная функция (`= 0`) делает класс **абстрактным**. Его нельзя создать напрямую — это "интерфейс", который обязывает наследников реализовать метод.
 
 ```cpp
 virtual void draw() = 0; // Чисто виртуальная
 ```
 
 **`override`**: (C++11) Всегда пишите это слово при переопределении метода. Оно заставляет компилятор проверить, что вы точно переопределяете виртуальную функцию, а не создали новую из-за опечатки в названии или параметрах.', 6)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 10.5-10.6
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Виртуальные методы', 
        'Создайте абстрактный класс `Animal` с чисто виртуальной функцией `void speak()`. Затем создайте классы `Cat` и `Dog`, наследующиеся от `Animal`. 
В классе `Cat` функция `speak()` должна выводить "Meow". В классе `Dog` — "Woof".
Функция `main` (уже написана) через указатель на базовый класс вызовет нужный метод.

Формат ввода:
Символ ''C'' или ''D''.

Формат вывода:
"Meow" для ''C'', "Woof" для ''D''.

Пример:
Ввод: C
Вывод: Meow', 
        E'#include <iostream>\n\n// Абстрактный класс Animal\n\n// Класс Cat\n\n// Класс Dog\n\nint main() {\n    char t;\n    if (std::cin >> t) {\n        Animal *a = nullptr;\n        if (t == ''C'') a = new Cat();\n        else if (t == ''D'') a = new Dog();\n        \n        if (a) {\n            a->speak();\n            delete a;\n        }\n    }\n    return 0;\n}',
        'class,public,virtual,override', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, 'C', 'Meow', TRUE),
           (v_task_id, 'D', 'Woof', TRUE);

    -- 10.7
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '10.7. Вызов в обход механизма виртуальности', 'Иногда внутри переопределенного метода нужно вызвать реализацию базового класса (например, чтобы дополнить её, а не полностью заменить).
Это делается явным указанием области видимости:

```cpp
void Dog::speak() {
    Animal::speak(); // Вызовет базовую реализацию, игнорируя vtable
    std::cout << " (And a Woof!)";
}
```', 7);

END $$;
