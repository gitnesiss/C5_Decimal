#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int flag_result = 1;  // FALSE = 0, TRUE = 1.
  s21_decimal tmp_first_num = {{value_1.bits[LOW], value_1.bits[MID],
                                value_1.bits[HIGH], value_1.bits[SCALE]}};
  s21_decimal tmp_second_num = {{value_2.bits[LOW], value_2.bits[MID],
                                 value_2.bits[HIGH], value_2.bits[SCALE]}};
  int sign_first_num = get_sign(value_1);
  int sign_second_num = get_sign(value_2);
  int scale_first_num = get_scale(value_1);
  int scale_second_num = get_scale(value_2);
  int type_first_num = value_type(
      tmp_first_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_second_num = value_type(
      tmp_second_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  if (type_first_num != 0 ||
      type_second_num !=
          0) {  // если первое или второе число INF, N_INF или NAN
    if ((type_first_num == 1 && type_second_num == 1) ||
        (type_first_num == 2 && type_second_num == 2)) {
      flag_result = 1;  // TRUE = 1
    } else {
      flag_result = 0;  // FALSE = 0
    }
  } else {  // иначе, если оба числа простые
    if (num_is_zero(value_1) == 1 && num_is_zero(value_2) == 1) {
      flag_result = 1;
    } else {
      if (sign_first_num !=
          sign_second_num) {  // Знак первого числа "+", а второго "-".
                              // Значение, указывающее на знак первого числа (0
                              // = "+") меньше знака второго (1 = "-").
        flag_result = 0;  // выставляем флаг FALSE = 0
      } else {  // Знаки у обоих чисел либо "-", либо "+" - (sign_first_num ==
                // sign_second_num)
        if (scale_first_num ==
            scale_second_num) {  // если нормализация прошла успешно
                                 // (scale_first_num == scale_second_num и не
                                 // было переполнения в одном из чесел),
                                 // выполняем:
          for (int i = 3; i >= 0 && flag_result == 1;
               i--) {  // проходимся по каждому из элементов
            if (value_1.bits[i] !=
                value_2.bits[i]) {  // если хоть один из value_1.bits[i] не
                                    // равен value_2.bits[i]
              flag_result = 0;  // устанавливаем флаг FALSE = 0
            }
          }
        } else {
          s21_big_decimal big_num_1 = {0};
          s21_big_decimal big_num_2 = {0};
          norm_from_decimal_to_big_decimal(value_1, value_2, &big_num_1,
                                           &big_num_2, 0);
          for (int i = (B_D_NOT_SCALE / 32 - 1); i >= 0 && flag_result == 1;
               i--) {
            if (big_num_1.bits[i] !=
                big_num_2.bits[i]) {  // если значение bits[i] первого числа
                                      // больше значения bits[i] второго
              flag_result = 0;  // выставляем флаг TRUE = 1
            }
          }
        }
      }
    }
  }
  return flag_result;
}
