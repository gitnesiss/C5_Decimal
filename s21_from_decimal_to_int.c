#include "s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int flag_result = 0;
  int sign = get_sign(src);
  int scale = get_scale(src);
  s21_decimal int_src;
  if (scale < 29) {
    s21_truncate(src, &int_src);
    if (sign == 0) {
      if (int_src.bits[0] <= 2147483647 && int_src.bits[1] == 0 &&
          int_src.bits[2] == 0) {
        *dst = int_src.bits[0];
      } else {
        flag_result = 1;
      }
    } else {
      if (int_src.bits[0] <= 2147483648 && int_src.bits[1] == 0 &&
          int_src.bits[2] == 0) {
        *dst = -int_src.bits[0];
      } else {
        flag_result = 1;
      }
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}
