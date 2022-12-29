#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  int flag_result = 0;
  copy_from_decimal_to_decimal(value, result);
  change_sign(result);
  return flag_result;
}
