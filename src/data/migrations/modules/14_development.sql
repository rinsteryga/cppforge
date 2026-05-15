SET client_encoding = 'UTF8';

DO $$ 
DECLARE
    v_mod_id BIGINT;
    v_lesson_id BIGINT;
BEGIN
    RAISE NOTICE 'Seeding Module 14: Развитие...';

    INSERT INTO modules (name, description) 
    VALUES ('Модуль 14: Развитие', 'Дальнейшие шаги и пути развития в мире C++.')
    RETURNING id INTO v_mod_id;

    -- 14.1
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
(v_mod_id, '14.1. Пути развития C++ разработчика', $content$C++ — огромный язык, и никто не знает его на 100%. Выберите свою "нишу" и углубляйтесь в неё:

1. **GameDev**: Движки (Unreal Engine), графика (Vulkan, DirectX), физика и ИИ. Здесь C++ — стандарт де-факто из-за производительности.
2. **Embedded & Robotics**: Микроконтроллеры, RTOS, драйверы и ROS (Robot Operating System). Работа в условиях ограниченных ресурсов.
3. **High-Load / FinTech**: Высокочастотный трейдинг (HFT), биржи. Каждая микросекунда на счету, используются kernel-bypass и кастомные аллокаторы.
4. **Systems Programming**: Разработка СУБД (Postgres, MongoDB), браузеров (Chromium), компиляторов (LLVM) и операционных систем.
5. **Backend & Networking**: Разработка высокопроизводительных серверов, протоколов и облачных инфраструктур.
6. **Computer Vision & AI**: Библиотеки вроде OpenCV, интеграция нейросетей через TensorRT.

Каждая сфера требует своих библиотек/фреймворков (Qt, Boost, RadioLib, OpenCV, DirectX), но база языка везде одна.$content$, 1);

    -- 14.2
    INSERT INTO lessons (module_id, title, content, order_index) VALUES
    (v_mod_id, '14.2. Заключение', $content$Поздравляем с прохождением базового курса по C++! 
Следующий шаг — писать код. Много кода. Придумайте свой проект (например, простенький веб-сервер, эмулятор или игру) и реализуйте его. Читайте чужой код на GitHub, изучайте стандарты (cppreference) и готовьтесь к собеседованиям.

Удачи!$content$, 2);

END $$;
