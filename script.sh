#!/usr/bin/env bash

set -e

# Compile bpftool

cd "$HOME"
cd dev/linux
make -j8 -C tools/bpf/bpftool install doc-install
cd

# Simple Introspection

cd echo

clang -O2 -target bpf -c counter.c -o counter_nobtf.o
llvm-objdump -d counter_nobtf.o

bpftool prog load counter_nobtf.o /sys/fs/bpf/counter_nobtf type xdp
bpftool prog show --bpffs
bpftool prog dump xlated pinned /sys/fs/bpf/counter_nobtf
bpftool prog dump jited pinned /sys/fs/bpf/counter_nobtf

bpftool map show
bpftool map dump name counter_map

# Attach to XDP

bpftool net attach xdp pinned /sys/fs/bpf/counter_nobtf dev enp3s0
bpftool net show
bpftool -p net show
bpftool -j net show | jq '.[].xdp[] | select(.devname == "enp3s0") | .id // .multi_attachments[].id'
ip link show dev enp3s0
bpftool map dump name counter_map
bpftool net detach xdp dev enp3s0
rm /sys/fs/bpf/counter_nobtf

## Run /usr/share/bcc/tools/execsnoop
bpftool perf show

# Again, with BTF

clang -O2 -target bpf -c counter.c -o counter_btf.o -g -D BTF
bpftool prog load counter_btf.o /sys/fs/bpf/counter_btf type xdp --debug
bpftool prog dump xlated name counter

bpftool net attach xdp pinned /sys/fs/bpf/counter_btf dev enp3s0
id=$(bpftool map -j | jq -r '.[] | select(.name == "counter_map") | select(.btf_id) | .id')
bpftool map dump id "$id"
bpftool net detach xdp dev enp3s0

btf_id=$(bpftool map -j | jq -r '.[] | select(.name == "counter_map") | select(.btf_id) | .btf_id')
bpftool btf dump id "$btf_id"
bpftool btf show

# More Complex Example

cd app

bpftool feature probe

sysctl -w kernel.bpf_stats_enabled=1

for i in *.c ; do clang -O2 -g -target bpf -c "$i" -o "${i%.c}.o"; done
bpftool gen object app.o *.o
bpftool prog loadall app.o /sys/fs/bpf/app type xdp map name counter_map name counter_map pinmaps /sys/fs/bpf/app
ls -l /sys/fs/bpf/app

## python3 -c 'print("\0"*16+"> Hello eCHO! <3", end="")'>/tmp/data
## bpftool prog tracelog
bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 3
bpftool map dump name call_table
bpftool map update name call_table key 0 0 0 0 value pinned /sys/fs/bpf/app/xdp_counter
bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 3
bpftool map dump name counter_map
bpftool map update name call_table key 0 0 0 0 value pinned /sys/fs/bpf/app/xdp_streak
bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 5
bpftool map dump name streak_map

bpftool prog show
## bpftool prog profile pinned /sys/fs/bpf/app/xdp_entrypoint cycles instructions l1d_loads
bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 10

bpftool prog dump xlated name streak visual | tee /dev/tty | dot -Tpng > cfg.png
xview cfg.png
