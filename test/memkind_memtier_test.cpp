// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <memkind_memtier.h>

#include "common.h"

class MemkindMemtierTest: public ::testing::Test
{
private:
    void SetUp()
    {}

    void TearDown()
    {}
};

class MemkindMemtierKindTest: public ::testing::Test
{
protected:
    struct memtier_kind *m_tier_kind;
    struct memtier_tier *m_tier_default;
    struct memtier_tier *m_tier_regular;

    void SetUp()
    {
        m_tier_default = memtier_tier_new(MEMKIND_DEFAULT);
        m_tier_regular = memtier_tier_new(MEMKIND_REGULAR);
        struct memtier_builder *builder = memtier_builder_new();
        ASSERT_NE(nullptr, m_tier_default);
        ASSERT_NE(nullptr, m_tier_regular);
        ASSERT_NE(nullptr, builder);

        int res = memtier_builder_add_tier(builder, m_tier_default, 1);
        ASSERT_EQ(0, res);
        res = memtier_builder_add_tier(builder, m_tier_regular, 1000);
        ASSERT_EQ(0, res);
        res = memtier_builder_set_policy(builder, MEMTIER_POLICY_CIRCULAR);
        ASSERT_EQ(0, res);
        res = memtier_builder_construct_kind(builder, &m_tier_kind);
        ASSERT_EQ(0, res);
        memtier_builder_delete(builder);
    }

    void TearDown()
    {
        memtier_delete_kind(m_tier_kind);
        memtier_tier_delete(m_tier_default);
        memtier_tier_delete(m_tier_regular);
    }
};

TEST_F(MemkindMemtierTest, test_tier_create_fatal)
{
    struct memtier_tier *tier = memtier_tier_new(nullptr);
    ASSERT_EQ(nullptr, tier);
}

TEST_F(MemkindMemtierTest, test_tier_create_success)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtierTest, test_tier_create_duplicate_fatal)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier);
    struct memtier_tier *tier2 = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_EQ(nullptr, tier2);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtierTest, test_tier_create_duplicate_success)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier);
    memtier_tier_delete(tier);
    struct memtier_tier *tier2 = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier2);
    memtier_tier_delete(tier2);
}

TEST_F(MemkindMemtierTest, test_tier_allocate)
{
    const size_t size = 512;
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier);
    void *ptr = memtier_tier_malloc(tier, size);
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(MEMKIND_DEFAULT, memkind_detect_kind(ptr));
    memtier_free(ptr);
    ptr = memtier_tier_calloc(tier, size, size);
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(MEMKIND_DEFAULT, memkind_detect_kind(ptr));
    void *new_ptr = memtier_tier_realloc(tier, ptr, size);
    ASSERT_NE(nullptr, new_ptr);
    ASSERT_EQ(MEMKIND_DEFAULT, memkind_detect_kind(ptr));
    memtier_free(new_ptr);
    int err = memtier_tier_posix_memalign(tier, &ptr, 64, 32);
    ASSERT_EQ(0, err);
    ASSERT_EQ(MEMKIND_DEFAULT, memkind_detect_kind(ptr));
    memtier_free(ptr);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtierTest, test_tier_builder_failure)
{
    struct memtier_builder *builder = memtier_builder_new();
    ASSERT_NE(nullptr, builder);
    int res = memtier_builder_add_tier(builder, nullptr, 1);
    ASSERT_NE(0, res);
    memtier_builder_delete(builder);
}

TEST_F(MemkindMemtierTest, test_tier_construct_failure_zero_tiers)
{
    struct memtier_kind *tier_kind = nullptr;
    struct memtier_builder *builder = memtier_builder_new();
    int res = memtier_builder_set_policy(builder, MEMTIER_POLICY_CIRCULAR);
    ASSERT_EQ(0, res);
    res = memtier_builder_construct_kind(builder, &tier_kind);
    ASSERT_NE(0, res);
    memtier_builder_delete(builder);
}

