SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
    v_task_id BIGINT;
    v_quiz_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding module: Функции...';

    INSERT INTO modules (name, description) 
    VALUES ('Функции', 'Объявление, определение, параметры и директивы препроцессора')
    RETURNING id INTO v_mod_id;

    -- Lesson 3.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '3.1. Объявление и Определение', $content$Функция — это строительный блок программы. Она позволяет разбить сложную задачу на мелкие части.

**Важные термины**:
- **Объявление (Прототип)**: сообщает компилятору имя, возвращаемый тип и типы аргументов. Позволяет вызывать функцию до того, как она будет описана полностью.
- **Определение**: содержит само тело функции (код).

```cpp
int power(int base, int exp); // Объявление

int main() {
    return power(2, 8); // Компилятор знает, что такая функция будет
}

int power(int base, int exp) { /* реализация */ } // Определение
```$content$, 1)
    RETURNING id INTO v_lesson_id;

    -- Lesson 3.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '3.2. Передача параметров и Стек', $content$В C параметры передаются **только по значению**.

```cpp
void reset(int x) {
    x = 0; // Меняет только локальную копию
}

int main() {
    int a = 10;
    reset(a);
    printf("%d", a); // Всё еще 10!
}
```

Локальные переменные живут на **стеке**. Как только функция завершается, её память освобождается автоматически.$content$, 2)
    RETURNING id INTO v_lesson_id;

    -- Quiz
    INSERT INTO quizzes (lesson_id, title, question) VALUES (v_lesson_id, 'Передача параметров', 'Что произойдет с переменной "a", если мы передадим её в функцию foo(a), которая внутри себя умножает аргумент на 10?') RETURNING id INTO v_quiz_id;
    INSERT INTO quiz_options (quiz_id, option_text, is_correct) VALUES 
    (v_quiz_id, 'Останется неизменной', TRUE),
    (v_quiz_id, 'Умножится на 10', FALSE),
    (v_quiz_id, 'Вызовет ошибку сегментации', FALSE),
    (v_quiz_id, 'Превратится в глобальную переменную', FALSE);

    -- Lesson 3.3
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '3.3. Рекурсия и Stack Overflow', $content$Рекурсия — это когда функция вызывает саму себя.

**Пример: Факториал**:
```cpp
int fact(int n) {
    if (n <= 1) return 1; // Базовый случай
    return n * fact(n - 1); // Рекурсивный шаг
}
```

**Опасность**: Каждый вызов занимает место на стеке. Если забыть условие выхода, программа упадет с ошибкой **Stack Overflow**.$content$, 3)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 3.3
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Числа Фибоначчи', 
        $description$Напишите рекурсивную функцию int fib(int n), которая возвращает n-е число Фибоначчи.
F(0) = 0, F(1) = 1.
F(n) = F(n-1) + F(n-2) для n >= 2.
Функция main написана за вас.

Формат ввода:
Одно целое число N (от 0 до 30).

Формат вывода:
Одно целое число — N-е число Фибоначчи.

Пример:
Ввод: 5
Вывод: 5$description$, 
        E'#include <stdio.h>\n\n// Напишите функцию fib здесь\n\nint main(void) {\n    int n;\n    scanf("%d", &n);\n    printf("%d\\n", fib(n));\n    return 0;\n}',
        'fib', 
        '#define,goto,asm,__asm__,__asm,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '5', '5', TRUE),
           (v_task_id, '0', '0', TRUE),
           (v_task_id, '10', '55', TRUE);

    -- Lesson 3.4
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '3.4. Препроцессор: #include и #define', $content$До компиляции код проходит через препроцессор. Он не знает правил языка C, он просто манипулирует текстом.

- **`#include`**: просто копирует текст другого файла.
- **`#define`**: выполняет текстовую замену.

**Подвох макросов**:
Макросы — это небезопасно. 
```cpp
#define SQUARE(x) x * x
// Вызов SQUARE(1 + 2) превратится в 1 + 2 * 1 + 2 = 5! (а должно быть 9)
```
**Золотое правило**: всегда берите аргументы макроса в скобки: `((x) * (x))`, но лучше используйте константы.$content$, 4)
    RETURNING id INTO v_lesson_id;

    -- Coding Task for 3.1-3.2
    INSERT INTO coding_tasks (lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel)
    VALUES (
        v_lesson_id, 
        'Собственная функция', 
        $description$Определите функцию int is_even(int num), которая возвращает 1, если число четное, и 0, если нечетное. Функция main уже написана за вас (она считывает число и выводит результат работы is_even). Вам нужно только написать функцию.

Формат ввода:
Одно целое число.

Формат вывода:
Число 1, если четное, иначе 0.

Пример:
Ввод: 4
Вывод: 1$description$, 
        E'#include <stdio.h>\n\n// Напишите функцию is_even здесь\n\nint main(void) {\n    int n;\n    if (scanf("%d", &n) == 1) {\n        printf("%d\\n", is_even(n));\n    }\n    return 0;\n}',
        'is_even', 
        '#define,goto,asm,__asm__,__asm,struct,class,new,delete', 
        2000, 
        256,
        FALSE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES (v_task_id, '4', '1', TRUE),
           (v_task_id, '7', '0', TRUE);

END $$;
