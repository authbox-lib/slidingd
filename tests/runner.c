#include <check.h>
#include <stdio.h>
#include <syslog.h>
#include "test_config.c"
#include "test_hll.c"
#include "test_shll.c"
#include "test_set.c"
#include "test_setmgr.c"
#include "test_art.c"
#include "test_serialize.c"
#include "test_sparse.c"

int main(void)
{
    setlogmask(LOG_UPTO(LOG_DEBUG));

    Suite *s1 = suite_create("hlld");
    /*
    TCase *tc1 = tcase_create("config");
    TCase *tc4 = tcase_create("shll");
    TCase *tc5 = tcase_create("set");
    TCase *tc7 = tcase_create("art");
    TCase *tc8 = tcase_create("hll");
    TCase *tc9 = tcase_create("serialize");
    */
    TCase *tc6 = tcase_create("manager");
    TCase *tc10 = tcase_create("sparse");
    SRunner *sr = srunner_create(s1);
    int nf;

    /*
    // Add the config tests
    suite_add_tcase(s1, tc1);
    tcase_add_test(tc1, test_config_get_default);
    tcase_add_test(tc1, test_config_bad_file);
    tcase_add_test(tc1, test_config_empty_file);
    tcase_add_test(tc1, test_config_basic_config);
    tcase_add_test(tc1, test_config_basic_config_precision);
    tcase_add_test(tc1, test_validate_default_config);
    tcase_add_test(tc1, test_validate_bad_config);
    tcase_add_test(tc1, test_join_path_no_slash);
    tcase_add_test(tc1, test_join_path_with_slash);
    tcase_add_test(tc1, test_sane_log_level);
    tcase_add_test(tc1, test_sane_default_eps);
    tcase_add_test(tc1, test_sane_default_precision);
    tcase_add_test(tc1, test_sane_flush_interval);
    tcase_add_test(tc1, test_sane_cold_interval);
    tcase_add_test(tc1, test_sane_in_memory);
    tcase_add_test(tc1, test_sane_use_mmap);
    tcase_add_test(tc1, test_sane_worker_threads);
    tcase_add_test(tc1, test_set_config_bad_file);
    tcase_add_test(tc1, test_set_config_empty_file);
    tcase_add_test(tc1, test_set_config_basic_config);
    tcase_add_test(tc1, test_update_filename_from_set_config);

    // Add the hll tests
    suite_add_tcase(s1, tc4);
    tcase_add_test(tc4, test_hll_init_bad);
    tcase_add_test(tc4, test_hll_init_and_destroy);
    tcase_add_test(tc4, test_hll_add);
    tcase_add_test(tc4, test_hll_add_hash);
    tcase_add_test(tc4, test_hll_convert_dense);
    tcase_add_test(tc4, test_hll_add_size);
    tcase_add_test(tc4, test_hll_size_total);
    tcase_add_test(tc4, test_hll_error_bound);
    tcase_add_test(tc4, test_hll_precision_for_error);
    tcase_add_test(tc4, test_hll_error_for_precision);
    tcase_add_test(tc4, test_hll_bytes_for_precision);
    tcase_add_test(tc4, test_hll_union_sparse);
    tcase_add_test(tc4, test_hll_union_dense);

    // Add the set tests
    suite_add_tcase(s1, tc5);
    tcase_set_timeout(tc5, 3);
    tcase_add_test(tc5, test_set_init_destroy);
    tcase_add_test(tc5, test_set_init_discover_destroy);
    tcase_add_test(tc5, test_set_init_discover_delete);
    tcase_add_test(tc5, test_set_init_proxied);
    tcase_add_test(tc5, test_set_add);
    tcase_add_test(tc5, test_set_restore);
    tcase_add_test(tc5, test_set_flush);
    tcase_add_test(tc5, test_set_add_in_mem);
    tcase_add_test(tc5, test_set_page_out);

    */
    // Add the set manager tests
    suite_add_tcase(s1, tc6);
    tcase_set_timeout(tc6, 3);
    tcase_add_test(tc6, test_mgr_init_destroy);
    tcase_add_test(tc6, test_mgr_create_drop);
    tcase_add_test(tc6, test_mgr_create_double_drop);
    tcase_add_test(tc6, test_mgr_list);
    tcase_add_test(tc6, test_mgr_list_prefix);
    tcase_add_test(tc6, test_mgr_list_no_sets);
    tcase_add_test(tc6, test_mgr_add_keys);
    tcase_add_test(tc6, test_mgr_add_no_set);
    tcase_add_test(tc6, test_mgr_flush_no_set);
    tcase_add_test(tc6, test_mgr_flush);
    tcase_add_test(tc6, test_mgr_unmap_no_set);
    tcase_add_test(tc6, test_mgr_unmap);
    tcase_add_test(tc6, test_mgr_unmap_add_keys);
    tcase_add_test(tc6, test_mgr_clear_no_set);
    tcase_add_test(tc6, test_mgr_clear_not_proxied);
    tcase_add_test(tc6, test_mgr_clear);
    tcase_add_test(tc6, test_mgr_clear_reload);
    tcase_add_test(tc6, test_mgr_list_cold_no_sets);
    tcase_add_test(tc6, test_mgr_list_cold);
    tcase_add_test(tc6, test_mgr_unmap_in_mem);
    tcase_add_test(tc6, test_mgr_restore);
    tcase_add_test(tc6, test_mgr_callback);
    /*

    // Add the art tests
    suite_add_tcase(s1, tc7);
    tcase_add_test(tc7, test_art_init_and_destroy);
    tcase_add_test(tc7, test_art_insert);
    tcase_add_test(tc7, test_art_insert_verylong);
    tcase_add_test(tc7, test_art_insert_search);
    tcase_add_test(tc7, test_art_insert_delete);
    tcase_add_test(tc7, test_art_insert_iter);
    tcase_add_test(tc7, test_art_iter_prefix);
    tcase_add_test(tc7, test_art_insert_copy_delete);

    suite_add_tcase(s1, tc8);
    tcase_set_timeout(tc8, 3);
    tcase_add_test(tc8, test_shll_init_and_destroy);
    tcase_add_test(tc8, test_shll_add_register);
    tcase_add_test(tc8, test_shll_add_hash);
    tcase_add_test(tc8, test_shll_remove_smaller);
    tcase_add_test(tc8, test_shll_remove_time);
    tcase_add_test(tc8, test_shll_shrink_register);
    tcase_add_test(tc8, test_shll_error_bound);
    tcase_add_test(tc8, test_shll_time_queries);

    suite_add_tcase(s1, tc9);
    tcase_set_timeout(tc8, 3);
    tcase_add_test(tc9, test_serialize_primitives);
    tcase_add_test(tc9, test_hll_serialize);
    tcase_add_test(tc9, test_hll_serialize_sparse);
    tcase_add_test(tc9, test_hll_serialize_registers);
    tcase_add_test(tc9, test_serialize_register);
    */

    suite_add_tcase(s1, tc10);
    tcase_add_test(tc10, test_sparse_init_destroy);
    tcase_add_test(tc10, test_sparse_insert);
    tcase_add_test(tc10, test_sparse_convert);


    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}

