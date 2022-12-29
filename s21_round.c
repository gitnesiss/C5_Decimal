#include "s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  int flag_result = 0;
  int scale = get_scale(value);
  int sign = get_sign(value);
  if (scale > 0) {
    s21_big_decimal tmp_num_one = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}};  // число один
    s21_big_decimal tmp_num_five = {
        {5, 0, 0, 0, 0, 0, 0, 0, 0}};  // граничное число
    s21_big_decimal tmp_num_ten = {
        {10, 0, 0, 0, 0, 0, 0, 0, 0, 0}};  // число десять
    s21_big_decimal temp_value = {0};
    s21_big_decimal temp_result = {0};
    s21_big_decimal remaind = {0};
    s21_big_decimal temp_value_int = {0};
    copy_from_decimal_to_big_decimal(value, &temp_value);
    big_set_scale(&temp_value, 0);
    copy_from_big_decimal_to_big_decimal(temp_value, &temp_value_int);
    while (scale) {
      big_decimal_reset(&remaind);
      big_division_action(temp_value_int, tmp_num_ten, &remaind,
                          1);  // выделяем остаток
      big_division_action(temp_value_int, tmp_num_ten, &temp_result,
                          0);  // выделяем целую часть
      copy_from_big_decimal_to_big_decimal(temp_result, &temp_value_int);
      scale--;  // уменьшили степень
      if (big_is_greater_or_equal(remaind, tmp_num_five)) {
        big_addition_action(temp_value_int, tmp_num_one, &temp_result);
        copy_from_big_decimal_to_big_decimal(temp_result, &temp_value_int);
      }
    }
    copy_from_big_decimal_to_decimal(temp_value_int, result);
    if (num_is_zero(*result) == 1) {
      set_sign(result, 0);
    } else {
      set_sign(result, sign);
    }
  } else {
    copy_from_decimal_to_decimal(value, result);
    if (num_is_zero(*result) == 1) {
      set_sign(result, 0);
    }
  }
  return flag_result;
}
