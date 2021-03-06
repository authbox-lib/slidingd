#ifndef SET_MANAGER_H
#define SET_MANAGER_H
#include <pthread.h>
#include "config.h"
#include "set.h"

/**
 * Opaque handle to the set manager
 */
typedef struct hlld_setmgr hlld_setmgr;

/**
 * Lists of sets
 */
struct hlld_set_list {
    char *full_key;
    struct hlld_set_list *next;
};

struct hlld_set_list_head {
   int size;
   struct hlld_set_list *head;
   struct hlld_set_list *tail;
};

/**
 * Defines the number of keys we set/check in a single
 * iteration for our multi commands. We do not do all the
 * keys at one time to prevent a client from holding locks
 * for too long. This is especially critical for set
 * operations which serialize access.
 */
#define MULTI_OP_SIZE 32

/**
 * Defines the number of keys before a hll is converted to the dense format.
 */
#define SPARSE_MAX_VALUES 256

/**
 * Initializer
 * @arg config The configuration
 * @arg vacuum Should vacuuming be enabled. True unless in a
 * test or embedded environment using setmgr_vacuum()
 * @arg mgr Output, resulting manager.
 * @return 0 on success.
 */
int init_set_manager(struct hlld_config *config, int vacuum, struct hlld_setmgr **mgr);

/**
 * Cleanup
 * @arg mgr The manager to destroy
 * @return 0 on success.
 */
int destroy_set_manager(struct hlld_setmgr *mgr);

/**
 * Fetch statistics
 * @arg mgr The manager
 * @return malloc'de string of human readable stats
 */
char *setmgr_get_stats(struct hlld_setmgr *mgr);

/**
 * Should be invoked periodically by client threads to allow
 * the vacuum thread to cleanup garbage state. It should also
 * be called before making other calls into the set manager
 * so that it is aware of a client making use of the current
 * state.
 * @arg mgr The manager
 */
void setmgr_client_checkpoint(struct hlld_setmgr *mgr);

/**
 * Should be invoked by clients when they no longer
 * need to make use of the set manager. This
 * allows the vacuum thread to cleanup garbage state.
 * @arg mgr The manager
 */
void setmgr_client_leave(struct hlld_setmgr *mgr);

/**
 * Flushes the set with the given name
 * @arg set_name The name of the set to flush
 * @return 0 on success. -1 if the set does not exist.
 */
int setmgr_flush_dense_set(struct hlld_setmgr *mgr, char *set_name);

/**
 * Sets keys in a given set
 * @arg set_name The name of the set
 * @arg keys A list of points to character arrays to add
 * @arg num_keys The number of keys to add
 * @return 0 on success, -1 if the set does not exist.
 * -2 on internal error.
 */
int setmgr_set_keys(struct hlld_setmgr *mgr, char *full_key, int full_key_len, char **keys, int num_keys, time_t time);

/**
 * Estimates the size of a set
 * @arg set_name The name of the set
 * @arg est Output pointer, the estimate on success.
 * @return 0 on success, -1 if the set does not exist.
 */
int setmgr_set_size(struct hlld_setmgr *mgr, char *full_key, int full_key_len, uint64_t *est, time_t timestamp, uint64_t time_window);

/**
 * Estimates the total size of a set
 * @arg set_name The name of the set
 * @arg est Output pointer, the estimate on success.
 * @return 0 on success, -1 if the set does not exist.
 */
int setmgr_set_size_total(struct hlld_setmgr *mgr, char *full_key, int full_key_len, uint64_t *est);
int setmgr_dense_set_size_total(struct hlld_setmgr *mgr, char *set_name, uint64_t *est);

/**
 * Estimates the size of the union of the sets
 * @arg set_name The name of the set
 * @arg est Output pointer, the estimate on success.
 * @return 0 on success, -1 if the set does not exist.
 */
int setmgr_set_union_size(struct hlld_setmgr *mgr, int num_sets, char **full_keys, int *full_key_lens, uint64_t *est, uint64_t time_window);

/**
 * Deletes the set entirely. This removes it from the set
 * manager and deletes it from disk. This is a permanent operation.
 * @arg set_name The name of the set to delete
 * @return 0 on success, -1 if the set does not exist.
 */
int setmgr_drop_set(struct hlld_setmgr *mgr, char *full_key, int full_key_len);

/**
 * Unmaps the set from memory, but leaves it
 * registered in the set manager. This is rarely invoked
 * by a client, as it can be handled automatically by hlld,
 * but particular clients with specific needs may use it as an
 * optimization.
 * @arg set_name The name of the set to delete
 * @return 0 on success, -1 if the set does not exist.
 */
int setmgr_unmap_dense_set(struct hlld_setmgr *mgr, char *set_name);

/**
 * Clears the set from the internal data stores. This can only
 * be performed if the set is proxied.
 * @arg set_name The name of the set to delete
 * @return 0 on success, -1 if the set does not exist, -2
 * if the set is not proxied.
 */
int setmgr_clear_set(struct hlld_setmgr *mgr, char *full_key, int full_key_len);

/**
 * Allocates space for and returns a linked
 * list of all the sets. The memory should be free'd by
 * the caller.
 * @arg mgr The manager to list from
 * @arg prefix The prefix to match on or NULL
 * @arg head Output, sets to the address of the list header
 * @return 0 on success.
 */
int setmgr_list_sets(struct hlld_setmgr *mgr, const char *prefix, struct hlld_set_list_head **head);

/**
 * Allocates space for and returns a linked
 * list of all the cold sets. This has the side effect
 * of clearing the list of cold sets! The memory should
 * be free'd by the caller.
 * @arg mgr The manager to list from
 * @arg head Output, sets to the address of the list header
 * @return 0 on success.
 */
int setmgr_list_cold_sets(struct hlld_setmgr *mgr, struct hlld_set_list_head **head);

/**
 * Convenience method to cleanup a set list.
 */
void setmgr_cleanup_list(struct hlld_set_list_head *head);

/**
 * Convenience method to cleanup a set list.
 */
int setmgr_get_hashes(struct hlld_setmgr *mgr, char *full_key, int full_key_len, uint64_t **hashes, size_t *count);

/**
 * This method allows a callback function to be invoked with hlld set.
 * The purpose of this is to ensure that a hlld set is not deleted or
 * otherwise destroyed while being referenced. The set is not locked
 * so clients should under no circumstance use this to read/write to the set.
 * It should be used to read metrics, size information, etc.
 * @return 0 on success, -1 if the set does not exist.
 */
typedef int(*set_cb)(void* in, char *full_key,  struct hlld_set *set);
int setmgr_dense_set_cb(struct hlld_setmgr *mgr, char *full_key, int full_key_len, set_cb cb, void* data);
int setmgr_set_cb(struct hlld_setmgr *mgr, char *full_key, set_cb cb, void* data);

/**
 * This method is used to force a vacuum up to the current
 * version. It is generally unsafe to use in hlld,
 * but can be used in an embeded or test environment.
 */
void setmgr_vacuum(struct hlld_setmgr *mgr);

#endif
