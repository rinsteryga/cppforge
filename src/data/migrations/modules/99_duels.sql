SET client_encoding = 'UTF8';

DO $$
DECLARE
    v_task_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding duels...';

    INSERT INTO coding_tasks (
        lesson_id, 
        duel_topic, 
        title, 
        description, 
        initial_code, 
        whitelist, 
        blacklist, 
        time_limit, 
        memory_limit, 
        is_duel
    )
    VALUES (
        NULL, 
        'Основы', 
        '[ДУЭЛЬ] Четное или Нечетное', 
        'Напишите программу, которая принимает одно целое число со стандартного ввода и выводит "Even", если оно четное, и "Odd", если нечетное.', 
        E'#include <stdio.h>\n\nint main(void) {\n    int n;\n    scanf("%d", &n);\n    \n    return 0;\n}',
        'main,return,int,void,#include,stdio.h,printf,scanf', 
        '', 
        2000, 
        256,
        TRUE
    ) RETURNING id INTO v_task_id;

    INSERT INTO test_cases (coding_task_id, input, expected_output, is_public)
    VALUES 
        (v_task_id, '4', E'Even\n', TRUE),
        (v_task_id, '7', E'Odd\n', TRUE);
    
    RAISE NOTICE 'Seeding completed. ID: %', v_task_id;
END $$;