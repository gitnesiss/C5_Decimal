#include "s21_decimal.h"

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal tmp_1 = value_1;  // 1-ое входящее decimal число
  s21_decimal tmp_2 = value_2;  // 2-ое входящее decimal число
  s21_big_decimal tmp_1_num = {0};  // 1-ое входящее decimal число
  s21_big_decimal tmp_2_num = {0};  // 2-ое входящее decimal число
  s21_big_decimal tmp_num_result = {0};
  int flag_result = 0;
  int tmp_sign =
      get_sign(tmp_1);  // знак числа, который пойдёт в результируещее число
  int tmp_result_sign =
      tmp_sign;  // знак числа, который пойдёт в результируещее число
  int tmp_1_num_scale =
      get_scale(tmp_1);  // итоговое значение степени при расчётах
  int tmp_2_num_scale =
      get_scale(tmp_2);  // итоговое значение степени при расчётах
  int tmp_result_scale = 0;  // итоговое значение степени при расчётах
  int type_first_num =
      value_type(tmp_1);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_second_num =
      value_type(tmp_2);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  decimal_reset(result);
  // проверка на -INF, +INF, NAN, num/0
  if (type_second_num == 3 || num_is_zero(tmp_2) == 1 || type_first_num == 1 ||
      type_first_num == 2 || type_first_num == 3) {
    set_number_is_not_a_number(result, 1);
    flag_result = 3;
  } else if ((num_is_zero(tmp_1) == 1 &&
              (type_second_num == 0 && num_is_zero(tmp_2) == 0)) ||
             (num_is_zero(tmp_1) == 1 && (type_second_num == 1)) ||
             (num_is_zero(tmp_1) == 1 && (type_second_num == 2))) {
    decimal_reset(result);
    set_sign(result, tmp_result_sign);
    // иначе, если оба числа просто числа
  } else {                  // это обычные числа
    set_sign_plus(&tmp_1);  // устанавливаем знак второго числа "+"
    set_sign_plus(&tmp_2);  // устанавливаем знак второго числа "+"
    if (s21_is_equal(tmp_1, tmp_2) == 1) {  // если исходные значения равны
      decimal_reset(result);  // записываем 0
    } else if (s21_is_less(tmp_1, tmp_2) ==
               1) {  // если первое исходное число меньше второго исходного
      copy_from_decimal_to_decimal(
          value_1, result);  // копируем значение value_1 в результат
    } else {  // если первое число больше второго числа
      if (tmp_1_num_scale >= tmp_2_num_scale) {
        tmp_result_scale = tmp_1_num_scale;
      } else {
        tmp_result_scale = tmp_2_num_scale;
      }
      norm_from_decimal_to_big_decimal(tmp_1, tmp_2, &tmp_1_num, &tmp_2_num, 0);
      big_set_scale(&tmp_1_num, 0);  // устанавливаем скейл 0
      big_set_scale(&tmp_2_num, 0);  // устанавливаем скейл 0
      big_division_action(tmp_1_num, tmp_2_num, &tmp_num_result, 1);
      if (big_num_is_zero(tmp_num_result) == 0) {
        big_set_sign(&tmp_num_result, tmp_result_sign);
        big_set_scale(&tmp_num_result, tmp_result_scale);
      }
      convert_big_decimal_to_decimal(tmp_num_result, result);
    }
  }
  return flag_result;
}
