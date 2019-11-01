Clear-Host;

$exec=$args[0]
$ErrorActionPreference = "Stop"

Write-Host "Building..."

rm ./output/*.img
rm ./output/*.lss
rm ./output/*.dump

& "C:\Users\jango\Documents\BCIT\Operating Systems\os_labs\gcc-arm-8.3-2019.03-i686-mingw32-aarch64-elf\bin\aarch64-elf-gcc.exe" `
	-mcpu=cortex-a53 `
    -O0 `
    -ffreestanding `
    -nostdlib `
	-T linker.ld  `
	src/boot.s `
    src/main.c `
	src/system.c `
	src/hal/fonts.c `
	src/hal/hal_cpu.c `
	src/hal/hal_io.c `
	src/hal/hal_video.c `
	src/drivers/delays/delays.c `
	src/drivers/fb/fb.c `
	src/drivers/mbox/mbox.c `
	src/drivers/uart/uart.c `
    -o output/kernel8.img

	& "C:\Users\jango\Documents\BCIT\Operating Systems\os_labs\gcc-arm-8.3-2019.03-i686-mingw32-aarch64-elf\bin\aarch64-elf-objdump" -D .\output\kernel8.img | Out-File -filepath output/kernel8.lss -Encoding ASCII
    & "C:\Users\jango\Documents\BCIT\Operating Systems\os_labs\gcc-arm-8.3-2019.03-i686-mingw32-aarch64-elf\bin\aarch64-elf-objdump" -s .\output\kernel8.img | Out-File -filepath output/kernel8.dump -Encoding ASCII


& 'C:\Program Files\qemu\qemu-system-aarch64.exe' -M raspi3 -kernel output/kernel8.img -serial stdio -serial null