TEST_F(MemkindMemtierTest, test_tier_builder_set_policy_failure)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    struct memtier_builder *builder = memtier_builder_new();
    ASSERT_NE(nullptr, tier);
    ASSERT_NE(nullptr, builder);
    int res = memtier_builder_add_tier(builder, tier, 1);
    ASSERT_EQ(0, res);
    memtier_policy_t fake_policy = static_cast<memtier_policy_t>(-1);
    res = memtier_builder_set_policy(builder, fake_policy);
    ASSERT_NE(0, res);
    memtier_tier_delete(tier);
    memtier_builder_delete(builder);
}

TEST_F(MemkindMemtierTest, test_tier_free_nullptr)
{
    for (int i = 0; i < 512; i++) {
        memtier_free(nullptr);
    }
}

TEST_F(MemkindMemtierKindTest, test_tier_builder_allocation_test_success)
{
    const size_t size = 512;
    void *ptr = memtier_kind_malloc(m_tier_kind, size);
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(MEMKIND_DEFAULT, memkind_detect_kind(ptr));
    memtier_free(ptr);
    ptr = memtier_kind_calloc(m_tier_kind, size, size);
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(MEMKIND_REGULAR, memkind_detect_kind(ptr));
    void *new_ptr = memtier_kind_realloc(m_tier_kind, ptr, size);
    ASSERT_NE(nullptr, new_ptr);
    ASSERT_EQ(MEMKIND_REGULAR, memkind_detect_kind(ptr));
    memtier_free(new_ptr);
    int err = memtier_kind_posix_memalign(m_tier_kind, &ptr, 64, 32);
    ASSERT_EQ(0, err);
    ASSERT_EQ(MEMKIND_DEFAULT, memkind_detect_kind(ptr));
    memtier_free(ptr);
}

TEST_F(MemkindMemtierKindTest, test_tier_check_size_empty_tier)
{
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_default));
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_regular));
}

TEST_F(MemkindMemtierKindTest, test_tier_check_size_nullptr)
{
    void *ptr = memtier_kind_malloc(m_tier_kind, 512);
    ASSERT_NE(ptr, nullptr);
    size_t ptr_usable_size = memtier_usable_size(ptr);
    size_t tier_alloc = memtier_tier_allocated_size(m_tier_default);
    tier_alloc += memtier_tier_allocated_size(m_tier_regular);

    ASSERT_EQ(ptr_usable_size, tier_alloc);
    memtier_free(nullptr);
    tier_alloc = memtier_tier_allocated_size(m_tier_default);
    tier_alloc += memtier_tier_allocated_size(m_tier_regular);
    ASSERT_EQ(ptr_usable_size, tier_alloc);

    void *memkind_ptr = memkind_malloc(MEMKIND_DEFAULT, 1024);
    ASSERT_NE(memkind_ptr, nullptr);
    tier_alloc = memtier_tier_allocated_size(m_tier_default);
    tier_alloc += memtier_tier_allocated_size(m_tier_regular);
    ASSERT_EQ(ptr_usable_size, tier_alloc);

    memkind_free(MEMKIND_DEFAULT, memkind_ptr);
    tier_alloc = memtier_tier_allocated_size(m_tier_default);
    tier_alloc += memtier_tier_allocated_size(m_tier_regular);
    ASSERT_EQ(ptr_usable_size, tier_alloc);

    memtier_free(ptr);
    tier_alloc = memtier_tier_allocated_size(m_tier_default);
    tier_alloc += memtier_tier_allocated_size(m_tier_regular);
    ASSERT_EQ(0U, tier_alloc);
}

TEST_F(MemkindMemtierKindTest, test_tier_check_size_malloc_kind)
{
    unsigned i;
    const size_t size_default = 1024;
    const size_t size_regular = 4096;
    const size_t alloc_no = 512;
    size_t counter = 0;
    std::vector<void *> kind_vec;

    for (i = 0; i < alloc_no; ++i) {
        void *ptr = memtier_kind_malloc(m_tier_kind, size_default);
        ASSERT_NE(ptr, nullptr);
        kind_vec.push_back(ptr);
        counter += memtier_usable_size(ptr);

        ptr = memtier_kind_malloc(m_tier_kind, size_regular);
        ASSERT_NE(ptr, nullptr);
        kind_vec.push_back(ptr);
        counter += memtier_usable_size(ptr);
    }

    size_t sum = memtier_tier_allocated_size(m_tier_default) + memtier_tier_allocated_size(m_tier_regular);
    ASSERT_EQ(sum, counter);

    for (auto const &ptr : kind_vec) {
        memtier_free(ptr);
    }
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_default));
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_regular));
}

