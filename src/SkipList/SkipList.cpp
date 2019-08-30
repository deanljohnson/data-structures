#include "SkipList.h"
#include "doctest.h"
#include <cstdlib>

TEST_CASE("Size defaults to 0")
{
    SkipList<int, int> list;
    CHECK(list.size() == 0);
}

TEST_CASE("Size Updates")
{
    SkipList<int, int> list;
    CHECK(list.size() == 0);

    list.insert(1, 2);
    CHECK(list.size() == 1);

    list.insert(3, 4);
    CHECK(list.size() == 2);
}

TEST_CASE("Overwriting an Element")
{
    SkipList<int, int> list;
    list.insert(1, 1);
    CHECK(*list.find(1) == 1);
    list.insert(1, 2);
    CHECK(*list.find(1) == 2);
}

TEST_CASE("Inserting Increasing Elements")
{
    SkipList<int, int> list;

    list.insert(1, 2);
    auto* find1 = list.find(1);
    CHECK(find1 != nullptr);
    CHECK(*find1 == 2);

    list.insert(3, 4);
    auto* find3 = list.find(3);
    CHECK(find3 != nullptr);
    CHECK(*find3 == 4);

    list.insert(5, 6);
    auto* find5 = list.find(5);
    CHECK(find5 != nullptr);
    CHECK(*find5 == 6);
}

TEST_CASE("Inserting Decreasing Elements")
{
    SkipList<int, int> list;

    list.insert(5, 6);
    auto* find5 = list.find(5);
    CHECK(find5 != nullptr);
    CHECK(*find5 == 6);

    list.insert(3, 4);
    auto* find3 = list.find(3);
    CHECK(find3 != nullptr);
    CHECK(*find3 == 4);

    list.insert(1, 2);
    auto* find1 = list.find(1);
    CHECK(find1 != nullptr);
    CHECK(*find1 == 2);
}

TEST_CASE("Inserting Into Middle of Lowest Level")
{
    SkipList<int, int> list;

    list.insert(1, 2);
    auto* find1 = list.find(1);
    CHECK(find1 != nullptr);
    CHECK(*find1 == 2);

    list.insert(5, 6);
    auto* find5 = list.find(5);
    CHECK(find5 != nullptr);
    CHECK(*find5 == 6);

    list.insert(3, 4);
    auto* find3 = list.find(3);
    CHECK(find3 != nullptr);
    CHECK(*find3 == 4);
}

TEST_CASE("Removing Single Element")
{
    SkipList<int, int> list;

    list.insert(1, 2);
    CHECK(list.erase(1));
}

TEST_CASE("Removing Multiple Elements")
{
    SkipList<int, int> list;

    list.insert(1, 2);
    list.insert(5, 6);
    list.insert(3, 4);
    bool erase5 = list.erase(5);
    CHECK(erase5);
    bool erase1 = list.erase(1);
    CHECK(erase1);
    bool erase3 = list.erase(3);
    CHECK(erase3);
}

TEST_CASE("Removing Non-Existent Element")
{
    SkipList<int, int> list;
    CHECK(!list.erase(1));
}

#include <iostream>
TEST_CASE("Print Test")
{
    SkipList<int, int> list;

    for (size_t i = 0; i < 99; i++)
    {
        list.insert(rand() % 99, rand() % 99);
        list.debugPrint();
        std::cout << std::endl;
    }
}