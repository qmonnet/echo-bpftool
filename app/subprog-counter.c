#include <linux/bpf.h>
#include "../bpf_helpers.h"

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 1);
	__type(key, __u32);
	__type(value, __u32);
} counter_map SEC(".maps");

SEC("xdp/counter")
int counter(struct xdp_md *ctx)
{
	__u32 key = 0;
	__u32 *counter;

	bpf_printk("function: counter\n");

	counter = bpf_map_lookup_elem(&counter_map, &key);
	if (!counter)
		return XDP_PASS;

	*counter = *counter + 1;

	return XDP_PASS;
}
