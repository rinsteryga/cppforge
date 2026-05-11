SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 11: Современный подход...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 11: Современный подход', 'Шаблоны, обработка исключений и новые стандарты C++.')
    RETURNING id INTO v_mod_id;

    -- 11.1
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.1. Шаблоны (Templates)', 'Шаблоны позволяют писать обобщенный код для любых типов данных. Вы пишете "каркас", а компилятор сам генерирует (инстанцирует) нужные версии функций или классов для каждого типа, с которым они используются.

Пример шаблонной функции:
  template <typename T>
  T my_max(T a, T b) {
      return (a > b) ? a : b;
  }

Вызов: `my_max<int>(5, 10)` или просто `my_max(5.5, 3.1)` (компилятор часто сам может вывести тип аргументов).
Это краеугольный камень библиотеки STL (Standard Template Library), где все контейнеры являются шаблонными классами (например, `std::vector<int>`).')
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 11.1
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Шаблон функции', 
        'Напишите шаблонную функцию `T get_min(T a, T b)`, которая возвращает минимальное из двух значений. Функция `main` проверит ее работу на целых и вещественных числах.

Формат ввода:
Четыре числа (два целых, два вещественных).

Формат вывода:
Два числа — минимальное из первой пары и минимальное из второй пары.

Пример:
Ввод: 10 5 3.14 2.71
Вывод: 
5
2.71', 
        E'#include <iostream>\n\n// Напишите шаблон get_min здесь\n\nint main() {\n    int i1, i2;\n    double d1, d2;\n    if (std::cin >> i1 >> i2 >> d1 >> d2) {\n        std::cout << get_min(i1, i2) << "\\n";\n        std::cout << get_min(d1, d2) << "\\n";\n    }\n    return 0;\n}',
        'main,return,int,void,double,#include,iostream,cin,cout,template,typename,class', 
        '#define,goto,asm,__asm__,__asm,scanf,printf', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '10 5 3.14 2.71', '5\n2.71\n', TRUE),
           (v_task_id, '-1 -5 0.5 0.9', '-5\n0.5\n', TRUE);

    -- 11.2
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.2. Исключения (try/catch/throw). noexcept', 'Исключения обеспечивают механизм обработки ошибок времени выполнения. Если функция сталкивается с проблемой (например, деление на ноль или нехватка памяти), она может "выбросить" исключение с помощью throw:
  if (b == 0) throw std::runtime_error("Divide by zero!");

Где-то выше по стеку вызовов это исключение можно "поймать" и обработать:
  try {
      int res = divide(a, b);
  } catch (const std::exception& e) {
      std::cout << "Error: " << e.what();
  }

Ключевое слово noexcept, добавленное в конце сигнатуры функции, является строгой гарантией того, что эта функция никогда не бросит исключение. Это позволяет компилятору и контейнерам STL делать мощные оптимизации.');

    -- 11.3
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.3. Атрибуты (Attributes)', 'Атрибуты — это унифицированный синтаксис (появился в C++11), позволяющий давать подсказки компилятору. Они пишутся в двойных квадратных скобках.

Самые полезные атрибуты:
- [[nodiscard]]: Если функция помечена этим атрибутом, компилятор выдаст предупреждение, если программист вызовет её, но проигнорирует возвращаемое значение. Полезно для функций, возвращающих код ошибки.
- [[maybe_unused]]: Подавляет предупреждение "unused variable" для переменных, которые логически нужны, но формально не используются.
- [[deprecated("Use V2")]]: Выдаст предупреждение при попытке использовать устаревшую функцию или класс.');

    -- 11.4
    INSERT INTO lessons (module_id, title, content) VALUES
    (v_mod_id, '11.4. namespace std и плохие практики', 'Стандартная библиотека C++ (STL) целиком находится в пространстве имен std. 
Мы часто пишем `using namespace std;` в начале файлов (.cpp), чтобы сэкономить время на наборе текста.

Однако это считается плохой практикой в серьезном (production) коде. 
Почему? Потому что пространство std огромно! Если вы подключите <algorithm> и напишете `using namespace std;`, а затем создадите свою функцию `min()`, `count()` или `sort()`, возникнет конфликт имен (Name Clash), который очень тяжело отлаживать.
Ещё хуже — писать `using namespace std;` в заголовочных файлах (.h или .hpp). Это "отравит" глобальное пространство имен для всех других файлов, которые заинклудят ваш заголовок!');

END $$;
