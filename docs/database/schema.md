# Database Schema Documentation  

**Project:** cppforge  
**Version:** 1.0  

---

## 1. Overview

The schema is designed to support a modular learning platform containing lessons, coding tasks, automated testing, execution results, and user progress tracking.  
It also provides a flexible achievement system and detailed submission history.

---

## 2. Tables

---

## 2.1. `users`

Stores basic account information.

| Column        | Type        | Notes                              |
|---------------|-------------|------------------------------------|
| id            | BIGSERIAL   | Primary key                        |
| username      | TEXT        | Unique, required                   |
| email         | TEXT        | Unique, required                   |
| password_hash | TEXT        | Required                           |
| avatar_path   | TEXT        | Optional                           |
| bio           | TEXT        | Optional                           |
| created_at    | TIMESTAMP   | Defaults to `NOW()`                |

---

## 2.2. `achievements`

Achievement definitions.

| Column      | Type        | Notes                     |
|-------------|-------------|---------------------------|
| id          | BIGSERIAL   | Primary key               |
| name        | TEXT        | Required                  |
| description | TEXT        | Optional                  |
| icon_path   | TEXT        | Optional                  |

---

## 2.3. `user_achievements`

Many-to-many relationship between `users` and `achievements`.

| Column         | Type      | Notes                               |
|----------------|-----------|-------------------------------------|
| user_id        | BIGINT    | FK → users(id), cascade delete      |
| achievement_id | BIGINT    | FK → achievements(id), cascade delete |
| earned_at      | TIMESTAMP | Defaults to `NOW()`                 |

Primary key: `(user_id, achievement_id)`.

---

## 2.4. `modules`

High-level grouping units.

| Column      | Type      | Notes       |
|-------------|-----------|-------------|
| id          | BIGSERIAL | Primary key |
| name        | TEXT      | Required    |
| description | TEXT      | Optional    |

---

## 2.5. `lessons`

Lessons within modules.

| Column    | Type      | Notes                               |
|-----------|-----------|-------------------------------------|
| id        | BIGSERIAL | Primary key                         |
| module_id | BIGINT    | FK → modules(id), cascade delete    |
| title     | TEXT      | Required                            |
| content   | TEXT      | Optional                            |

Index: `module_id`.

---

## 2.6. `coding_tasks`

Coding exercises linked to lessons.

| Column        | Type      | Notes                                |
|---------------|-----------|--------------------------------------|
| id            | BIGSERIAL | Primary key                          |
| lesson_id     | BIGINT    | FK → lessons(id), cascade delete     |
| title         | TEXT      | Required                             |
| description   | TEXT      | Optional                             |
| initial_code  | TEXT      | Optional                             |
| time_limit    | INT       | Default: 2000 ms                     |
| memory_limit  | INT       | Default: 256 MB                      |

Index: `lesson_id`.

---

## 2.7. `test_cases`

Input/expected output pairs for coding tasks.

| Column         | Type      | Notes                             |
|----------------|-----------|-----------------------------------|
| id             | BIGSERIAL | Primary key                       |
| coding_task_id | BIGINT    | FK → coding_tasks(id), cascade delete |
| input          | TEXT      | Required                          |
| expected_output| TEXT      | Required                          |
| is_public      | BOOLEAN   | Default: TRUE                     |

Index: `coding_task_id`.

---

## 2.8. `submissions`

Stores each attempt by a user for a specific task.

| Column          | Type      | Notes                               |
|-----------------|-----------|-------------------------------------|
| id              | BIGSERIAL | Primary key                         |
| user_id         | BIGINT    | FK → users(id), cascade delete      |
| module_id       | BIGINT    | FK → modules(id), cascade delete    |
| coding_task_id  | BIGINT    | FK → coding_tasks(id), cascade delete |
| source_code     | TEXT      | Required                            |
| submitted_at    | TIMESTAMP | Default: NOW()                      |
| is_success      | BOOLEAN   | Default: FALSE                      |

Indexes: `user_id`, `coding_task_id`.

---

## 2.9. `execution_results`

Execution output of a submission.

| Column           | Type      | Notes                               |
|------------------|-----------|-------------------------------------|
| id               | BIGSERIAL | Primary key                         |
| submission_id    | BIGINT    | FK → submissions(id), cascade delete |
| is_success       | BOOLEAN   | Required                            |
| output           | TEXT      | Optional                            |
| errors           | TEXT      | Optional                            |
| time_ms          | INT       | Optional                            |
| passed_tests_count | INT     | Optional                            |

Index: `submission_id`.

---

## 2.10. `user_progress`

Tracks completion of lessons by users.

| Column     | Type      | Notes                               |
|------------|-----------|-------------------------------------|
| id         | BIGSERIAL | Primary key                         |
| user_id    | BIGINT    | FK → users(id), cascade delete      |
| module_id  | BIGINT    | FK → modules(id), cascade delete    |
| lesson_id  | BIGINT    | FK → lessons(id), cascade delete    |
| is_completed | BOOLEAN | Default: FALSE                      |
| updated_at | TIMESTAMP | Default: NOW()                      |

Unique constraint: `(user_id, lesson_id)`.

---

## 3. Relationships Summary

- **User ↔ Achievement:** many-to-many  
- **Module → Lessons:** one-to-many  
- **Lesson → CodingTasks:** one-to-many  
- **CodingTask → TestCases:** one-to-many  
- **User + Task → Submissions:** many-to-many through submissions  
- **Submission → ExecutionResults:** one-to-one  
- **User + Lesson → UserProgress:** tracks user progress  

---

## 4. Notes

- All foreign keys apply **ON DELETE CASCADE**, simplifying cleanup.  
- Schema is stable and not expected to change often.  
- Indexes are added for frequent relations to optimize lookups.  
- Designed to work well with Qt SQL modules (QSqlDatabase, QSqlQuery).

---
