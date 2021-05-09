#ifndef _av1_obu_define_h_
#define _av1_obu_define_h_

#define NUM_REF_FRAMES               8
#define REFS_PER_FRAME               7
#define SELECT_SCREEN_CONTENT_TOOLS  2
#define SELECT_INTEGER_MV            2

// type of ref[NUM_REF_FRAMES]
// maybe timeline:
// GOLDEN -> LAST3 -> LAST2 -> LAST -> INTRA -> BWDREF -> ALTREF2 -> ALTREF
#define REF_INTRA_FRAME              0
#define REF_LAST_FRAME               1
#define REF_LAST2_FRAME              2
#define REF_LAST3_FRAME              3
#define REF_GOLDEN_FRAME             4
#define REF_BWDREF_FRAME             5
#define REF_ALTREF2_FRAME            6
#define REF_ALTREF_FRAME             7

#endif
