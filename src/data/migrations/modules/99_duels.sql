SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding duels...';

    -- Task 1: Even or Odd
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Основы', '[ДУЭЛЬ] Четное или Нечетное', 
        'Напишите программу, которая принимает одно целое число со стандартного ввода и выводит "Even", если оно четное, и "Odd", если нечетное.

Формат ввода:
Одно целое число.

Формат вывода:
Слово "Even" или "Odd".

Пример:
Ввод: 4
Вывод: Even', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    if (std::cin >> n) {\n        // Ваш код\n    }\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '4', 'Even', TRUE), (v_task_id, '7', 'Odd', TRUE);

    -- Task 2: Reverse Integer
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Математика', '[ДУЭЛЬ] Разворот числа', 
        'Напишите программу, которая считывает целое число и выводит его же, но прочитанное справа налево. Знак сохраняется (если было отрицательное, остается отрицательным). Если число заканчивается на нули, они отбрасываются (например, 120 -> 21).

Формат ввода:
Одно целое 32-битное число.

Формат вывода:
Развернутое число.

Пример:
Ввод: -123
Вывод: -321', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '-123', '-321', TRUE), (v_task_id, '120', '21', TRUE), (v_task_id, '0', '0', TRUE);

    -- Task 3: Palindrome Number
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Алгоритмы', '[ДУЭЛЬ] Палиндром ли число?', 
        'Определите, является ли введенное целое неотрицательное число палиндромом (читается одинаково слева направо и справа налево).

Формат ввода:
Одно целое число N.

Формат вывода:
"true" если да, иначе "false".

Пример:
Ввод: 121
Вывод: true', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '121', 'true', TRUE), (v_task_id, '10', 'false', TRUE);

    -- Task 4: Valid Parentheses
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Структуры данных', '[ДУЭЛЬ] Правильная скобочная последовательность', 
        'Считайте строку из скобок ''()'', ''{}'', ''[]''. Определите, правильная ли это последовательность (каждая открывающая имеет парную закрывающую в нужном порядке).

Формат ввода:
Строка S (до 10^4 символов).

Формат вывода:
"Yes" если правильная, "No" если нет.

Пример:
Ввод: ()[]{}
Вывод: Yes', 
        E'#include <iostream>\n#include <string>\n#include <stack>\n\nint main() {\n    std::string s;\n    std::cin >> s;\n    // Ваш код\n    return 0;\n}',
        'string,stack', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '()[]{}', 'Yes', TRUE), (v_task_id, '([)]', 'No', TRUE), (v_task_id, '{[]}', 'Yes', TRUE);

    -- Task 5: Factorial
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Функции', '[ДУЭЛЬ] Факториал', 
        'Напишите программу, которая считывает число N и выводит его факториал (N!). Гарантируется, что N от 0 до 12.
0! = 1.

Формат ввода:
Целое число N (0 <= N <= 12).

Формат вывода:
Факториал N.

Пример:
Ввод: 5
Вывод: 120', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '5', '120', TRUE), (v_task_id, '0', '1', TRUE);

    -- Task 6: Count Vowels
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Строки', '[ДУЭЛЬ] Подсчет гласных', 
        'Считайте одно слово (строку без пробелов). Подсчитайте количество английских гласных букв в нем (a, e, i, o, u) без учета регистра.

Формат ввода:
Одна строка.

Формат вывода:
Одно целое число — количество гласных.

Пример:
Ввод: Hello
Вывод: 2', 
        E'#include <iostream>\n#include <string>\n\nint main() {\n    std::string s;\n    std::cin >> s;\n    // Ваш код\n    return 0;\n}',
        'string', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, 'Hello', '2', TRUE), (v_task_id, 'CPP', '0', TRUE);

    -- Task 7: Max Element Array
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Массивы', '[ДУЭЛЬ] Максимум в векторе', 
        'Считайте число N. Затем считайте N целых чисел. Найдите и выведите максимальное из них.

Формат ввода:
Число N. Затем N чисел.

Формат вывода:
Максимальное число.

Пример:
Ввод: 4 5 12 -3 8
Вывод: 12', 
        E'#include <iostream>\n#include <vector>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        'vector', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '4 5 12 -3 8', '12', TRUE);

    -- Task 8: FizzBuzz
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Алгоритмы', '[ДУЭЛЬ] FizzBuzz', 
        'Напишите классическую программу FizzBuzz для числа N (N выводится, от 1 до N). Если число делится на 3, выводите Fizz, если на 5 — Buzz, если на оба — FizzBuzz, иначе само число. Каждое слово или число на новой строке.

