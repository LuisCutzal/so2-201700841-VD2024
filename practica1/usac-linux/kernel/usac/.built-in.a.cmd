savedcmd_kernel/usac/built-in.a := rm -f kernel/usac/built-in.a;  printf "kernel/usac/%s " syscall1.o syscall2.o | xargs ar cDPrST kernel/usac/built-in.a
