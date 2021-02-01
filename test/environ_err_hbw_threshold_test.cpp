// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <memkind.h>
#include <memory>
#include <numa.h>
#include <numaif.h>
#include <omp.h>
#include <sys/sysinfo.h>
#include <unordered_map>

#include "memory_topology.h"
#include "TestPrereq.hpp"

#define MB (1024*1024)

using MapNodeSet = std::unordered_map<int, std::unordered_set<int>>;
TestPrereq tp;

class TopologyCfg
{
public:
    TopologyCfg()
    {
        std::string threshold_str = std::getenv("MEMKIND_HBW_THRESHOLD");
        if (threshold_str.empty()) {
            throw std::runtime_error("MEMKIND_HBW_THRESHOLD not set");
        }
        m_threshold_val = std::stoi(threshold_str);
    };
private:
    int m_threshold_val;
};

using TpgPtr = std::unique_ptr<AbstractTopology>;

class KNM_All2All_threshold : public KNM_All2All, TopologyCfg
{
public:
    KNM_All2All_threshold(memkind_t kind):KNM_All2All(kind) {};
};

class KNM_SNC2_threshold : public KNM_SNC2
{
public:
    KNM_SNC2_threshold(memkind_t kind):KNM_SNC2(kind) {}
};

class KNM_SNC4_threshold : public KNM_SNC4, TopologyCfg
{
public:
    KNM_SNC4_threshold(memkind_t kind):KNM_SNC4(kind) {};
};

class CLX_2_var1_threshold : public CLX_2_var1, TopologyCfg
{
public:
    CLX_2_var1_threshold(memkind_t kind):CLX_2_var1(kind) {};
};

class CLX_2_var1_HMAT_threshold : public CLX_2_var1_HMAT, TopologyCfg
{
public:
    CLX_2_var1_HMAT_threshold(memkind_t kind):CLX_2_var1_HMAT(kind) {};
};

class CLX_2_var1_HBW_threshold : public CLX_2_var1_HBW, TopologyCfg
{
public:
    CLX_2_var1_HBW_threshold(memkind_t kind):CLX_2_var1_HBW(kind) {};
};

class CLX_2_var2_threshold : public CLX_2_var2, TopologyCfg
{
public:
    CLX_2_var2_threshold(memkind_t kind):CLX_2_var2(kind) {};
};

class CLX_2_var2_HMAT_threshold : public CLX_2_var2_HMAT, TopologyCfg
{
public:
    CLX_2_var2_HMAT_threshold(memkind_t kind):CLX_2_var2_HMAT(kind) {};
};

class CLX_2_var2_HBW_threshold : public CLX_2_var2_HBW, TopologyCfg
{
public:
    CLX_2_var2_HBW_threshold(memkind_t kind):CLX_2_var2_HBW(kind) {};
};

class CLX_2_var3_threshold : public CLX_2_var3, TopologyCfg
{
public:
    CLX_2_var3_threshold(memkind_t kind):CLX_2_var3(kind) {};
};

class CLX_2_var3_HMAT_threshold : public CLX_2_var3_HMAT, TopologyCfg
{
public:
    CLX_2_var3_HMAT_threshold(memkind_t kind):CLX_2_var3_HMAT(kind) {};
};

class CLX_2_var3_HBW_threshold : public CLX_2_var3_HBW, TopologyCfg
{
public:
    CLX_2_var3_HBW_threshold(memkind_t kind):CLX_2_var3_HBW(kind) {};

};

class CLX_2_var4_HBW_threshold : public CLX_2_var4_HBW, TopologyCfg
{
public:
    CLX_2_var4_HBW_threshold(memkind_t kind):CLX_2_var4_HBW(kind) {};
};

class CLX_4_var1_threshold : public CLX_4_var1, TopologyCfg
{
public:
    CLX_4_var1_threshold(memkind_t kind):CLX_4_var1(kind) {};
};

class CLX_4_var1_HMAT_threshold : public CLX_4_var1_HMAT, TopologyCfg
{
public:
    CLX_4_var1_HMAT_threshold(memkind_t kind):CLX_4_var1_HMAT(kind) {};
};

class CLX_4_var1_HBW_threshold : public CLX_4_var1_HBW, TopologyCfg
{
public:
    CLX_4_var1_HBW_threshold(memkind_t kind):CLX_4_var1_HBW(kind) {};
};

class CLX_4_var2_threshold : public CLX_4_var2, TopologyCfg
{
public:
    CLX_4_var2_threshold(memkind_t kind):CLX_4_var2(kind) {};
};

class CLX_4_var2_HMAT_threshold : public CLX_4_var2_HMAT, TopologyCfg
{
public:
    CLX_4_var2_HMAT_threshold(memkind_t kind):CLX_4_var2_HMAT(kind) {};
};

class CLX_4_var2_HBW_threshold : public CLX_4_var2_HBW, TopologyCfg
{
public:
    CLX_4_var2_HBW_threshold(memkind_t kind):CLX_4_var2_HBW(kind) {};
};

class CLX_4_var3_threshold : public CLX_4_var3, TopologyCfg
{
public:
    CLX_4_var3_threshold(memkind_t kind):CLX_4_var3(kind) {};
};

