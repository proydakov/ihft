#!/bin/bash
#
# Based on Erik Rigtorp tuning guide
# https://rigtorp.se/low-latency-guide/
#
# Kernel cmdline example
# $ cat /proc/cmdline
# BOOT_IMAGE=/boot/vmlinuz-5.13.0-28-generic root=UUID=76a2a6bf-b3b0-6fb6-9ab7-25cacb4c5f34 ro default_hugepagesz=1G isolcpus=6-11 nohz_full=6-11 rcu_nocbs=6-11 quiet splash vt.handoff=7
#
# Reserve several hugepages
# $ echo 8 > /proc/sys/vm/nr_hugepages
#

if grep -q "isolcpus" /proc/cmdline; then
    echo isolcpus found
    CMDLINE=$(cat /proc/cmdline)
    for word in $CMDLINE
    do
        if [[ $word == *"isolcpus="* ]]; then
            cpus="$word"
            pattern=""
            cpus=${cpus/isolcpus=/$pattern}
            echo $cpus
            /usr/bin/tuna --cpus=$cpus --isolate
        fi
    done

    /usr/sbin/irqbalance --foreground --oneshot

    /usr/sbin/swapoff -a

    echo never > /sys/kernel/mm/transparent_hugepage/enabled
    echo 0 > /proc/sys/kernel/numa_balancing
    echo 0 > /sys/kernel/mm/ksm/run

    sysctl vm.stat_interval=60

    find /sys/devices/system/cpu -name scaling_governor -exec sh -c 'echo performance > {}' ';'
else
    echo isolcpus not found
fi
