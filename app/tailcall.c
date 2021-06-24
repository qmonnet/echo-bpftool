#include <linux/bpf.h>
#include "../bpf_helpers.h"

/*
 * Tail call example: just perform a tail call to the first (and only) program
 * referenced by the prog_array map.
 */

struct {
	__uint(type, BPF_MAP_TYPE_PROG_ARRAY);
	__uint(max_entries, 1);
	__type(key, __u32);
	__type(value, __u32);
} call_table SEC(".maps");

__noinline int do_stuff(struct xdp_md *ctx)
{
	bpf_printk("function: tailcall\n");

	bpf_tail_call_static(ctx, &call_table, 0);

	bpf_printk("tail call failed\n");

	return XDP_PASS;
}
