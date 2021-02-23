// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <memkind.h>
#include <stdlib.h>
/**
 * Header file for the memkind heap manager.
 * More details in memtier(3) man page.
 *
 * API standards are described in memtier(3) man page.
 */

/// \brief Forward declaration
struct tiering_class;
struct tiering_config;
struct tiering_kind;

typedef enum memtier_policy_t
{
    /**
     * TODO FIX ME
     */
    MEMTIER_DUMMY_VALUE = 0,

} memtier_policy_t;

///
/// \brief Create a memtier class
/// \note STANDARD API
/// \return tiering class, NULL on failure
///
struct tiering_class *memtier_cls_new(void);

///
/// \brief Delete memtier class
/// \note STANDARD API
/// \param cls memtier class
///
void memtier_cls_delete(struct tiering_class *cls);

///
/// \brief Set memtier ratio of memtier class
/// \note STANDARD API
/// \param cls memtier class
/// \param ratio expected rati of class
/// \return Operation status, MEMKIND_SUCCESS on success, other values on
/// failure
///
int memtier_cls_set_ratio(struct tiering_class *cls, size_t ratio);

///
/// \brief Set memory kind of memtier class
/// \note STANDARD API
/// \param cls memtier class
/// \param kind memtier kind
/// \return Operation status, MEMKIND_SUCCESS on success, other values on
/// failure
///
int memtier_cls_set_memory_kind(struct tiering_class *cls, memkind_t kind);

///
/// \brief Create a memtier configuration
/// \note STANDARD API
/// \return memtier configuration, NULL on failure
///
struct tiering_config *memtier_config_new();

///
/// \brief Delete memtier configuration
/// \note STANDARD API
/// \param cfg memtier configuration
///
void memtier_config_delete(struct tiering_config *cfg);

///
/// \brief Add memtier class to memtier configuration
/// \note STANDARD API
/// \param cfg memtier configuration
/// \param cls memtier class
/// \return Operation status, MEMKIND_SUCCESS on success, other values on
/// failure
///
int memtier_config_add_tiering_class(struct tiering_config *cfg,
                                     struct tiering_class cls);

///
/// \brief Set memtier policy to memtier configuration
/// \note STANDARD API
/// \param cfg memtier configuration
/// \param policy memtier policy
/// \return Operation status, MEMKIND_SUCCESS on success, other values on
/// failure
///
int memtier_config_set_policy(struct tiering_config *cfg,
                              memtier_policy_t policy);

///
/// \brief Create a memtier kind
/// \note STANDARD API
/// \param cfg memtier configuration
/// \param kind pointer to kind which will be created
/// \return Operation status, MEMKIND_SUCCESS on success, other values on
/// failure
///
int memtier_new_kind_with_config(struct tiering_config *cfg,
                                 struct tiering_kind **kind);

///
/// \brief Delete memtier kind
/// \note STANDARD API
/// \param kind memtier kind
///
void memtier_delete_kind(struct tiering_kind *kind);

///
/// \brief Allocates size bytes of uninitialized storage of the specified
///        tiering kind
/// \note STANDARD API
/// \param kind memtier kind
/// \param size number of bytes to allocate
/// \return Pointer to the allocated memory
///
void *memtier_kind_malloc(struct tiering_kind *kind, size_t size);

///
/// \brief Allocates size bytes of uninitialized storage of the specified
///        tiering class
/// \note STANDARD API
/// \param cls specified tiering class
/// \param size number of bytes to allocate
/// \return Pointer to the allocated memory
void *memtier_cls_malloc(struct tiering_class *cls, size_t size);

///
/// \brief Allocates memory of the specified tiering class for an array of num
///        elements of size bytes each and initializes all bytes in the
///        allocated storage to zero
/// \note STANDARD API
/// \param cls specified tiering class
/// \param num number of objects
/// \param size specified size of each element
/// \return Pointer to the allocated memory
///
void *memtier_cls_calloc(struct tiering_class *cls, size_t num, size_t size);

///
/// \brief Allocates memory of the specified tiering kind for an array of num
///        elements of size bytes each and initializes all bytes in the
///        allocated storage to zero
/// \note STANDARD API
/// \param kind memtier kind
/// \param num number of objects
/// \param size specified size of each element
/// \return Pointer to the allocated memory
///
void *memtier_kind_calloc(struct tiering_kind *kind, size_t num, size_t size);

///
/// \brief Allocates size bytes of the specified tiering class and places the
///        address of the allocated memory in *memptr. The address of the
///        allocated memory will be a multiple of alignment, which must be a
///        power of two and a multiple of sizeof(void*)
/// \note STANDARD API
/// \param cls specified tiering class
/// \param memptr address of the allocated memory
/// \param alignment specified alignment of bytes
/// \param size specified size of bytes
/// \return operation status, MEMKIND_SUCCESS on success, EINVAL or
///         ENOMEM on failure
///
int memtier_cls_posix_memalign(struct tiering_class *cls, void **memptr,
                               size_t alignment, size_t size);

///
/// \brief Allocates size bytes of the specified tiering kind and places the
///        address of the allocated memory in *memptr. The address of the
//         allocated memory will be a multiple of alignment, which must be a
///        power of two and a multiple of sizeof(void*)
/// \note STANDARD API
/// \param kind specified tiering kind
/// \param memptr address of the allocated memory
/// \param alignment specified alignment of bytes
/// \param size specified size of bytes
/// \return operation status, MEMKIND_SUCCESS on success, EINVAL or
///         ENOMEM on failure
///
int memtier_kind_posix_memalign(struct tiering_kind *kind, void **memptr,
                                size_t alignment, size_t size);

///
/// \brief Reallocates memory of the specified tiering class
/// \note STANDARD API
/// \param cls specified tiering class
/// \param ptr pointer to the memory block to be reallocated
/// \param size new size for the memory block in bytes
/// \return Pointer to the allocated memory
///
void *memtier_cls_realloc(struct tiering_class *cls, void *ptr, size_t size);

///
/// \brief Reallocates memory of the specified tiering kind
/// \note STANDARD API
/// \param kind specified tiering kind
/// \param ptr pointer to the memory block to be reallocated
/// \param size new size for the memory block in bytes
/// \return Pointer to the allocated memory
///
void *memtier_kind_realloc(struct tiering_kind *kind, void *ptr, size_t size);

///
/// \brief Obtain size of block of memory allocated with the tiering API
/// \note STANDARD API
/// \param ptr pointer to the allocated memory
/// \return Number of usable bytes
///
size_t memtier_usable_size(void *ptr);

///
/// \brief Free the memory space allocated with the tiering API
/// \note STANDARD API
/// \param ptr pointer to the allocated memory
///
void memtier_free(void *ptr);

#ifdef __cplusplus
}
#endif
