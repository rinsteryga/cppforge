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
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.1. Function overloading', 'Перегрузка функций позволяет создавать несколько функций с одним и тем же именем, но разным набором параметров (по типу или количеству). Компилятор C++ умеет по переданным аргументам во время компиляции определять, какую именно функцию вызвать.
Это называется статическим полиморфизмом (Static Polymorphism).

Пример:
  int add(int a, int b) { return a + b; }
  double add(double a, double b) { return a + b; }
  
Вызов add(5, 5) вызовет первую функцию, а add(2.5, 3.1) — вторую. Важно понимать, что возвращаемый тип не участвует в разрешении перегрузки — функции должны отличаться именно списком параметров!')
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 9.1
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Перегрузка print', 
        'Реализуйте две функции `print` (используя `std::cout`): одна принимает `int` и выводит строку "int: X", вторая принимает `double` и выводит "double: X". В функции `main` обе будут вызваны с разными типами.

Формат ввода:
Одно число (целое или вещественное).

Формат вывода:
Соответствующая строка.

Пример:
Ввод: 42
Вывод: int: 42', 
        E'#include <iostream>\n\n// Ваши функции print\n\nint main() {\n    int a;\n    double b;\n    if (std::cin >> a >> b) {\n        print(a);\n        print(b);\n    }\n    return 0;\n}',
        'main,return,int,void,double,#include,iostream,cin,cout', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '10 3.14', 'int: 10\ndouble: 3.14\n', TRUE);

    -- 9.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.2. Параметры по умолчанию', 'В C++ можно задавать значения по умолчанию для параметров функции. Это избавляет от необходимости писать несколько перегруженных функций.

Значения по умолчанию должны указываться в объявлении функции и строго с конца списка параметров (справа налево).
Пример:
  void printMessage(std::string msg, int count = 1) { ... }

Вы можете вызвать эту функцию как printMessage("Hello", 3) или просто printMessage("Hello") — во втором случае count автоматически станет равен 1.');

    -- 9.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.3. inline. Некоторые особенности компиляторов', 'Ключевое слово inline является подсказкой (hint) компилятору встроить тело функции прямо в место её вызова, вместо того чтобы выполнять дорогостоящую инструкцию CALL (прыжок в памяти). Это может ускорить программу.

Однако современные компиляторы (GCC, Clang) настолько умны, что часто игнорируют inline и сами решают, какую функцию встраивать (особенно с флагами оптимизации -O2, -O3). Тем не менее, inline имеет и другой важный эффект: он позволяет определять функцию прямо в заголовочном файле без получения ошибки "Multiple definition" от линковщика.');

    -- 9.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.4. class. Основы инкапсуляции', 'Классы — это фундаментальная концепция ООП в C++. Класс объединяет данные (поля) и функции, которые с ними работают (методы), в одну сущность.

Инкапсуляция (сокрытие данных) обеспечивается спецификаторами доступа:
- public: доступно из любого места.
- private: доступно только из методов этого же класса.

По умолчанию в C++ все члены `class` являются private, а все члены `struct` — public. Это единственное реальное отличие struct от class в C++!

Пример:
  class BankAccount {
  private:
      int balance;
  public:
      void deposit(int amount) { balance += amount; }
  };
Мы не можем напрямую изменить balance снаружи, только через метод deposit.')
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 9.4
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Класс Counter', 
        'Напишите класс `Counter` с приватным полем `value` (инициализированным 0). Реализуйте публичные методы `void increment()` (увеличивает value на 1) и `int get()` (возвращает value). В функции `main` создайте объект класса и вызовите `increment` N раз.

Формат ввода:
Целое число N (от 0 до 1000).

Формат вывода:
Число N (значение счетчика).

Пример:
Ввод: 5
Вывод: 5', 
        E'#include <iostream>\n\n// Ваш класс Counter\n\nint main() {\n    int n;\n    if (std::cin >> n) {\n        Counter c;\n        for(int i = 0; i < n; ++i) c.increment();\n        std::cout << c.get() << "\\n";\n    }\n    return 0;\n}',
        'main,return,int,void,#include,iostream,cin,cout,class,private,public', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', '5\n', TRUE),
           (v_task_id, '0', '0\n', TRUE);

    -- 9.5
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.5. Неявный указатель this', 'Внутри нестатических методов класса всегда доступен неявный указатель `this`. Он содержит адрес объекта, для которого был вызван метод.

Когда вы пишете:
  void setValue(int value) {
      this->value = value;
  }
Компилятор понимает, что `this->value` относится к полю класса, а `value` — к параметру функции. Это полезно для разрешения конфликта имен.');

    -- 9.6
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.6. Лямбда-выражения', 'Лямбда-выражения (появились в C++11) позволяют создавать анонимные функции (замыкания) прямо в месте их использования.
Синтаксис: [захват](параметры) -> возвращаемый_тип { тело }

Захват (capture list) позволяет лямбде "видеть" переменные из окружающей области:
- [=] — захват всего по значению (копия).
- [&] — захват всего по ссылке.
- [x] — захват только переменной x по значению.

Пример:
  int factor = 2;
  auto multiply = [factor](int n) { return n * factor; };
  std::cout << multiply(5); // Выведет 10');

    -- 9.7
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '9.7. static', 'Ключевое слово static имеет два основных применения в C++:
1. Локальные статические переменные в функциях сохраняют свое значение между вызовами. Они инициализируются только один раз.
2. Статические члены класса. Поля и методы, помеченные static, принадлежат самому классу, а не конкретному объекту. Для их вызова не нужен экземпляр: `ClassName::staticMethod()`.');

END $$;
