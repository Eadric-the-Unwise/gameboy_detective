; .include    "global.s"

    .globl  .set_xy_col_btt

    ;; BANKED:  checked
    .area   _BASE

_set_bkg_tiles_col::
    PUSH    BC

    LDA HL,4(SP)    ; Skip return address and registers
    LD  A,(HL+)     ; D = x
    LD  D, A
    LD  E,(HL)      ; E = y
    LDA HL,9(SP)
    LD  A,(HL-)     ; BC = tiles pointer
    LD  B, A
    LD  A,(HL-)
    LD  C, A
    LD  A,(HL-)     ; A = map_width
    LD  H,(HL)      ; H = col_height m
    LD  L,A     ; L = map_width

    CALL    .set_xy_col_btt

    POP BC
    RET