class CLX_4_var3_HMAT_threshold : public CLX_4_var3_HMAT, TopologyCfg
{
public:
    CLX_4_var3_HMAT_threshold(memkind_t kind):CLX_4_var3_HMAT(kind) {};
};

class CLX_4_var3_HBW_threshold : public CLX_4_var3_HBW, TopologyCfg
{
public:
    CLX_4_var3_HBW_threshold(memkind_t kind):CLX_4_var3_HBW(kind) {};
};

class CLX_4_var4_threshold : public CLX_4_var4, TopologyCfg
{
public:
    CLX_4_var4_threshold(memkind_t kind):CLX_4_var4(kind) {};
};

class CLX_4_var4_HMAT_threshold : public CLX_4_var4_HMAT, TopologyCfg
{
public:
    CLX_4_var4_HMAT_threshold(memkind_t kind):CLX_4_var4_HMAT(kind) {};
};

class CLX_4_var4_HBW_threshold : public CLX_4_var4_HBW, TopologyCfg
{
public:
    CLX_4_var4_HBW_threshold(memkind_t kind):CLX_4_var4_HBW(kind) {};
};

TpgPtr TopologyCfgFactory(std::string kind_name)
{
    std::string env_tpg = std::getenv("MEMKIND_TEST_TOPOLOGY");
    if (env_tpg.empty()) {
        throw std::runtime_error("MEMKIND_TEST_TOPOLOGY not set");
    }
    memkind_t kind = tp.memory_kind_from_str(kind_name);

    if (env_tpg.compare("KnightsMillAll2All") == 0)
        return TpgPtr(new KNM_All2All_threshold(kind));
    else if (env_tpg.compare("KnightsMillSNC2") == 0)
        return TpgPtr(new KNM_SNC2_threshold(kind));
    else if (env_tpg.compare("KnightsMillSNC4") == 0)
        return TpgPtr(new KNM_SNC4_threshold(kind));
    else if (env_tpg.compare("KnightsMillSNC4") == 0)
        return TpgPtr(new KNM_SNC4_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var1") == 0)
        return TpgPtr(new CLX_2_var1_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var1HMAT") == 0)
        return TpgPtr(new CLX_2_var1_HMAT_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var1HBW") == 0)
        return TpgPtr(new CLX_2_var1_HBW_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var2") == 0)
        return TpgPtr(new CLX_2_var2_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var2HMAT") == 0)
        return TpgPtr(new CLX_2_var2_HMAT_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var2HBW") == 0)
        return TpgPtr(new CLX_2_var2_HBW_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var3") == 0)
        return TpgPtr(new CLX_2_var3_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var3HMAT") == 0)
        return TpgPtr(new CLX_2_var3_HMAT_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var3HBW") == 0)
        return TpgPtr(new CLX_2_var3_HBW_threshold(kind));
    else if (env_tpg.compare("CascadeLake2Var4HBW") == 0)
        return TpgPtr(new CLX_2_var4_HBW_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var1") == 0)
        return TpgPtr(new CLX_4_var1_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var1HMAT") == 0)
        return TpgPtr(new CLX_4_var1_HMAT_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var1HBW") == 0)
        return TpgPtr(new CLX_4_var1_HBW_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var2") == 0)
        return TpgPtr(new CLX_4_var2_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var2HMAT") == 0)
        return TpgPtr(new CLX_4_var2_HMAT_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var2HBW") == 0)
        return TpgPtr(new CLX_4_var2_HBW_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var3") == 0)
        return TpgPtr(new CLX_4_var3_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var3HMAT") == 0)
        return TpgPtr(new CLX_4_var3_HMAT_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var3HBW") == 0)
        return TpgPtr(new CLX_4_var3_HBW_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var4") == 0)
        return TpgPtr(new CLX_4_var4_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var4HMAT") == 0)
        return TpgPtr(new CLX_4_var4_HMAT_threshold(kind));
    else if (env_tpg.compare("CascadeLake4Var4HBW") == 0)
        return TpgPtr(new CLX_4_var4_HBW_threshold(kind));
    else
        throw std::runtime_error("Unknown topology");
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: %s [MEMKIND_HBW/MEMKIND_HBW_ALL]" << std::endl;
        return -1;
    }
    if (!tp.is_libhwloc_supported()) {
        return 0;
    }

    if (numa_available() != 0) {
        std::cerr << "numa_available() failed" << std::endl;
        return -1;
    }

    const size_t size = 11*MB-5;
    int threads_num = get_nprocs();
    auto tpg = TopologyCfgFactory(argv[1]);
    bool status = true;
    #pragma omp parallel for num_threads(threads_num)
    for(int thread_id=0; thread_id<threads_num; ++thread_id) {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(thread_id, &cpu_set);
        sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set);
        void *ptr = tpg->allocate(size);
        if (tpg->is_kind_supported()) {
            memset(ptr, 0, size);
            int cpu = sched_getcpu();
            int init_node = numa_node_of_cpu(cpu);
            if(!tpg->verify_kind(init_node, ptr)) {
                status = false;
            }
            tpg->deallocate(ptr);
        } else if (ptr) {
            tpg->deallocate(ptr);
            status = false;
        }
    }
    if (status)
        return 0;
    else
        return -1;
}
