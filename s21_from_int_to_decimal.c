#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int flag_result = 0;
  if (src < 0) {
    set_sign_minus(dst);
    dst->bits[0] = ~(src - 1);  // инвертируем число в положительное
    for (int i = 1; i < 3;
         i++) {  // Элементы dst->bits[1] и dst->bits[2] массива обнуляем.
      dst->bits[i] = 0;
    }
  } else {
    for (int i = 1; i < 4; i++) {  // Элементы dst->bits[1], dst->bits[2],
                                   // dst->bits[3] массива обнуляем.
      dst->bits[i] = 0;
    }
    dst->bits[0] = src;
  }
  return flag_result;
}
