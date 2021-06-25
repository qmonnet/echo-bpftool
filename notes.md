# Compile bpftool

- Compile from Linux source.
- Targets `install` and `doc-install` build and install bpftool and its
  documentatoin.

    $ cd "$HOME"
    $ cd dev/linux
    # make -j8 -C tools/bpf/bpftool install doc-install
    $ cd

# Simple Introspection

- Build the program, without BTF information.
- Dump the bytecode with `llvm-objdump`, observe the references to the maps.

    $ cd echo

    $ clang -O2 -target bpf -c counter.c -o counter_nobtf.o
    $ llvm-objdump -d counter_nobtf.o

- Load and pin the program
- List programs, also print bpffs pinned paths
- Dump translated and jited instructions
- Observe the difference for translated instructions: map addresses

    # bpftool prog load counter_nobtf.o /sys/fs/bpf/counter_nobtf type xdp
    # bpftool prog show --bpffs
    # bpftool prog dump xlated pinned /sys/fs/bpf/counter_nobtf
    # bpftool prog dump jited pinned /sys/fs/bpf/counter_nobtf

- Show and dump the map
- Mention lookup, update, delete for maps

    # bpftool map show
    # bpftool map dump name counter_map

# Attach to XDP

- Attach the XDP program to the network interface
- List network programs
- Use JSON to get the ids of programs loaded on the interface
- Validate with `ip link`
- Observe that the value in the map is incremented
- Detach XDP program

    # bpftool net attach xdp pinned /sys/fs/bpf/counter_nobtf dev enp3s0
    # bpftool net show
    # bpftool -p net show
    # bpftool -j net show | jq '.[].xdp[] | select(.devname == "enp3s0") | .id // .multi_attachments[].id'
    # ip link show dev enp3s0
    # bpftool map dump name counter_map
    # bpftool net detach xdp dev enp3s0

- Run `/usr/share/bcc/tools/execsnoop` in a different terminal
- List the loaded tracing programs

    # bpftool perf show

# Again, with BTF

- Start all over again, but with BTF information. Note the flag (and macro)
- Load, with libbpf debug information
- Dump both programs (same name), observe C insructions for the new one

    $ clang -O2 -target bpf -c counter.c -o counter_btf.o -g -D BTF
    # bpftool prog load counter_btf.o /sys/fs/bpf/counter_btf type xdp --debug
    # bpftool prog dump xlated name counter

- Attach XDP program to the interface
- Get map id and dump map, observe the number is incremented _and_ we get the
  structure of the map

    # bpftool net attach xdp pinned /sys/fs/bpf/counter_btf dev enp3s0
    # id=$(bpftool map -j | jq -r '.[] | select(.name == "counter_map") | select(.btf_id) | .id')
    # bpftool map dump id "$id"
    # bpftool net detach xdp dev enp3s0

- Get BTF id for the map
- Show BTF object for the map
- List BTF object, observe BTF info for kernel (`vmlinux`) and modules

    # btf_id=$(bpftool map -j | jq -r '.[] | select(.name == "counter_map") | select(.btf_id) | .btf_id')
    # bpftool btf dump id "$btf_id"
    # bpftool btf show

- Remove old program without BTF information

    # rm /sys/fs/bpf/counter_nobtf

# More Complex Example

- Before we use a LRU hash map, check that the system supports it

    $ cd app
    # bpftool feature probe

- Enable statistics gathering, for later

    # sysctl -w kernel.bpf_stats_enabled=1

- Compile all portions of the sample application
- Assemble object files into a single object file for the application
- Load all programs contained in this single object file (three programs, one
  doing tail calls), reuse previous map for packet counter, pin new maps
- Look at the new pinned paths produced

    $ for i in *.c ; do clang -O2 -g -target bpf -c "$i" -o "${i%.c}.o"; done
    # bpftool gen object app.o *.o
    # bpftool prog loadall app.o /sys/fs/bpf/app type xdp map name counter_map name counter_map pinmaps /sys/fs/bpf/app
    $ ls -l /sys/fs/bpf/app

- Make sure we have some input data for test runs:
  `python3 -c 'print("\0"*16+"> Hello eCHO! <3", end="")'>/tmp/data`
- In a separate terminal, run `bpftool prog tracelog`
- Do a test run (3 times) with no program in tail call map, observe logs
- Dump tail call map
- Update tail call map with counter program
- Redo test run, observe logs
- Dump counter map, observe numbers being incremented
- Update tail call map agains with streak program
- Redo test run (different number of runs)
- Observe streak map

    # bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 3
    # bpftool map dump name call_table
    # bpftool map update name call_table key 0 0 0 0 value pinned /sys/fs/bpf/app/xdp_counter
    # bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 3
    # bpftool map dump name counter_map
    # bpftool map update name call_table key 0 0 0 0 value pinned /sys/fs/bpf/app/xdp_streak
    # bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 5
    # bpftool map dump name streak_map

- Show programs to observe statistics

    # bpftool prog show

- In a separate terminal, start profiling:
  `bpftool prog profile pinned /sys/fs/bpf/app/xdp_entrypoint cycles instructions l1d_loads`
- Do a last test run

    # bpftool prog run pinned /sys/fs/bpf/app/xdp_entrypoint data_in /tmp/data data_out - repeat 10

- Dump control flow graph for the program

    # bpftool prog dump xlated name streak visual | tee /dev/tty | dot -Tpng > cfg.png
    $ xview cfg.png
