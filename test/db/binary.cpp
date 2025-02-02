#define SPDLOG_ACTIVE_LEVEL 6

#include "data.h"
#include "binarydatabase.h"
#include "qsort.h"

#include <gtest/gtest.h>

#include <chrono>
#include <random>
#include <iostream>

class DB_Binary : public testing::Test
{
public:
    DB_Binary()
        : db("")
    {}

    void TearDown() override
    {
        db.Clear();
    }

    void Fill(int size)
    {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        static std::uniform_int_distribution dist;

        for (int i = 0; i < size; i++)
        {
            Data data;

            data.original.push_back(dist(mt));
            data.sorted = data.original;
            std::sort(data.sorted.begin(), data.sorted.end());

            db.Add(data);
        }
    }

    uint64_t TestLoadAndSort(int count)
    {
        using namespace std::chrono;
        auto start = system_clock::now();
        for (int i = 0; i < count; i++)
        {
            Data data = db.Get(i);
            QuickSort(data.original.begin(), data.original.end(), std::less());
        }
        return duration_cast<microseconds>(system_clock::now() - start).count();
    }

protected:
    BinaryDatabase db;
};

TEST_F(DB_Binary, Test100)
{
    ASSERT_NO_THROW(Fill(100));
    ASSERT_NO_THROW(
        auto time = TestLoadAndSort(100);
        std::cout << "Loaded and sorted 100 arrays in " << time << " mcs.\nAveraged " << time / 100 << " mcs per array\n";
    );
}

TEST_F(DB_Binary, Test1000)
{
    ASSERT_NO_THROW(Fill(1000));
    ASSERT_NO_THROW(
        auto time = TestLoadAndSort(1000);
        std::cout << "Loaded and sorted 1000 arrays in " << time << " mcs.\nAveraged " << time / 1000 << " mcs per array\n";
    );
}

TEST_F(DB_Binary, Test10000)
{
    ASSERT_NO_THROW(Fill(10000));
    ASSERT_NO_THROW(
        auto time = TestLoadAndSort(10000);
        std::cout << "Loaded and sorted 10000 arrays in " << time << " mcs.\nAveraged " << time / 10000 << " mcs per array\n";
    );
}
