#ifndef __BE_OPCODE_H
#define __BE_OPCODE_H

#define NO_JUMP                 -1

/* define bits */
#define IOP_BITS                6
#define IRA_BITS                8
#define IRKB_BITS               9
#define IRKC_BITS               9

#define IRKC_POS                0
#define IRKB_POS                (IRKC_POS + IRKC_BITS)
#define IRA_POS                 (IRKB_POS + IRKB_BITS)
#define IOP_POS                 (IRA_POS + IRA_BITS)
#define IAx_BITS                (IRA_BITS + IRKB_BITS + IRKC_BITS)
#define IBx_BITS                (IRKC_BITS + IRKB_BITS)

#define INS_MASK(pos, bits)     (((1 << (bits)) - 1) << pos)
#define INS_GETx(i, mask, pos)  (((binstruction)(i) & (mask)) >> (pos))
#define INS_SETx(v, mask, pos)  (((binstruction)(v) << (pos)) & (mask))

#define isK(v)                  (((v) & (1 << (IRKB_BITS - 1))) != 0)
#define setK(v)                 ((v) | (1 << (IRKB_BITS - 1)))
#define KR2idx(v)               ((v) & 0xFF)
#define isKB(v)                 (((v) & (1 << (IRA_POS - 1))) != 0)
#define isKC(v)                 (((v) & (1 << (IRKB_POS - 1))) != 0)

#define IOP_MASK                INS_MASK(IOP_POS, IOP_BITS)
#define IRA_MASK                INS_MASK(IRA_POS, IRA_BITS)
#define IRKB_MASK               INS_MASK(IRKB_POS, IRKB_BITS)
#define IRKC_MASK               INS_MASK(IRKC_POS, IRKC_BITS)
#define IAx_MASK                INS_MASK(0, IAx_BITS)
#define IBx_MASK                INS_MASK(0, IBx_BITS)
#define IsBx_MAX                (IBx_MASK >> 1)
#define IsBx_MIN                (-IsBx_MAX - 1)

#define IGET_OP(i)              INS_GETx(i, IOP_MASK, IOP_POS)
#define IGET_RA(i)              INS_GETx(i, IRA_MASK, IRA_POS)
#define IGET_RKB(i)             INS_GETx(i, IRKB_MASK, IRKB_POS)
#define IGET_RKC(i)             INS_GETx(i, IRKC_MASK, IRKC_POS)
#define IGET_Bx(i)              INS_GETx(i, IBx_MASK, 0)
#define IGET_sBx(i)             ((int)IGET_Bx(i) - IsBx_MAX)

#define ISET_OP(i)              INS_SETx(i, IOP_MASK, IOP_POS)
#define ISET_RA(i)              INS_SETx(i, IRA_MASK, IRA_POS)
#define ISET_RKB(i)             INS_SETx(i, IRKB_MASK, IRKB_POS)
#define ISET_RKC(i)             INS_SETx(i, IRKC_MASK, IRKC_POS)
#define ISET_Bx(i)              INS_SETx(i, IBx_MASK, 0)
#define ISET_sBx(i)             (ISET_Bx((int)(i) + IsBx_MAX))

typedef enum {
    /* don't change order */
/*  opcode           parameters         description */
    OP_ADD,       /*  A, B, C  |   R(A) <- RK(B) + RK(C) */
    OP_SUB,       /*  A, B, C  |   R(A) <- RK(B) - RK(C) */
    OP_MUL,       /*  A, B, C  |   R(A) <- RK(B) * RK(C) */
    OP_DIV,       /*  A, B, C  |   R(A) <- RK(B) / RK(C) */
    OP_MOD,       /*  A, B, C  |   R(A) <- RK(B) % RK(C) */
    OP_LT,        /*  A, B, C  |   R(A) <- RK(B) < RK(C) */
    OP_LE,        /*  A, B, C  |   R(A) <- RK(B) <= RK(C) */
    OP_EQ,        /*  A, B, C  |   R(A) <- RK(B) == RK(C) */
    OP_NE,        /*  A, B, C  |   R(A) <- RK(B) != RK(C) */
    OP_GT,        /*  A, B, C  |   R(A) <- RK(B) > RK(C) */
    OP_GE,        /*  A, B, C  |   R(A) <- RK(B) >= RK(C) */
    OP_RANGE,     /*  A, B, C  |   R(A) <- range(RK(B), RK(C)) */
    OP_NEG,       /*  A, B     |   R(A) <- -RK(B) */
    OP_LDNIL,     /*  A        |   R(A) <- nil */
    OP_LDBOOL,    /*  A, B, C  |   R(A) <- cast_bool(B), if(C): pc++ */
    OP_LDINT,     /*  A, sBx   |   R(A) <- sBx */
    OP_LDCONST,   /*  A, Bx    |   R(A) <- K(Bx) */
    OP_MOVE,      /*  A, B, C  |   R(A) <- RK(B) */
    OP_GETGBL,    /*  A, Bx    |   R(A) <- GLOBAL(Bx) */
    OP_SETGBL,    /*  A, Bx    |   R(A) -> GLOBAL(Bx) */
    OP_GETUPV,    /*  A, Bx    |   R(A) <- UPVALUE(Bx)*/
    OP_SETUPV,    /*  A, Bx    |   R(A) -> UPVALUE(Bx)*/
    OP_JMP,       /*  sBx      |   pc <- pc + sBx */
    OP_JMPT,      /*  A, sBx   |   if(R(A)): pc <- pc + sBx  */
    OP_JMPF,      /*  A, sBx   |   if(not R(A)): pc <- pc + sBx  */
    OP_CALL,      /*  A        |   CALL(R(A), B) */
    OP_RET,       /*  A, B     |   if (R(A)) R(-1) <- RK(B) */
    OP_CLOSURE,   /*  A, Bx    |   R(A) <- CLOSURE(proto_table[Bx])*/
    OP_GETMBR,    /*  A, B, C  |   R(A) <- RK(B).RK(C) */
    OP_GETMET,    /*  A, B, C  |   R(A) <- RK(B).RK(C) */
    OP_SETMBR,    /*  A, B, C  |   R(A).RK(B) <- RK(C) */
    OP_GETIDX,    /*  A, B, C  |   R(A) <- RK(B)[RK(C)] */
    OP_SETIDX,    /*  A, B, C  |   R(A)[RK(B)] <- RK(C) */
    OP_SETSUPER,  /*  A, B     |   class:R(A) set super with class:RK(B) */
    OP_CLOSE      /*  A        |   close upvalues */
} bopcode;

const char *be_opcode2str(bopcode op);

#endif
