/* Routine optimized for bit-shuffling a buffer for a type size of 1 byte. */
static void
bitshuffle1_neon(const uint8_t* const src, uint8_t* dest, const size_t nbyte) {

  const size_t elem_size = 1;
  uint16x8_t x0;
  size_t i, j, k;
  uint8x8_t lo_x, hi_x, lo, hi;

  const int8_t __attribute__ ((aligned (16))) xr[8] = {0,1,2,3,4,5,6,7};
  uint8x8_t mask_and = vdup_n_u8(0x01);
  int8x8_t mask_shift = vld1_s8(xr);

  /* #define CHECK_MULT_EIGHT(n) if (n % 8) exit(0); */
  CHECK_MULT_EIGHT(nbyte);

  for (i = 0, k = 0; i < nbyte; i += 16, k++) {
    /* Load 16-byte groups */
    x0 = vld1q_u8(src + k*16);
    /* Split in 8-bytes grops */
    lo_x = vget_low_u8(x0);
    hi_x = vget_high_u8(x0);
    for (j = 0; j < 8; j++) {
      /* Create mask from the most significant bit of each 8-bit element */
      lo = vand_u8(lo_x, mask_and);
      lo = vshl_u8(lo, mask_shift);
      hi = vand_u8(hi_x, mask_and);
      hi = vshl_u8(hi, mask_shift);
      lo = vpadd_u8(lo,lo);
      lo = vpadd_u8(lo,lo);
      lo = vpadd_u8(lo,lo);
      hi = vpadd_u8(hi,hi);
      hi = vpadd_u8(hi,hi);
      hi = vpadd_u8(hi,hi);
      /* Shift packed 8-bit */
      lo_x = vshr_n_u8(lo_x, 1);
      hi_x = vshr_n_u8(hi_x, 1);
      /* Store the created mask to the destination vector */
      vst1_lane_u8(dest + 2*k + j*nbyte/(8*elem_size), lo, 0);
      vst1_lane_u8(dest + 2*k+1 + j*nbyte/(8*elem_size), hi, 0);
    }
  }
}

