// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <memkind_memtier.h>

#include "TestPrereq.hpp"
#include "common.h"

class MemkindMemtiertest: public ::testing::Test
{
protected:
    const char *memory_tpg;
    TestPrereq tp;
    void SetUp()
    {}

    void TearDown()
    {}
};

TEST_F(MemkindMemtiertest, test_tier_create_fatal)
{
    struct memtier_tier *tier = memtier_tier_new(nullptr);
    ASSERT_EQ(nullptr, tier);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtiertest, test_tier_create_success)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtiertest, test_tier_create_duplicate_fatal)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier);
    struct memtier_tier *tier2 = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_EQ(nullptr, tier2);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtiertest, test_tier_create_duplicate_success)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier);
    memtier_tier_delete(tier);
    struct memtier_tier *tier2 = memtier_tier_new(MEMKIND_DEFAULT);
    ASSERT_NE(nullptr, tier2);
    memtier_tier_delete(tier2);
}

TEST_F(MemkindMemtiertest, test_tier_allocate)
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
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtiertest, test_tier_builder_failure)
{
    struct memtier_builder *builder = memtier_builder();
    int res = memtier_builder_add_tier(builder, NULL, 1);
    ASSERT_NE(0, res);
}

TEST_F(MemkindMemtiertest, test_tier_builder_set_policy_failure)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    struct memtier_builder *builder = memtier_builder();
    ASSERT_NE(nullptr, tier);
    int res = memtier_builder_add_tier(builder, tier, 1);
    ASSERT_EQ(0, res);
    memtier_policy_t fake_policy = static_cast<memtier_policy_t>(-1);
    res = memtier_builder_set_policy(builder, fake_policy);
    ASSERT_NE(0, res);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtiertest, test_tier_builder_success)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    struct memtier_builder *builder = memtier_builder();
    struct memtier_kind *tier_kind = nullptr;
    ASSERT_NE(nullptr, tier);
    int res = memtier_builder_add_tier(builder, tier, 1);
    ASSERT_EQ(0, res);
    res = memtier_builder_set_policy(builder, MEMTIER_DUMMY_VALUE);
    ASSERT_EQ(0, res);
    res = memtier_builder_construct_kind(builder, &tier_kind);
    ASSERT_EQ(0, res);
    memtier_delete_kind(tier_kind);
    memtier_tier_delete(tier);
}

TEST_F(MemkindMemtiertest, test_tier_builder_allocation_test_success)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    struct memtier_builder *builder = memtier_builder();
    struct memtier_kind *tier_kind = nullptr;
    ASSERT_NE(nullptr, tier);
    int res = memtier_builder_add_tier(builder, tier, 1);
    ASSERT_EQ(0, res);
    res = memtier_builder_set_policy(builder, MEMTIER_DUMMY_VALUE);
    ASSERT_EQ(0, res);
    res = memtier_builder_construct_kind(builder, &tier_kind);
    ASSERT_EQ(0, res);

    memtier_delete_kind(tier_kind);
    memtier_tier_delete(tier);
}
