#pragma once
#include "data.h"

#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#define DB_INFO(...) SPDLOG_LOGGER_INFO(m_databaseLogger, __VA_ARGS__)
#define DB_WARN(...) SPDLOG_LOGGER_WARN(m_databaseLogger, __VA_ARGS__)
#define DB_ERROR(...) SPDLOG_LOGGER_ERROR(m_databaseLogger, __VA_ARGS__)

class Database
{
public:
    enum class Status
    {
        Ready,
        Restricted,
        Unavailable
    };

    virtual ~Database() = default;

    virtual void Add(const Data& data) = 0;

    virtual Data Get(uint32_t id) const = 0;
    virtual std::vector<Data> GetAll() const = 0;

    virtual void Modify(uint32_t id, const Data& data) = 0;

    virtual void Remove(uint32_t id) = 0;

    virtual void Clear() = 0;

    virtual uint32_t GetCount() const = 0;

    Status GetStatus() const;

protected:
    void SetStatus(Status status);

    inline static std::shared_ptr<spdlog::logger> m_databaseLogger = std::make_shared<spdlog::logger>("db", spdlog::sinks_init_list {
        std::make_shared<spdlog::sinks::basic_file_sink_st>("logs/db.log", true)
    });

private:
    Status m_status = Status::Unavailable;
};

std::ostream& operator<<(std::ostream& ostream, Database::Status status);

template<>
struct fmt::formatter<Database::Status> : public fmt::ostream_formatter {};
