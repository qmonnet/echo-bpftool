#pragma once
#include <linux/bpf.h>
#include "../bpf_helpers.h"

int do_stuff(struct xdp_md *ctx);
