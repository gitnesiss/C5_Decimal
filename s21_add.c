#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_result = 0;
  s21_decimal tmp_1_num = {
      {value_1.bits[0], value_1.bits[1], value_1.bits[2], value_1.bits[3]}};
  s21_decimal tmp_2_num = {
      {value_2.bits[0], value_2.bits[1], value_2.bits[2], value_2.bits[3]}};
  int sign_first_num = get_sign(tmp_1_num);  // знак первого числа
  int sign_second_num = get_sign(tmp_2_num);  // знак второго числа
  int type_first_num = value_type(tmp_1_num);
  int type_second_num = value_type(tmp_2_num);
  if (type_first_num != 0 || type_second_num != 0) {
    if ((type_first_num == 0 && type_second_num == 1) ||
        (type_first_num == 1 && type_second_num == 0) ||
        (type_first_num == 1 && type_second_num == 1)) {
      set_number_is_infinity(result, 0);
      flag_result = 1;
    } else if ((type_first_num == 0 && type_second_num == 2) ||
               (type_first_num == 2 && type_second_num == 0) ||
               (type_first_num == 2 && type_second_num == 2)) {
      set_number_is_infinity(result, 1);
      flag_result = 2;
    } else if (((type_first_num == 1 && type_second_num == 2) ||
                (type_first_num == 2 && type_second_num == 1)) ||
               type_first_num == 3 || type_second_num == 3) {
      set_number_is_not_a_number(result, 1);
      flag_result = 3;
    }
  } else {  // иначе, если оба числа простые
    // проверка на то, что числа противоположные по знаку, но равные по значению
    if (opposite_num(tmp_1_num, tmp_2_num) == 1) {
      decimal_reset(result);
    } else {
      // проверка на то, что одно из слагаемых равно 0
      // если одно из слагаемых равно 0
      if ((num_is_zero(tmp_1_num) == 1) || (num_is_zero(tmp_2_num) == 1)) {
        if (num_is_zero(tmp_1_num) == 1) {  // если первое число 0
          copy_from_decimal_to_decimal(tmp_2_num, result);
        } else if (num_is_zero(tmp_2_num) == 1) {
          copy_from_decimal_to_decimal(tmp_1_num, result);
        }
      } else {  // иначе, если ни одно из слагаемых не равно 0
        // если знаки чисел равны
        if (sign_first_num == sign_second_num) {
          s21_big_decimal term_1 = {0};
          s21_big_decimal term_2 = {0};
          s21_big_decimal result_summ = {0};
          norm_from_decimal_to_big_decimal(tmp_1_num, tmp_2_num, &term_1,
                                           &term_2, 1);
          big_addition_action(term_1, term_2, &result_summ);
          big_set_sign(&result_summ, sign_first_num);
          flag_result = convert_big_decimal_to_decimal(result_summ, result);
        } else {  // если знаки чисел разные (sign_first_num != sign_second_num)
          if (sign_first_num == 1) {  // если знак певого числа отрицательный
            change_sign(&tmp_1_num);
            if (s21_is_greater(tmp_1_num, tmp_2_num) == 1) {
              flag_result = s21_sub(tmp_1_num, tmp_2_num, result);
              change_sign(result);
            } else {
              flag_result = s21_sub(tmp_2_num, tmp_1_num, result);
            }
          } else {  // если знак первого числа положительный (sign_first_num ==)
            change_sign(&tmp_2_num);
            if (s21_is_greater(tmp_1_num, tmp_2_num) == 1) {
              flag_result = s21_sub(tmp_1_num, tmp_2_num, result);
            } else {
              flag_result = s21_sub(tmp_2_num, tmp_1_num, result);
              change_sign(result);
            }
          }
        }
      }
    }
  }
  return flag_result;
}
