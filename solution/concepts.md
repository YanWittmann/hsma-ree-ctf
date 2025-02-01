get an embedded file out of the executable again:

```
binwalk --dd='.*' crackme
file _crackme.extracted/*
_crackme.extracted/0:          ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=ca0f9b7f8c036304341e9cda497207c8c67f6020, for GNU/Linux 3.2.0, not stripped
_crackme.extracted/735B8:      PNG image data, 800 x 450, 8-bit/color RGB, non-interlaced
_crackme.extracted/7360D:      Microsoft color profile 2.1, type Lino, RGB/XYZ-mntr device, IEC/sRGB model by HP, 3144 bytes, 9-2-1998 6:49:00, relative colorimetric "sRGB IEC61966-2.1"
_crackme.extracted/7360D.zlib: zlib compressed data
_crackme.extracted/7407D:      MIPSEL-BE MIPS-II ECOFF executable stripped - version 0.0
_crackme.extracted/7407D.zlib: zlib compressed data
```

---

input: abcd

is translated into x,y,z rotation values using three methods:

compute_rotation_x(input)
compute_rotation_y(input)
compute_rotation_z(input)

each method uses a different algorithm to compute the rotation, each has to be reverse-engeneered to calculate the
correct input for the desired rotation.
possible algorithms:

- input length
- ascii sum
- conditional logic based on e.g. ascii range
- random if not constraint met (ends in "_5435"), else value

https://neal.fun/password-game/

when desired rotation is achieved, ascii sum needs to be calculated for all three rotations.

---

the players need to extract the png using a hint they get when entering the wrong password.
extracted png content gives a hint on the rotations they must enforce:

           (x,y,z),  (x,y,z)
              |         |
              v         v
           [#=42, .=52, ... ]
              |         |
              v         v

`repeat(bin[45234  *  59070], 4) XOR hex(CF146B53D41C5943CF165C50C0164061) = PW{OK_I_PULL_UP}`

`XOR( 50577B4F4B5F495F50554C4C5F55507D , 9f43101c9f43101c9f43101c9f43101c ) = CF146B53D41C5943CF165C50C0164061`

and this needs to be entered as a password into the program.
the program calculates a checksum based on the input and compares it with a static value.
if the values are equal, the flag is printed!

FLAG{CAPYBARA_BASED_ENCRYPTION}
