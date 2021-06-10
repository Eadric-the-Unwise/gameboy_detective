;    .include    "global.s"
;
    .LCDC       = 0x40  ; LCD control
    .STAT       = 0x41  ; LCD status
; from globals.s
.macro WAIT_STAT ?lbl
lbl:    LDH A, (.STAT)
    AND #0x02       ; Check if in LCD modes 0 or 1
    JR  NZ, lbl
.endm

    .area   _BASE

;void set_bkg_tiles_col(UINT8 screen_x, UINT8 screen_y, UINT8 col_rows, UINT8 src_map_width, const UINT8 * p_src_map) 


    ;; Set window tile table from BC at XY = DE of size WH = HL
.set_xy_col_wtt::
    PUSH    HL      ; Store col_height, map_width
    LDH A,(.LCDC)
    BIT 6,A
    JR  Z,.is98
    JR  .is9c
    ;; Set background tile table from (BC) at XY = DE of size WH = HL
.set_xy_col_btt::
    PUSH    HL      ; Store WH
    LDH A,(.LCDC)
    BIT 3,A
    JR  NZ,.is9c
.is98:
    LD  HL,#0x9800
    JR  .set_xy_col_tt
.is9c:
    LD  HL,#0x9C00
    ;; Set background tile from (BC) at XY = DE, size col_height & map_width on stack, to vram address (HL)
.set_xy_col_tt::
    PUSH    BC      ; Store source

    SWAP    E       ; DE = Y,X
    RLC E           ; Y << 1
    LD  A,E         ; A = X
    AND #0x03       ; A &= 0x03 (mask to only lower bits)
    ADD H           
    LD  B,A
    LD  A,#0xE0
    AND E
    ADD D
    LD  C,A     ; dest BC = HL + 0x20 * Y + X

    POP HL      ; HL = source
    POP DE      ; DE = (D)col_height, (E)map_width on stack

; map column VRAM Addr start (BC), col_height & src_map_width (DE), map source (HL)
3$:
    ; Copy n(col_height) tiles into VRAM bkg tile map
    WAIT_STAT
    LD  A, (HL)    
    LD  (BC), A      ; Copy tile id into VRAM map

    DEC D            ; Decrement col_height, check for exit
    RET Z

    PUSH    DE       ; save updated col_height and src_map_width

    LD  D, #0
    ADD HL, DE       ; source += map_width (move to next tile)

    ; map in VRAM: move to next row, handle bottom-to-top wrap around
    LD  A, B
    AND #0xFC
    LD  E, A         ; save VRAM map bkg/win selector bits

    LD  A,#0x20      ; VRAM map is 32 tiles wide

    ADD C
    LD  C, A
    ADC B
    SUB C
    AND #0x03        ; mask to only VRAM map upper bits
    OR  E            ; restore saved bkg/win selector bits
    LD  B, A

    POP    DE
    
    JR  3$
