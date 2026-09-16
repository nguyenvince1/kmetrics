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
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x12ca6142, "ktime_get_coarse_with_offset" },
	{ 0xf296206e, "__num_possible_cpus" },
	{ 0xf296206e, "nr_cpu_ids" },
	{ 0xb5c51982, "__cpu_present_mask" },
	{ 0xb6d2afcd, "__num_online_cpus" },
	{ 0x3a645690, "__bitmap_weight" },
	{ 0xc7ffe1aa, "si_meminfo" },
	{ 0x40a621c5, "scnprintf" },
	{ 0x437e81c7, "simple_read_from_buffer" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x155e60e3, "misc_deregister" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe98a2697, "misc_register" },
	{ 0xe8213e80, "_printk" },
	{ 0xe9196a28, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0xbd03ed67,
	0x12ca6142,
	0xf296206e,
	0xf296206e,
	0xb5c51982,
	0xb6d2afcd,
	0x3a645690,
	0xc7ffe1aa,
	0x40a621c5,
	0x437e81c7,
	0xd272d446,
	0x155e60e3,
	0xd272d446,
	0xe98a2697,
	0xe8213e80,
	0xe9196a28,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__x86_return_thunk\0"
	"__ref_stack_chk_guard\0"
	"ktime_get_coarse_with_offset\0"
	"__num_possible_cpus\0"
	"nr_cpu_ids\0"
	"__cpu_present_mask\0"
	"__num_online_cpus\0"
	"__bitmap_weight\0"
	"si_meminfo\0"
	"scnprintf\0"
	"simple_read_from_buffer\0"
	"__stack_chk_fail\0"
	"misc_deregister\0"
	"__fentry__\0"
	"misc_register\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "5A07F55D0635AEC43A3B992");
