#include <linux/bpf.h>
#include "../bpf_helpers.h"
#include "main.h"

SEC("xdp/entrypoint")
int entrypoint(struct xdp_md *ctx)
{
	bpf_printk("function: entrypoint\n");

	return do_stuff(ctx);
}

char _license[] SEC("license") = "GPL";
