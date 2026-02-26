#!/bin/bash

NAME="game"

EFI_INC="/usr/include/efi"
EFI_LIB="/usr/lib"

gcc -I"$EFI_INC" -I"$EFI_INC/x86_64" -I"$EFI_INC/protocol" \
    -fpic -ffreestanding -fno-stack-protector -fno-stack-check \
    -fshort-wchar -mno-red-zone -maccumulate-outgoing-args \
    -c "$NAME.c" -o "$NAME.o"

ld -shared -Bsymbolic \
   -L"$EFI_LIB" \
   -T "$EFI_LIB/elf_x86_64_efi.lds" \
   "$EFI_LIB/crt0-efi-x86_64.o" "$NAME.o" \
   -o "$NAME.so" -lgnuefi -lefi

objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym \
        -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
        --target efi-app-x86_64 --subsystem=10 "$NAME.so" "$NAME.efi"


mkdir -p EFI/BOOT

cp "$NAME.efi" EFI/BOOT/BOOTX64.EFI

echo "Gotowe! Struktura EFI/BOOT/BOOTX64.EFI przygotowana."
