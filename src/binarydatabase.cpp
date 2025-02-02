#include "binarydatabase.h"

template<typename T>
std::ostream& writeBinary(std::ostream& ostream, T data)
{
    return ostream.write((char*)&data, sizeof(T));
}

template<typename T>
std::ostream& writeBinary(std::ostream& ostream, const std::vector<T>& dataVector)
{
    writeBinary(ostream, (uint32_t)dataVector.size());
    if (dataVector.size() == 0) return ostream;
    return ostream.write((char*)dataVector.data(), dataVector.size() * sizeof(T));
}

template<typename T>
std::istream& readBinary(std::istream& istream, T& data)
{
    return istream.read((char*)&data, sizeof(data));
}

template<typename T>
std::istream& readBinary(std::istream& istream, std::vector<T>& data)
{
    uint32_t size = 0;
    readBinary(istream, size);
    if (size == 0) return istream;
    data.resize(size);
    return istream.read((char*)data.data(), size * sizeof(T));
}

BinaryDatabase::BinaryDatabase(std::string_view fileName)
    : m_fileName(fileName)
    , m_writerThread(&BinaryDatabase::writerThread, this)
{
    SetStatus(Status::Ready);
    loadCache();
}

BinaryDatabase::~BinaryDatabase()
{
    notifyWriter(true);
    if (m_writerThread.joinable()) m_writerThread.join();
}

void BinaryDatabase::Add(const Data& data)
{
    std::scoped_lock l(m_mutex);
    DB_INFO("BinaryDatabase | Added record.\n{}", data);
    m_cache.push_back(data);
    notifyWriter();
}

Data BinaryDatabase::Get(uint32_t id) const
{
    std::scoped_lock l(m_mutex);
    if (id >= m_cache.size())
    {
        DB_WARN("BinaryDatabase | Tried to get record with 'id={}', which is higher than or equal to 'count={}'", id, m_cache.size());
        return {};
    }
    DB_INFO("BinaryDatabase | Get record with 'id={}' returned\n{}", id, m_cache[id]);
    return m_cache[id];
}

std::vector<Data> BinaryDatabase::GetAll() const
{
    std::scoped_lock l(m_mutex);
    DB_INFO("BinaryDatabase | Returned all records in the cache. 'count={}'", m_cache.size());
    return m_cache;
}

void BinaryDatabase::Modify(uint32_t id, const Data& data)
{
    std::scoped_lock l(m_mutex);
    DB_INFO("BinaryDatabase | Modified record. Old =\n{}\nNew =\n{}", m_cache[id], data);
    m_cache[id] = data;
    notifyWriter();
}

void BinaryDatabase::Remove(uint32_t id)
{
    std::scoped_lock l(m_mutex);
    if (id >= m_cache.size())
    {
        DB_WARN("BinaryDatabase | Tried to remove record with 'id={}', which is higher than or equal to 'count={}'", id, m_cache.size());
        return;
    }
    m_cache.erase(m_cache.begin() + id);
    notifyWriter();
}

void BinaryDatabase::Clear()
{
    std::scoped_lock l(m_mutex);
    DB_INFO("BinaryDatabase | Cleared database");
    m_cache.clear();
    notifyWriter();
}

uint32_t BinaryDatabase::GetCount() const
{
    std::scoped_lock l(m_mutex);
    return m_cache.size();
}

void BinaryDatabase::notifyWriter(bool stop)
{
    m_stop = stop;
    m_write = true;
    m_cv.notify_one();
}

void BinaryDatabase::loadCache()
{
    std::ifstream file(m_fileName);
    if (!file.is_open())
    {
        DB_WARN("BinaryDatabase | Failed to open the file '{}' to load, will start with the empty database", m_fileName);
        return;
    }

    uint32_t cacheSize = 0;
    readBinary(file, cacheSize);

    DB_INFO("BinaryDatabase | Read cache size: {}", cacheSize);

    std::scoped_lock l(m_mutex);
    m_cache.resize(cacheSize);
    for (int i = 0; i < cacheSize; i++)
    {
        readBinary(file, m_cache[i].original);
        readBinary(file, m_cache[i].sorted);
        DB_INFO("BinaryDatabase | Read record {}.\n{}", i, m_cache[i]);
    }
}

void BinaryDatabase::writerThread()
{
    while (true)
    {
        std::unique_lock l(m_mutex);
        m_cv.wait(l, [this]() { return m_write || m_stop; });
        m_write = false;
        auto cacheCopy = m_cache;
        l.unlock();

        std::ofstream file(m_fileName, std::ios_base::binary);
        if (!file.is_open() && GetStatus() == Status::Ready)
        {
            DB_ERROR("BinaryDatabase | Failed to open file '{}' to write. DB will operate in restricted mode, only utilizing cache.", m_fileName);
            SetStatus(Status::Restricted);
            return;
        }
        else if (GetStatus() != Status::Ready)
        {
            DB_INFO("BinaryDatabase | Successfully opened file '{}' to write. DB will continue to operate fully functional.", m_fileName);
            SetStatus(Status::Ready);
        }

        writeBinary(file, (uint32_t)cacheCopy.size());
        for (const auto& data : cacheCopy)
        {
            writeBinary(file, data.original);
            writeBinary(file, data.sorted);
        }

        if (m_stop) break;
    }
}
