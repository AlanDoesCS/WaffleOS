; just a config files for graphics modes etc.
[BITS 16]

flags db 0b00000000
; | BITS | PURPOSE                                 |
; | ---- | --------------------------------------- |
; | 0    | Edit bit: set if changes have been made |
; | 1    |                                         |
; | 2    |                                         |
; | 3    |                                         |
; | 4    |                                         |
; | 5    |                                         |
; | 6    |                                         |
; | 7    |                                         |

graphics_mode db 0
; 0 = text mode
; 1 = 320x200x256
; 2 = 640x480x16
; 3 = 800x600x16
; 4 = 1024x768x16
; 5 = 1280x1024x16
; >5 = unused