Формат ввода:
Одно целое число N (1 <= N <= 100).

Формат вывода:
N строк ответа.

Пример:
Ввод: 5
Вывод: 1
2
Fizz
4
Buzz', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '5', E'1\n2\nFizz\n4\nBuzz', TRUE);

    -- Task 9: Two Sum
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'STL', '[ДУЭЛЬ] Сумма двух', 
        'Дан массив из N целых чисел и число target. Найдите ИНДЕКСЫ (0-based) двух чисел, которые в сумме дают target. Гарантируется, что решение ровно одно. Сначала считайте N, затем N элементов массива, затем target.

Формат ввода:
Число N. Затем N чисел массива. В конце число target.

Формат вывода:
Два индекса через пробел (в порядке возрастания индексов).

Пример:
Ввод: 4 2 7 11 15 9
Вывод: 0 1', 
        E'#include <iostream>\n#include <vector>\n#include <unordered_map>\n\nint main() {\n    // Ваш код\n    return 0;\n}',
        'vector,unordered_map', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '4 2 7 11 15 9', '0 1', TRUE), (v_task_id, '3 3 2 4 6', '1 2', TRUE);

    -- Task 10: Sum of Digits
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Математика', '[ДУЭЛЬ] Сумма цифр', 
        'Считайте неотрицательное целое число. Выведите сумму его цифр.

Формат ввода:
Одно целое число.

Формат вывода:
Сумма цифр.

Пример:
Ввод: 123
Вывод: 6', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '123', '6', TRUE), (v_task_id, '0', '0', TRUE);

    -- Task 11: Palindrome String
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Строки', '[ДУЭЛЬ] Палиндром строки', 
        'Проверьте, является ли введенная строка палиндромом. Выведите YES или NO.
Строка состоит из строчных латинских букв.

Пример:
Ввод: abba
Вывод: YES', 
        E'#include <iostream>\n#include <string>\n#include <algorithm>\n\nint main() {\n    std::string s;\n    std::cin >> s;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, 'abba', 'YES', TRUE), 
        (v_task_id, 'abc', 'NO', TRUE);

    -- Task 12: Single Number
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Алгоритмы', '[ДУЭЛЬ] Одинокий элемент', 
        'Дан массив из 2N+1 чисел. Каждое число встречается ровно дважды, кроме одного. Найдите его.

Формат ввода:
Сначала число M (количество элементов, M = 2N+1). Затем M целых чисел.

Пример:
Ввод: 3 2 2 1
Вывод: 1', 
        E'#include <iostream>\n#include <vector>\n\nint main() {\n    int m;\n    std::cin >> m;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '3 2 2 1', '1', TRUE), 
        (v_task_id, '5 4 1 2 1 2', '4', TRUE);

    -- Task 13: Factorial
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Математика', '[ДУЭЛЬ] Факториал', 
        'Вычислите факториал числа N (N! = 1 * 2 * ... * N). N <= 12.

Пример:
Ввод: 5
Вывод: 120', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '5', '120', TRUE), 
        (v_task_id, '0', '1', TRUE);

    -- Task 14: Power of Two
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Математика', '[ДУЭЛЬ] Степень двойки', 
        'Проверьте, является ли целое положительное число N степенью двойки. Выведите YES или NO.

Пример:
Ввод: 16
Вывод: YES', 
        E'#include <iostream>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '16', 'YES', TRUE), 
        (v_task_id, '18', 'NO', TRUE);

    -- Task 15: Maximum of Array
    INSERT INTO coding_tasks (
        lesson_id, duel_topic, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit, is_duel
    ) VALUES (
        NULL, 'Алгоритмы', '[ДУЭЛЬ] Максимум в массиве', 
        'Найдите максимальный элемент в массиве целых чисел.

Формат ввода:
Число N (количество элементов), затем N чисел.

Пример:
Ввод: 4 1 5 2 3
Вывод: 5', 
        E'#include <iostream>\n#include <vector>\n\nint main() {\n    int n;\n    std::cin >> n;\n    // Ваш код\n    return 0;\n}',
        '', 
        '#define,goto,asm,__asm__,__asm,system', 
        2000, 256, TRUE
    ) RETURNING id INTO v_task_id;
    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public) VALUES 
        (v_task_id, '4 1 5 2 3', '5', TRUE), 
        (v_task_id, '3 -1 -5 -2', '-1', TRUE);

    RAISE NOTICE 'Seeding completed. Last ID: %', v_task_id;
END $$;