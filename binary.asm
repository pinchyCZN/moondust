;nasmw -t -f  win32 -o$(IntDir)\$(InputName).obj -Xvc $(InputName).asm

SECTION .data


global _line8x8
global _line8x8end
global ?line8x8@@3PADA

?line8x8@@3PADA:
_line8x8:
 incbin "charset.bin"
_line8x8end:

