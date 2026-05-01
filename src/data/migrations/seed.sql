-- ===========================================================
-- CPPFORGE Database Seed Script
-- ===========================================================

SET client_encoding = 'UTF8';

BEGIN;

TRUNCATE TABLE 
    users, 
    modules, 
    lessons, 
    coding_tasks, 
    test_cases, 
    quizzes, 
    quiz_options, 
    matching_tasks, 
    matching_pairs, 
    achievements, 
    user_achievements, 
    submissions, 
    execution_results, 
    user_progress 
RESTART IDENTITY CASCADE;

COMMIT;

\ir modules/01_intro.sql
\ir modules/02_basics.sql
\ir modules/03_functions.sql
\ir modules/04_pointers.sql
\ir modules/05_strings.sql
\ir modules/06_structs_heap.sql
\ir modules/07_filesystem.sql
\ir modules/08_cpp_basics.sql
\ir modules/09_oop_functions.sql
\ir modules/10_oop.sql
\ir modules/11_modern_cpp.sql
\ir modules/12_stl.sql
\ir modules/13_best_practices.sql
\ir modules/14_development.sql
\ir modules/99_duels.sql

BEGIN;

INSERT INTO achievements (name, description, icon_path, condition_type, condition_value) VALUES
('First Code', 'Solve your first coding task!', ':/images/default_avatar1.png', 'LEVELS_COMPLETED', 1),
('Expert', 'Solve 10 tasks to become an expert.', ':/images/patrick.png', 'LEVELS_COMPLETED', 10),
('Persistent', 'Solve tasks 3 days in a row.', ':/images/default_avatar2.png', 'STREAK_DAYS', 3),
('Tenacious', 'Solve tasks 7 days in a row.', ':/images/default_avatar3.png', 'STREAK_DAYS', 7),
('Easter Egg Finder', 'Find the hidden secret.', ':/images/lb.png', 'CUSTOM_EVENT', 0);

COMMIT;
