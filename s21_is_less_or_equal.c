#include "s21_decimal.h"

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int flag_result = 0;  // FALSE = 0, TRUE = 1.
  s21_decimal tmp_first_num = {{value_1.bits[LOW], value_1.bits[MID],
                                value_1.bits[HIGH], value_1.bits[SCALE]}};
  s21_decimal tmp_second_num = {{value_2.bits[LOW], value_2.bits[MID],
                                 value_2.bits[HIGH], value_2.bits[SCALE]}};
  int type_first_num = value_type(
      tmp_first_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_second_num = value_type(
      tmp_second_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN

  if (type_first_num != 0 ||
      type_second_num !=
          0) {  // если первое или второе число INF, N_INF или NAN
    if ((type_first_num == 0 && type_second_num == 1) ||
        (type_first_num == 1 && type_second_num == 1) ||
        (type_first_num == 2 && type_second_num == 0) ||
        (type_first_num == 2 && type_second_num == 1) ||
        (type_first_num == 2 && type_second_num == 2)) {
      flag_result = 1;  // TRUE = 1
    } else {
      flag_result = 0;  // FALSE = 0
    }
  } else {  // иначе, если оба числа простые
    if (s21_is_greater(value_1, value_2) == 0) {
      flag_result = 1;
    }
  }
  return flag_result;
}
