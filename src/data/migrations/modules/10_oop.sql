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
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.1. Конструкторы и Деструкторы', 'Конструктор — это специальный метод, который автоматически вызывается при создании объекта. Он имеет то же имя, что и класс, и не возвращает тип (даже void).
Его задача — инициализировать объект, выделить необходимые ресурсы.

  class Entity {
  public:
      Entity() { std::cout << "Created!"; } // Конструктор по умолчанию
      Entity(int x) { ... } // Параметризованный конструктор
  };

Деструктор вызывается при уничтожении объекта (выходе из области видимости или вызове delete). Имя деструктора — имя класса с тильдой: ~Entity(). Он освобождает ресурсы.

Также в C++ существует список инициализации членов (Member Initializer List), который эффективнее обычного присваивания в теле конструктора:
  Entity(int x) : m_x(x) {}')
    RETURNING id INTO v_lesson_id;

    -- 10.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.2. Перегрузка операторов. Функторы', 'C++ позволяет переопределять поведение стандартных операторов (+, -, *, ==) для ваших собственных классов. Это делает код более выразительным: `vec3 = vec1 + vec2;` вместо `vec3 = vec1.add(vec2);`.

  Vector operator+(const Vector& other) const {
      return Vector(x + other.x, y + other.y);
  }

Если вы перегрузите оператор вызова функции `operator()`, ваш класс превратится в функтор (функциональный объект). Функторы можно передавать в алгоритмы STL так же, как и обычные функции или лямбды.');

    -- 10.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.3. Наследование. Базовый и производный классы', 'Наследование — это механизм ООП, позволяющий создать новый класс (производный, derived) на основе существующего (базового, base). Производный класс наследует все поля и методы базового класса и может добавлять свои.

  class Animal {
  public:
      void eat() { std::cout << "Eating..."; }
  };

  class Dog : public Animal {
  public:
      void bark() { std::cout << "Bark!"; }
  };
Объект класса Dog может вызывать как bark(), так и унаследованный eat().')
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
Вывод: 
Shape
Radius: 5', 
        E'#include <iostream>\n\n// Базовый класс Shape\n\n// Производный класс Circle\n\nint main() {\n    int r;\n    if (std::cin >> r) {\n        Circle c(r);\n        c.printType();\n        c.printRadius();\n    }\n    return 0;\n}',
        'main,return,int,void,#include,iostream,cin,cout,class,public', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', 'Shape\nRadius: 5\n', TRUE),
           (v_task_id, '10', 'Shape\nRadius: 10\n', TRUE);

    -- 10.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.4. Защита при наследовании', 'Мы знаем про public и private. При наследовании появляется потребность в спецификаторе protected. 
Члены класса, помеченные как protected, недоступны "снаружи" класса (как private), но ДОСТУПНЫ в производных классах!

Само наследование тоже имеет спецификатор (например, `class Dog : public Animal`).
- public наследование: protected члены остаются protected, public остаются public (интерфейс сохраняется, работает отношение "IS-A").
- private наследование: всё унаследованное становится private внутри производного класса.');

    -- 10.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.5. Полиморфизм в C++. virtual', 'Полиморфизм позволяет обращаться к объектам производных классов через указатель или ссылку на базовый класс.

  Animal* a = new Dog();
  a->speak(); // Какая функция вызовется?

По умолчанию в C++ вызов функций ранний (раннее связывание, на этапе компиляции). Компилятор видит, что тип указателя `Animal*`, и вызовет `Animal::speak()`.
Чтобы метод стал полиморфным (динамическое связывание, позднее), нужно использовать ключевое слово `virtual` в базовом классе.

  virtual void speak() { ... }

Тогда компилятор создаст таблицу виртуальных функций (vtable), и вызов `a->speak()` обратится к vtable объекта в рантайме, найдя и вызвав именно `Dog::speak()`.');
    
    -- 10.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.6. Чисто виртуальные функции. override', 'Если виртуальной функции не нужна реализация в базовом классе, мы можем сделать её "чисто виртуальной" (pure virtual):
  virtual void draw() = 0;

Класс, содержащий хотя бы одну такую функцию, становится абстрактным. Вы не можете создать объект абстрактного класса (нельзя сделать `new Animal()`). Все производные классы ОБЯЗАНЫ переопределить эту функцию, иначе они тоже станут абстрактными.

Ключевое слово `override` (из C++11) пишется в конце сигнатуры метода в производном классе. Оно заставляет компилятор строго проверить, действительно ли метод с такой сигнатурой существует в базовом классе и является виртуальным. Это защищает от опечаток!')
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
        'main,return,int,void,#include,iostream,cin,cout,class,public,virtual,override', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, 'C', 'Meow\n', TRUE),
           (v_task_id, 'D', 'Woof\n', TRUE);

    -- 10.7
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '10.7. Вызов в обход механизма виртуальности', 'Иногда внутри переопределенного метода нужно вызвать реализацию базового класса (например, чтобы дополнить её, а не полностью заменить).
Это делается явным указанием области видимости:

  void Dog::speak() {
      Animal::speak(); // Вызовет базовую реализацию, игнорируя vtable
      std::cout << " (And a Woof!)";
  }');

END $$;
