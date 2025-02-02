#pragma once
#include "database.h"

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

class BinaryDatabase : public Database
{
public:
    BinaryDatabase(std::string_view fileName);
    ~BinaryDatabase();

    void Add(const Data& data) override;

    Data Get(uint32_t id) const override;
    std::vector<Data> GetAll() const override;

    void Modify(uint32_t id, const Data& data) override;

    void Remove(uint32_t id) override;

    void Clear() override;

    uint32_t GetCount() const override;

private:
    void notifyWriter(bool stop = false);

    void loadCache();

    void writerThread();

private:
    const std::string m_fileName;
    std::vector<Data> m_cache;

    std::thread m_writerThread;
    std::condition_variable m_cv;
    bool m_write;
    bool m_stop;
    mutable std::mutex m_mutex;
};
