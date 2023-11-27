#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

struct {
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __type(key, __u32);
    __type(value, __u32);
    __uint(max_entries, 256);
} packet_count SEC(".maps");

// 程序的入口点
SEC("xdp")
int xdp_packet_counter(struct xdp_md *ctx) {
    __u32 key_in = 0;  // 键0代表入站数据包
    __u32  *value_in;

    value_in = bpf_map_lookup_elem(&packet_count, &key_in);

    if (value_in) (*value_in)++;

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
