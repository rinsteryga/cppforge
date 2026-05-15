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
    user_progress,
    tips
RESTART IDENTITY CASCADE;

INSERT INTO tips (content) VALUES
('Используйте std::unique_ptr для исключительного владения и std::shared_ptr для разделяемого владения ресурсами.'),
('RAII (Resource Acquisition Is Initialization) связывает жизненный цикл ресурса с временем жизни объекта.'),
('Используйте ''auto'' для избежания избыточных объявлений типов, но сохраняйте читаемость кода.'),
('Лямбда-выражения позволяют лаконично создавать анонимные функциональные объекты.'),
('Всегда используйте ''const'' для переменных и функций-членов, которые не изменяют состояние объекта.'),
('Передавайте крупные объекты по константной ссылке, чтобы избежать лишнего копирования.'),
('Используйте std::move для передачи владения и предотвращения глубокого копирования объектов.'),
('Предпочитайте std::array или std::vector обычным массивам в стиле C.'),
('Используйте циклы for на основе диапазонов (range-based for) для удобной итерации по контейнерам.'),
('std::optional отлично подходит для возврата значений, которые могут отсутствовать.'),
('constexpr позволяет вычислять значения функций и переменных на этапе компиляции.'),
('Используйте std::string_view для строковых параметров только для чтения, чтобы избежать лишних аллокаций.'),
('Помечайте переопределенные виртуальные функции ключевым словом ''override'' для защиты от ошибок.'),
('Предпочитайте строго типизированные перечисления (enum class) обычным перечислениям в стиле C.'),
('Используйте std::make_unique и std::make_shared для безопасного создания умных указателей.'),

('Идиома Pimpl (Pointer to Implementation) помогает скрыть детали реализации и сократить время компиляции.'),
('Правило пяти: если вы определили деструктор, вам, скорее всего, нужны конструкторы копирования/перемещения и операторы присваивания.'),
('CRTP (Curiously Recurring Template Pattern) позволяет реализовать статический полиморфизм.'),
('Предпочитайте композицию наследованию для создания гибких и расширяемых систем.'),
('Используйте паттерн Singleton с осторожностью; рассмотрите возможность внедрения зависимостей (Dependency Injection).'),
('Паттерн Factory (Фабрика) помогает создавать объекты, не указывая их точный класс.'),
('Паттерн Observer (Наблюдатель) полезен для реализации систем обработки событий.'),

('Используйте std::vector::reserve(), чтобы избежать многократных переаллокаций памяти, если размер известен заранее.'),
('Избегайте std::endl; он принудительно очищает буфер вывода. Используйте ''\n'' для лучшей производительности.'),
('Помечайте конструкторы с одним аргументом как ''explicit'', чтобы избежать непреднамеренных неявных преобразований типов.'),
('Оптимизация малых строк (SSO): std::string часто хранит короткие строки без выделения памяти в куче.'),
('Выравнивание данных: используйте alignas() для оптимизации доступа к памяти или работы с оборудованием.'),
('Используйте атрибут [[nodiscard]], чтобы компилятор предупреждал, если результат важной функции игнорируется.'),
('SFINAE (Substitution Failure Is Not An Error) — ключевая техника метапрограммирования шаблонов в C++.'),
('Специализация шаблонов позволяет определять специфическую реализацию для конкретных типов данных.');

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
('First Code', 'Solve your first coding task!', ':/images/patrick.png', 'LEVELS_COMPLETED', 1),
('Expert', 'Solve 10 tasks to become an expert.', ':/images/terry.png', 'LEVELS_COMPLETED', 10),
('Persistent', 'Solve tasks 2 days in a row.', ':/images/tony.png', 'STREAK_DAYS', 2),
('Tenacious', 'Solve tasks 5 days in a row.', ':/images/walter.png', 'STREAK_DAYS', 5),
('Course Master', 'Complete all modules in the course!', ':/images/kanye.png', 'COURSE_COMPLETED', 1),
('Easter Egg Finder', 'Find the hidden secret.', ':/images/lb.png', 'CUSTOM_EVENT', 0);

COMMIT;
