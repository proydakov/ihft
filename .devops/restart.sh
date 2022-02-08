#!/bin/bash
#
# Based on Erik Rigtorp tuning guide
# https://rigtorp.se/low-latency-guide/
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