TEST_F(MemkindMemtierKindTest, test_tier_check_size_calloc_kind)
{
    unsigned i;
    const size_t size_default = 1024;
    const size_t num = 10;
    const size_t size_regular = 4096;
    const size_t alloc_no = 512;
    size_t counter = 0;
    std::vector<void *> kind_vec;

    for (i = 0; i < alloc_no; ++i) {
        void *ptr = memtier_kind_calloc(m_tier_kind, num, size_default);
        ASSERT_NE(ptr, nullptr);
        kind_vec.push_back(ptr);
        counter += memtier_usable_size(ptr);

        ptr = memtier_kind_calloc(m_tier_kind, num, size_regular);
        ASSERT_NE(ptr, nullptr);
        kind_vec.push_back(ptr);
        counter += memtier_usable_size(ptr);
    }

    size_t sum = memtier_tier_allocated_size(m_tier_default) + memtier_tier_allocated_size(m_tier_regular);
    ASSERT_EQ(sum, counter);

    for (auto const &ptr : kind_vec) {
        void *new_ptr = memtier_kind_realloc(m_tier_kind, ptr, 0);
        ASSERT_EQ(new_ptr, nullptr);
    }
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_default));
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_regular));
}

TEST_F(MemkindMemtierKindTest, test_tier_check_size_calloc_kind)
{
    unsigned i;
    const size_t size_default = 1024;
    const size_t num = 10;
    const size_t size_regular = 4096;
    const size_t alloc_no = 512;
    size_t counter = 0;
    std::vector<void *> kind_vec;

    for (i = 0; i < alloc_no; ++i) {
        void *ptr = memtier_kind_realloc(m_tier_kind, num, size_default);
        ASSERT_NE(ptr, nullptr);
        kind_vec.push_back(ptr);
        counter += memtier_usable_size(ptr);

        ptr = memtier_kind_calloc(m_tier_kind, num, size_regular);
        ASSERT_NE(ptr, nullptr);
        kind_vec.push_back(ptr);
        counter += memtier_usable_size(ptr);
    }

    size_t sum = memtier_tier_allocated_size(m_tier_default) + memtier_tier_allocated_size(m_tier_regular);
    ASSERT_EQ(sum, counter);

    for (auto const &ptr : kind_vec) {
        memtier_free(ptr);
    }
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_default));
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_regular));
}

TEST_F(MemkindMemtierKindTest, test_tier_check_size_malloc)
{
    unsigned i;
    const size_t size_default = 1024;
    const size_t size_regular = 4096;
    const size_t alloc_no = 512;
    size_t def_counter = 0;
    size_t reg_counter = 0;
    std::vector<void *> def_vec;
    std::vector<void *> reg_vec;

    for (i = 0; i < alloc_no; ++i) {
        void *ptr = memtier_tier_malloc(m_tier_default, size_default);
        ASSERT_NE(ptr, nullptr);
        def_vec.push_back(ptr);
        def_counter += memtier_usable_size(ptr);

        ptr = memtier_tier_malloc(m_tier_regular, size_regular);
        ASSERT_NE(ptr, nullptr);
        reg_vec.push_back(ptr);
        reg_counter += memtier_usable_size(ptr);
    }

    ASSERT_EQ(def_counter, memtier_tier_allocated_size(m_tier_default));
    ASSERT_EQ(reg_counter, memtier_tier_allocated_size(m_tier_regular));

    for (auto const &ptr : def_vec) {
        void *new_ptr = memtier_tier_realloc(m_tier_default, ptr, 0);
        ASSERT_EQ(new_ptr, nullptr);
    }
    for (auto const &ptr : reg_vec) {
        void *new_ptr = memtier_tier_realloc(m_tier_regular, ptr, 0);
        ASSERT_EQ(new_ptr, nullptr);
    }
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_default));
    ASSERT_EQ(0U, memtier_tier_allocated_size(m_tier_regular));
}

//realloc 3 variants
//posix_memalign
