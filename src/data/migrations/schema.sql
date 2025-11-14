-- ===========================================================
-- CPPFORGE Database Schema
-- Version: 1.0
-- Description: Initial schema for the C++ learning platform
-- ===========================================================

-- Drop existing tables in reverse dependency order (for re-init)
DROP TABLE IF EXISTS execution_results CASCADE;
DROP TABLE IF EXISTS submissions CASCADE;
DROP TABLE IF EXISTS test_cases CASCADE;
DROP TABLE IF EXISTS coding_tasks CASCADE;
DROP TABLE IF EXISTS lessons CASCADE;
DROP TABLE IF EXISTS user_progress CASCADE;
DROP TABLE IF EXISTS user_achievements CASCADE;
DROP TABLE IF EXISTS achievements CASCADE;
DROP TABLE IF EXISTS modules CASCADE;
DROP TABLE IF EXISTS users CASCADE;

-- ===========================================================
-- USERS
-- ===========================================================
CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    email TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    avatar_path TEXT,
    bio TEXT,
    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

-- ===========================================================
-- ACHIEVEMENTS
-- ===========================================================
CREATE TABLE achievements (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT,
    icon_path TEXT,
    date_earned TIMESTAMP
);

-- User ↔ Achievement (many-to-many)
CREATE TABLE user_achievements (
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    achievement_id BIGINT NOT NULL REFERENCES achievements(id) ON DELETE CASCADE,
    earned_at TIMESTAMP DEFAULT NOW(),
    PRIMARY KEY (user_id, achievement_id)
);

-- ===========================================================
-- MODULES
-- ===========================================================
CREATE TABLE modules (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT
);

-- ===========================================================
-- LESSONS
-- ===========================================================
CREATE TABLE lessons (
    id BIGSERIAL PRIMARY KEY,
    module_id BIGINT NOT NULL REFERENCES modules(id) ON DELETE CASCADE,
    title TEXT NOT NULL,
    content TEXT
);
CREATE INDEX idx_lessons_module_id ON lessons(module_id);

-- ===========================================================
-- CODING TASKS
-- ===========================================================
CREATE TABLE coding_tasks (
    id BIGSERIAL PRIMARY KEY,
    lesson_id BIGINT NOT NULL REFERENCES lessons(id) ON DELETE CASCADE,
    title TEXT NOT NULL,
    description TEXT,
    initial_code TEXT,
    time_limit INT DEFAULT 2000,
    memory_limit INT DEFAULT 256
);
CREATE INDEX idx_tasks_lesson_id ON coding_tasks(lesson_id);

-- ===========================================================
-- TEST CASES
-- ===========================================================
CREATE TABLE test_cases (
    id BIGSERIAL PRIMARY KEY,
    coding_task_id BIGINT NOT NULL REFERENCES coding_tasks(id) ON DELETE CASCADE,
    input TEXT NOT NULL,
    expected_output TEXT NOT NULL,
    is_public BOOLEAN DEFAULT TRUE
);
CREATE INDEX idx_test_cases_task_id ON test_cases(coding_task_id);

-- ===========================================================
-- SUBMISSIONS
-- ===========================================================
CREATE TABLE submissions (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    module_id BIGINT NOT NULL REFERENCES modules(id) ON DELETE CASCADE,
    coding_task_id BIGINT NOT NULL REFERENCES coding_tasks(id) ON DELETE CASCADE,
    source_code TEXT NOT NULL,
    submitted_at TIMESTAMP NOT NULL DEFAULT NOW(),
    is_success BOOLEAN DEFAULT FALSE
);
CREATE INDEX idx_submissions_user_id ON submissions(user_id);
CREATE INDEX idx_submissions_task_id ON submissions(coding_task_id);

-- ===========================================================
-- EXECUTION RESULTS
-- ===========================================================
CREATE TABLE execution_results (
    id BIGSERIAL PRIMARY KEY,
    submission_id BIGINT NOT NULL REFERENCES submissions(id) ON DELETE CASCADE,
    is_success BOOLEAN NOT NULL,
    output TEXT,
    errors TEXT,
    time_ms INT,
    passed_tests_count INT
);
CREATE INDEX idx_execution_results_submission_id ON execution_results(submission_id);

-- ===========================================================
-- USER PROGRESS
-- ===========================================================
CREATE TABLE user_progress (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    module_id BIGINT NOT NULL REFERENCES modules(id) ON DELETE CASCADE,
    lesson_id BIGINT NOT NULL REFERENCES lessons(id) ON DELETE CASCADE,
    is_completed BOOLEAN DEFAULT FALSE,
    updated_at TIMESTAMP DEFAULT NOW(),
    UNIQUE (user_id, lesson_id)
);
