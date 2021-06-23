#include <linux/bpf.h>
#include "bpf_helpers.h"

#ifdef BTF
struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 1);
	__type(key, __u32);
	__type(value, __u32);
} counter_map SEC(".maps");
#else
struct bpf_map_def SEC("maps") counter_map = {
	.type = BPF_MAP_TYPE_ARRAY,
	.key_size = sizeof(__u32),
	.value_size = sizeof(__u32),
	.max_entries = 1,
};
#endif

SEC("xdp")
int counter(struct xdp_md *ctx)
{
	__u32 key = 0;
	__u32 *counter;

	counter = bpf_map_lookup_elem(&counter_map, &key);
	if (!counter)
		return XDP_PASS;

	*counter = *counter + 1;

	return XDP_PASS;
}
