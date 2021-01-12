// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */
#pragma once

#include <climits>
#include <unordered_set>

#include "common.h"
#include "numa.h"
#include "numaif.h"
#include "config.h"

namespace TestPrereq
{
    enum memory_var{HBM, PMEM};

    #define CPUID_MODEL_SHIFT       (4)
    #define CPUID_MODEL_MASK        (0xf)
    #define CPUID_EXT_MODEL_MASK    (0xf)
    #define CPUID_EXT_MODEL_SHIFT   (16)
    #define CPUID_FAMILY_MASK       (0xf)
    #define CPUID_FAMILY_SHIFT      (8)
    #define CPU_MODEL_KNL           (0x57)
    #define CPU_MODEL_KNM           (0x85)
    #define CPU_MODEL_CLX           (0x55)
    #define CPU_FAMILY_INTEL        (0x06)

    typedef struct {
        uint32_t model;
        uint32_t family;
    } cpu_model_data_t;

    typedef struct registers_t {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
    } registers_t;

    inline static void cpuid_asm(int leaf, int subleaf, registers_t *registers)
    {
    #ifdef __x86_64__
        asm volatile("cpuid":"=a"(registers->eax),
                    "=b"(registers->ebx),
                    "=c"(registers->ecx),
                    "=d"(registers->edx):"0"(leaf), "2"(subleaf));
    #else
        registers->eax = 0;
    #endif
    }

    static cpu_model_data_t get_cpu_model_data()
    {
        registers_t registers;
        cpuid_asm(1, 0, &registers);
        uint32_t model = (registers.eax >> CPUID_MODEL_SHIFT) & CPUID_MODEL_MASK;
        uint32_t model_ext = (registers.eax >> CPUID_EXT_MODEL_SHIFT) &
                            CPUID_EXT_MODEL_MASK;

        cpu_model_data_t data;
        data.model = model | (model_ext << 4);
        data.family = (registers.eax >> CPUID_FAMILY_SHIFT) & CPUID_FAMILY_MASK;
        return data;
    }


    bool check_cpu(memory_var variant)
    {
        cpu_model_data_t cpu = get_cpu_model_data();
        std::cout<<"check_cpu "<<std::endl;
        std::cout<<"FAMILY: " << cpu.family << std::endl;
        std::cout<<"MODEL: " << cpu.model << std::endl;
        switch(variant)
        {
            case HBM:
                return cpu.family == CPU_FAMILY_INTEL &&
                (cpu.model == CPU_MODEL_KNL || cpu.model == CPU_MODEL_KNM);
            case PMEM:
                return cpu.family == CPU_FAMILY_INTEL && (cpu.model == CPU_MODEL_CLX);
            default:
                return false;
        }
    }

    std::unordered_set<int> get_closest_numa_nodes(int first_node, std::unordered_set<int> nodes)
    {
        int min_distance = INT_MAX;
        std::unordered_set<int> closest_numa_ids;

        for (auto const &node: nodes) {
            int distance_to_i_node = numa_distance(first_node, node);

            if (distance_to_i_node < min_distance) {
                min_distance = distance_to_i_node;
                closest_numa_ids.clear();
                closest_numa_ids.insert(node);
            } else if (distance_to_i_node == min_distance) {
                closest_numa_ids.insert(node);
            }
        }
        return closest_numa_ids;
    }

    bool is_libhwloc_supported(void)
    {
        #ifdef MEMKIND_HWLOC
            return true;
        #else
            return false;
        #endif
    }

    bool is_libdaxctl_supported(void)
    {
        #ifdef MEMKIND_DAXCTL_KMEM
            return true;
        #else
            return false;
        #endif
    }

    bool is_MCDRAM_present(void)
    {
        #ifdef MEMKIND_HWLOC
        hwloc_obj_t node = NULL;
        while ((node = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NUMANODE, node)) != NULL) {
            if (node->subtype && !strcmp(node->subtype, "MCDRAM")) {
                return true;
            }
        }
        return false;
        #else
            return false;
        #endif
    }

    bool is_KMEM_DAX_PRESENT(void)
    {
        #ifdef MEMKIND_HWLOC
        hwloc_obj_t node = NULL;
        while ((node = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NUMANODE, node)) != NULL) {
            if (hwloc_obj_get_info_by_name(obj, "DAXDevice") != NULL) {
                return true;
            }
        }
        return false;
        #else
            return false;
        #endif
    }

    std::unordered_set<int> get_memory_only_numa_nodes(void)
    {
        struct bitmask *cpu_mask = numa_allocate_cpumask();
        std::unordered_set<int> mem_only_nodes;

        const int MAXNODE_ID = numa_max_node();
        for (int id = 0; id <= MAXNODE_ID; ++id) {
            int res = numa_node_to_cpus(id, cpu_mask);
            if (res == -1) {
                continue;
            }

            if (numa_node_size64(id, nullptr) > 0 &&
                numa_bitmask_weight(cpu_mask) == 0) {
                mem_only_nodes.insert(id);
            }
        }
        numa_free_cpumask(cpu_mask);

        return mem_only_nodes;
    }

    std::unordered_set<int> get_regular_numa_nodes(void)
    {
        struct bitmask *cpu_mask = numa_allocate_cpumask();
        std::unordered_set<int> regular_nodes;

        const int MAXNODE_ID = numa_max_node();
        for (int id = 0; id <= MAXNODE_ID; ++id) {
            int res = numa_node_to_cpus(id, cpu_mask);
            if (res == -1) {
                continue;
            }
            if (numa_bitmask_weight(cpu_mask) != 0) {
                regular_nodes.insert(id);
            }
        }
        numa_free_cpumask(cpu_mask);

        return regular_nodes;
    }

    size_t get_free_space(std::unordered_set<int> nodes)
    {
        size_t sum_of_free_space = 0;
        long long free_space;

        for(auto const &node: nodes) {
            int result = numa_node_size64(node, &free_space);
            if (result == -1)
                continue;
            sum_of_free_space += free_space;
        }

        return sum_of_free_space;
    }

    bool is_HighBandWidthSupported(void)
    {
        if ((getenv("MEMKIND_HBW_NODES") != NULL) || is_MCDRAM_present())
            return true;
        auto mem_only_nodes = get_memory_only_numa_nodes();
        if (mem_only_nodes.size() == 0)
            return false;
        return false;
    }

    bool is_KMEM_DAX_Supported(void)
    {
        if ((getenv("MEMKIND_DAX_KMEM_NODES") != NULL) || is_KMEM_DAX_PRESENT())
            return true;
        std::unordered_set<int> mem_only_nodes = get_memory_only_numa_nodes();
        if (mem_only_nodes.size() == 0)
            return false;
        return true;
    }
}
