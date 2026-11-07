#include <server/RequestLoggingMiddleware.hpp>

#include <QtTest>

#include <algorithm>
#include <mutex>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

namespace
{
    class CapturingBuffer : public std::streambuf
    {
    public:
        std::string contents()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_data;
        }

    protected:
        int overflow(int character) override
        {
            if (character == EOF)
            {
                return character;
            }
            std::lock_guard<std::mutex> lock(m_mutex);
            m_data.push_back(static_cast<char>(character));
            return character;
        }

        std::streamsize xsputn(const char *data, std::streamsize size) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_data.append(data, static_cast<std::size_t>(size));
            return size;
        }

    private:
        std::mutex m_mutex;
        std::string m_data;
    };

    std::vector<std::string> lines(const std::string &text)
    {
        std::vector<std::string> result;
        std::size_t start = 0;
        while (start < text.size())
        {
            const auto end = text.find('\n', start);
            if (end == std::string::npos)
            {
                result.push_back(text.substr(start));
                break;
            }
            result.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        return result;
    }
} // namespace

class TestRequestLoggingMiddleware : public QObject
{
    Q_OBJECT

private slots:
    void accessLogCanBeDisabled()
    {
        cppforge::server::RequestLoggingMiddleware middleware(false);
        cppforge::server::RequestLoggingMiddleware::context ctx;
        crow::request request;
        request.method = crow::HTTPMethod::Get;
        request.url = "/api/v1/modules?page=1";
        crow::response response(200);

        CapturingBuffer buffer;
        auto *previous = std::clog.rdbuf(&buffer);
        middleware.before_handle(request, response, ctx);
        middleware.after_handle(request, response, ctx);
        std::clog.rdbuf(previous);

        QVERIFY(buffer.contents().empty());
        QVERIFY(!response.get_header_value("X-Request-Id").empty());
    }

    void accessLogIsOneLineWithoutQueryOrSecrets()
    {
        cppforge::server::RequestLoggingMiddleware middleware(true);
        cppforge::server::RequestLoggingMiddleware::context ctx;
        crow::request request;
        request.method = crow::HTTPMethod::Get;
        request.url = "/api/v1/modules?PG_PASSWORD=secret";
        crow::response response(200);

        CapturingBuffer buffer;
        auto *previous = std::clog.rdbuf(&buffer);
        middleware.before_handle(request, response, ctx);
        middleware.after_handle(request, response, ctx);
        std::clog.rdbuf(previous);

        const std::string output = buffer.contents();
        QCOMPARE(std::count(output.begin(), output.end(), '\n'), std::ptrdiff_t(1));
        QVERIFY(output.find("request_id=req-") == 0);
        QVERIFY(output.find("method=GET") != std::string::npos);
        QVERIFY(output.find("path=/api/v1/modules") != std::string::npos);
        QVERIFY(output.find("status=200") != std::string::npos);
        QVERIFY(output.find("secret") == std::string::npos);
        QVERIFY(output.find("PG_PASSWORD") == std::string::npos);
    }

    void concurrentAccessLogsStayOnSeparateLines()
    {
        cppforge::server::RequestLoggingMiddleware middleware(true);
        CapturingBuffer buffer;
        auto *previous = std::clog.rdbuf(&buffer);

        constexpr int requestCount = 24;
        std::vector<std::thread> workers;
        workers.reserve(requestCount);
        for (int index = 0; index < requestCount; ++index)
        {
            workers.emplace_back(
                [&middleware]
                {
                    cppforge::server::RequestLoggingMiddleware::context ctx;
                    crow::request request;
                    request.method = crow::HTTPMethod::Get;
                    request.url = "/api/v1/modules";
                    crow::response response(200);
                    middleware.before_handle(request, response, ctx);
                    middleware.after_handle(request, response, ctx);
                });
        }
        for (auto &worker : workers)
        {
            worker.join();
        }
        std::clog.rdbuf(previous);

        const auto outputLines = lines(buffer.contents());
        QCOMPARE(outputLines.size(), std::size_t(requestCount));
        for (const auto &line : outputLines)
        {
            QVERIFY(line.rfind("request_id=req-", 0) == 0);
            QVERIFY(line.find("method=GET") != std::string::npos);
            QVERIFY(line.find("path=/api/v1/modules") != std::string::npos);
            QVERIFY(line.find("status=200") != std::string::npos);
            QVERIFY(line.find("duration_ms=") != std::string::npos);
        }
    }
};

QTEST_GUILESS_MAIN(TestRequestLoggingMiddleware)

#include "test_RequestLoggingMiddleware.moc"
