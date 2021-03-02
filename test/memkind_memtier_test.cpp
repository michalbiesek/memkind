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

TEST_F(MemkindMemtiertest, test_tier_API_Dummy)
{
    struct memtier_tier *tier = memtier_tier_new(MEMKIND_DEFAULT);
    memtier_tier_delete(tier);
}
