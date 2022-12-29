#include "s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int flag_result = 0;
  int scale = get_scale(src);
  int sign = get_sign(src);
  s21_big_decimal tmp_num_one = {{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}};  // число один
  s21_big_decimal tmp_num_five = {
      {5, 0, 0, 0, 0, 0, 0, 0, 0}};  // граничное число
  s21_big_decimal tmp_num_ten = {
      {10, 0, 0, 0, 0, 0, 0, 0, 0, 0}};  // число десять
  s21_big_decimal remaind = {0};
  s21_big_decimal temp_src_int = {0};
  s21_big_decimal temp_result = {0};
  s21_decimal result = {0};
  int all_digits = 0;
  int int_dst = 0;
  if (scale < 29) {
    copy_from_decimal_to_big_decimal(
        src, &temp_src_int);  // копируем входящий децимал в биг децимал
    big_set_sign_plus(&temp_src_int);
    all_digits = big_digits_in_mantis(
        temp_src_int);  // узнаем сколько разрядов в десятичном числе
    int int_digits = all_digits - scale;
    int zero_digits = int_digits - 7;
    big_set_scale(&temp_src_int, 0);  // зануляем скейл для дальнейших операций
    while (all_digits >=
           10) {  //  цикл деления на десять и округления последнего разряда, 7
                  //  так как у флоата 7 значащих разрядов
      big_decimal_reset(&remaind);
      big_division_action(temp_src_int, tmp_num_ten, &remaind,
                          1);  // выделяем остаток
      big_division_action(temp_src_int, tmp_num_ten, &temp_result,
                          0);  // выделяем целую часть
      copy_from_big_decimal_to_big_decimal(temp_result, &temp_src_int);
      all_digits--;  // уменьшаем разряды
      if (big_is_greater_or_equal(remaind, tmp_num_five)) {
        big_addition_action(temp_src_int, tmp_num_one, &temp_result);
        copy_from_big_decimal_to_big_decimal(temp_result, &temp_src_int);
      }
    }
    copy_from_big_decimal_to_decimal(temp_src_int, &result);
    set_sign(&result, sign);
    s21_from_decimal_to_int(result, &int_dst);
    if (int_digits > 7 && all_digits >= 9) {
      *dst = int_dst * pow(10, zero_digits - 2);
    } else {
      *dst = int_dst * pow(10, -(all_digits - int_digits));
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}
