#include <linux/bpf.h>
#include <linux/if_ether.h>
#include "../bpf_helpers.h"

/*
 * Use a LRU hash map to count how many packets in a row we saw for the last
 * three observed Ethertype values. There is no real interest here, this is just
 * a demo program.
 */

struct {
	__uint(type, BPF_MAP_TYPE_LRU_HASH);
	__uint(max_entries, 3);
	__type(key, __u16);
	__type(value, __u32);
} streak_map SEC(".maps");

SEC("xdp/streak")
int streak(struct xdp_md *ctx)
{
	void *data_end = (void *)(long)ctx->data_end;
	void *data = (void *)(long)ctx->data;
	struct ethhdr *eth = data;
	__u32 *counter;
	__u16 h_proto;

	bpf_printk("function: streak\n");

	if (data + sizeof(*eth) > data_end)
		return XDP_PASS;

	h_proto = eth->h_proto;

	counter = bpf_map_lookup_elem(&streak_map, &h_proto);

	if (counter) {
		*counter = *counter + 1;
	} else {
		__u32 one = 1;

		bpf_map_update_elem(&streak_map, &h_proto, &one, BPF_ANY);
	}

	return XDP_PASS;
}
