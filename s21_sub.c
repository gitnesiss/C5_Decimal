#include "s21_decimal.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_result = 0;
  int sign_1_num = get_sign(value_1);
  int sign_2_num = get_sign(value_2);
  int sign_result_num = 0;
  int scale_result_num = 0;
  int type_1_num =
      value_type(value_1);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_2_num =
      value_type(value_2);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  if (type_1_num != 0 ||
      type_2_num != 0) {  // если первое или второе число INF, N_INF или NAN
    if ((type_1_num == 0 && type_2_num == 2) ||
        (type_1_num == 1 && type_2_num == 0) ||
        (type_1_num == 1 && type_2_num == 2)) {
      set_number_is_infinity(result, 0);
    } else if ((type_1_num == 0 && type_2_num == 1) ||
               (type_1_num == 2 && type_2_num == 0) ||
               (type_1_num == 2 && type_2_num == 1)) {
      set_number_is_infinity(result, 1);
    } else {
      set_number_is_not_a_number(result, 1);
    }
  } else {  // иначе, если оба числа простые
    if ((num_is_zero(value_1) == 1) ||
        (num_is_zero(value_2) == 1)) {    // если одно из чисел
      if (num_is_zero(value_1) == 1) {    // если первое число 0
        if (num_is_zero(value_2) == 1) {  // и второе число 0
          decimal_reset(
              result);  // записываем в результат 0 (обнуляем структуру result)
        } else {        // и второе число не ноль
          s21_negate(value_2, result);  // меняем знак второго числа и
                                        // записываем его в результат
        }
      } else /* if (num_is_zero(value_2) == 1) */ {  // иначе, если второе число
                                                     // 0
        copy_from_decimal_to_decimal(
            value_1, result);  // копируем первое число в результат
      }
    } else if (s21_is_equal(value_1, value_2) == 1) {
      decimal_reset(result);
    } else {
      // если знаки равны
      if (sign_1_num == sign_2_num) {
        s21_decimal tmp_num_1 = {0};
        s21_decimal tmp_num_2 = {0};
        // правильное выставление числа, выставлем числа так чтоб первым стояло
        // число с большим значением по мантисе
        if (sign_1_num == 0) {  // если знаки обоих чисел "+"
          if (s21_is_greater_or_equal(value_1, value_2) ==
              1) {  // если первое число больше второго
            copy_from_decimal_to_decimal(value_1, &tmp_num_1);
            copy_from_decimal_to_decimal(value_2, &tmp_num_2);
            sign_result_num = 0;
          } else {
            copy_from_decimal_to_decimal(value_1, &tmp_num_2);
            copy_from_decimal_to_decimal(value_2, &tmp_num_1);
            sign_result_num = 1;
          }
        } else {  // иначе знаки обоих чисел "-"
                  // if (sign_1_num == 1) {
          set_sign_plus(&value_1);
          set_sign_plus(&value_2);
          // }
          if (s21_is_greater_or_equal(value_1, value_2) ==
              1) {  // если первое число больше второго
            copy_from_decimal_to_decimal(value_1, &tmp_num_1);
            copy_from_decimal_to_decimal(value_2, &tmp_num_2);
            sign_result_num = 1;
          } else {
            copy_from_decimal_to_decimal(value_1, &tmp_num_2);
            copy_from_decimal_to_decimal(value_2, &tmp_num_1);
            sign_result_num = 0;
          }
        }
        // переводим числа в тип данных s21_big_decimal
        s21_big_decimal big_num_1 = {0};
        s21_big_decimal big_num_2 = {0};
        s21_big_decimal big_tmp_result = {0};
        copy_from_decimal_to_big_decimal(tmp_num_1, &big_num_1);
        copy_from_decimal_to_big_decimal(tmp_num_2, &big_num_2);
        norm_big_decimal(&big_num_1, &big_num_2, 2);
        scale_result_num = big_get_scale(big_num_1);
        big_subtraction_action(big_num_1, big_num_2, &big_tmp_result);
        big_set_sign(&big_tmp_result, sign_result_num);
        big_set_scale(&big_tmp_result, scale_result_num);
        flag_result = convert_big_decimal_to_decimal(big_tmp_result, result);
      } else {
        if (sign_1_num == 0) {  // если знак певого числа положительный "+",
                                // значит второй знак "-"
          set_sign_plus(&value_1);
          set_sign_plus(&value_2);
          flag_result = s21_add(value_1, value_2, result);
        } else {  // если знак первого числа отрицательный (sign_1_num == 1) =>
                  // (sign_2_num == 0)
          change_sign(&value_2);
          flag_result = s21_add(value_1, value_2, result);
        }
      }
    }
  }
  return flag_result;
}
