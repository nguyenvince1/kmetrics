#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xf296206e, "__num_possible_cpus" },
	{ 0x3a645690, "__bitmap_weight" },
	{ 0xc7ffe1aa, "si_meminfo" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x85acaba2, "cancel_delayed_work_sync" },
	{ 0x155e60e3, "misc_deregister" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0xe8213e80, "_printk" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xb5c51982, "__cpu_online_mask" },
	{ 0x9aa6980d, "mutex_lock" },
	{ 0x9aa6980d, "mutex_unlock" },
	{ 0xecfe08b6, "cpufreq_quick_get" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0xe4de56b4, "__ubsan_handle_load_invalid_value" },
	{ 0xd710adbf, "__kmalloc_noprof" },
	{ 0x71798f7e, "delayed_work_timer_fn" },
	{ 0x02f9bbf0, "timer_init_key" },
	{ 0xe98a2697, "misc_register" },
	{ 0xaef1f20d, "system_percpu_wq" },
	{ 0x8ce83585, "queue_delayed_work_on" },
	{ 0x40a621c5, "scnprintf" },
	{ 0x86632fd6, "_find_next_bit" },
	{ 0x437e81c7, "simple_read_from_buffer" },
	{ 0x47892a40, "get_cpu_idle_time" },
	{ 0xd272d446, "__fentry__" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x12ca6142, "ktime_get_coarse_with_offset" },
	{ 0xf296206e, "nr_cpu_ids" },
	{ 0xb5c51982, "__cpu_present_mask" },
	{ 0xb6d2afcd, "__num_online_cpus" },
	{ 0xe9196a28, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xf296206e,
	0x3a645690,
	0xc7ffe1aa,
	0xd272d446,
	0xd272d446,
	0x85acaba2,
	0x155e60e3,
	0xcb8b6ec6,
	0xe8213e80,
	0x092a35a2,
	0xb5c51982,
	0x9aa6980d,
	0x9aa6980d,
	0xecfe08b6,
	0x092a35a2,
	0xe4de56b4,
	0xd710adbf,
	0x71798f7e,
	0x02f9bbf0,
	0xe98a2697,
	0xaef1f20d,
	0x8ce83585,
	0x40a621c5,
	0x86632fd6,
	0x437e81c7,
	0x47892a40,
	0xd272d446,
	0xbd03ed67,
	0x12ca6142,
	0xf296206e,
	0xb5c51982,
	0xb6d2afcd,
	0xe9196a28,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__num_possible_cpus\0"
	"__bitmap_weight\0"
	"si_meminfo\0"
	"__x86_return_thunk\0"
	"__stack_chk_fail\0"
	"cancel_delayed_work_sync\0"
	"misc_deregister\0"
	"kfree\0"
	"_printk\0"
	"_copy_from_user\0"
	"__cpu_online_mask\0"
	"mutex_lock\0"
	"mutex_unlock\0"
	"cpufreq_quick_get\0"
	"_copy_to_user\0"
	"__ubsan_handle_load_invalid_value\0"
	"__kmalloc_noprof\0"
	"delayed_work_timer_fn\0"
	"timer_init_key\0"
	"misc_register\0"
	"system_percpu_wq\0"
	"queue_delayed_work_on\0"
	"scnprintf\0"
	"_find_next_bit\0"
	"simple_read_from_buffer\0"
	"get_cpu_idle_time\0"
	"__fentry__\0"
	"__ref_stack_chk_guard\0"
	"ktime_get_coarse_with_offset\0"
	"nr_cpu_ids\0"
	"__cpu_present_mask\0"
	"__num_online_cpus\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2D1E5D740C1A032F7D9483B");
