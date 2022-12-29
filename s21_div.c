#include "s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_result = 0;
  decimal_reset(result);
  s21_decimal tmp_1_num = {{value_1.bits[0], value_1.bits[1], value_1.bits[2],
                            value_1.bits[3]}};  // 1-ое входящее decimal число
  s21_decimal tmp_2_num = {{value_2.bits[0], value_2.bits[1], value_2.bits[2],
                            value_2.bits[3]}};  // 2-ое входящее decimal число
  s21_big_decimal big_num_dec_max = {{4294967295, 4294967295, 4294967295, 0, 0,
                                      0, 0, 0, 0,
                                      0x00000000}};  // максимальное число
                                                     // близкое к +INF
                                                     // типа big_decimal
  s21_big_decimal dividend = {0};  // dividend - делимое
  s21_big_decimal divisor = {0};  // divisor - делитель = вычитаемое
  s21_big_decimal tmp_1b_num_start = {
      0};  // dividend - делимое, 1-ое big decimal число, подготовленное число
           // для начала расчётов
  s21_big_decimal tmp_2b_num_start = {
      0};  // divisor - делитель = вычитаемое, 2-ое big decimal число,
           // подготовленное число для начала расчётов
  s21_big_decimal tmp_result_num = {
      0};  // quotient - частное, результат деления чисел типа big decimal
  s21_big_decimal minuend = {0};     // уменьшаемое
  s21_big_decimal subtrahend = {0};  // вычитаемое = делитель
  s21_big_decimal tmp_num_int_part = {0};  // целая часть числа
  int sign_num_result = 0;  // знак числа, который пойдёт в результируещее число
  int type_first_num = value_type(
      tmp_1_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_second_num = value_type(
      tmp_2_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int tmp_scale_dividend = 0;
  int tmp_scale_divisor = 0;
  int tmp_scale_result = 0;  // итоговое значение степени при расчётах
  sign_num_result = final_sign(value_1, value_2);
  // проверка на -INF, +INF, NAN, num/0
  if ((num_is_zero(tmp_1_num) == 1 && num_is_zero(tmp_2_num) == 1) ||
      (type_first_num == 3 && num_is_zero(tmp_2_num) == 1)) {
    set_number_is_not_a_number(result, 1);
    flag_result = 3;
  } else if (((type_first_num == 0 && get_sign(tmp_1_num) == 0 &&
               num_is_zero(tmp_1_num) == 0) &&
              (num_is_zero(tmp_2_num) == 1)) ||
             (type_first_num == 1 && num_is_zero(tmp_2_num) == 1)) {
    set_number_is_infinity(result, 0);
    flag_result = 3;
  } else if (((type_first_num == 0 && get_sign(tmp_1_num) == 1 &&
               num_is_zero(tmp_1_num) == 0) &&
              (num_is_zero(tmp_2_num) == 1)) ||
             (type_first_num == 2 && num_is_zero(tmp_2_num) == 1)) {
    set_number_is_infinity(result, 1);
    flag_result = 3;
  } else if (((type_first_num == 1) &&
              ((type_second_num == 0) && (get_sign(tmp_2_num) == 0))) ||
             ((type_first_num == 2) &&
              ((type_second_num == 0) && (get_sign(tmp_2_num) == 1) &&
               (num_is_zero(tmp_2_num) == 0)))) {
    set_number_is_infinity(result, 0);
    flag_result = 3;
  } else if (((type_first_num == 1) &&
              (type_second_num == 0 && get_sign(tmp_2_num) == 1)) ||
             ((type_first_num == 2) &&
              (type_second_num == 0 && get_sign(tmp_2_num) == 0))) {
    set_number_is_infinity(result, 1);
    flag_result = 2;
  } else if (type_first_num == 3 || type_second_num == 3 ||
             ((type_first_num == 1) && (type_second_num == 1)) ||
             ((type_first_num == 1) && (type_second_num == 2)) ||
             ((type_first_num == 2) && (type_second_num == 1)) ||
             ((type_first_num == 2) && (type_second_num == 2))) {
    set_number_is_not_a_number(result, 1);
    flag_result = 3;
  } else if ((num_is_zero(tmp_1_num) == 1 &&
              (type_second_num == 0 &&
               num_is_zero(tmp_2_num) == 0)) ||  // проверка на то что это
                                                 // значение в результате даст 0
             (num_is_zero(tmp_1_num) == 1 && (type_second_num == 1)) ||
             (num_is_zero(tmp_1_num) == 1 && (type_second_num == 2)) ||
             ((type_first_num == 0 && num_is_zero(tmp_1_num) == 0) &&
              type_second_num == 1) ||
             ((type_first_num == 0 && num_is_zero(tmp_1_num) == 0) &&
              type_second_num == 2)) {
    decimal_reset(result);
    set_sign(result, sign_num_result);
    // иначе, если оба числа просто числа
  } else {
    // настраиваем числа и знаки
    set_sign(&tmp_1_num, 0);
    set_sign(&tmp_2_num, 0);
    norm_from_decimal_to_big_decimal(tmp_1_num, tmp_2_num, &dividend, &divisor,
                                     4);
    tmp_scale_dividend = big_get_scale(dividend);
    tmp_scale_divisor = big_get_scale(divisor);
    big_set_scale(&dividend, 0);  // устанавливаем скейл 0
    big_set_scale(&divisor, 0);   // устанавливаем скейл 0
    if (tmp_scale_dividend != tmp_scale_divisor) {
      tmp_scale_dividend -= tmp_scale_divisor;
      tmp_scale_divisor = 0;
    } else {
      tmp_scale_dividend = 0;
      tmp_scale_divisor = 0;
    }
    copy_from_big_decimal_to_big_decimal(
        dividend, &tmp_1b_num_start);  // копируем  делимое в делимое, которое
                                       // останется неизменным
    copy_from_big_decimal_to_big_decimal(
        divisor, &tmp_2b_num_start);  // копируем делитель в делитель, который
                                      // останется неизменным
    copy_from_big_decimal_to_big_decimal(
        divisor, &subtrahend);  // копируем делитель в вычитаемое
    int flag_stop = 0;  // флаг окончания цикла
    int count_recalculation_cycle = 0;  // счётчик цикла перерасчёта
    while (flag_stop == 0) {
      big_division_action(dividend, divisor, &tmp_result_num,
                          0);  // получаем целую часть числа при делении
      if (count_recalculation_cycle == 0) {
        if (big_is_greater(tmp_result_num, big_num_dec_max) == 1) {
          if (sign_num_result == 0) {
            set_number_is_infinity(result, 0);
            flag_result = 1;
          } else {
            set_number_is_infinity(result, 1);
            flag_result = 2;
          }
          flag_stop = 1;
        }
      }
      if (flag_result == 0) {
        copy_from_big_decimal_to_big_decimal(
            tmp_result_num, &tmp_num_int_part);  // заносим значение целой части
                                                 // числа в переменную
        big_decimal_reset(&tmp_result_num);
        big_division_action(dividend, divisor, &tmp_result_num,
                            1);  // получаем остаток от деления
        if (big_num_is_zero(tmp_result_num) ==
            1) {  // если остаток равен 0, то записываем целую часть в результат
          flag_stop = 1;
        } else {  // иначе, перезапускаем цикл с увеличенным dividend: dividend
                  // * 10
          if (count_recalculation_cycle < 31) {
            count_recalculation_cycle++;  // увеличиваем счётчик цикла
                                          // перерасчёта
            big_decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
            big_decimal_reset(&dividend);  // обнуляем структуру уменьшаемого
            big_decimal_reset(
                &tmp_result_num);  // обнуляем структуру уменьшаемого
            copy_from_big_decimal_to_big_decimal(
                tmp_1b_num_start,
                &dividend);  // копируем в структуру делимого неизменное делимое
                             // (базовое значение)
            big_mul_by_ten_change_scale(
                &dividend,
                count_recalculation_cycle);  // умножаем на 10 делимое столько
                                             // раз скольки равно число счётчика
                                             // цикла перерасчёта
          } else {
            flag_stop = 1;
          }
        }
      }
    }
    if (flag_result == 0) {
      // определяем окончательную степень
      tmp_scale_result = tmp_scale_dividend + count_recalculation_cycle;
      // устанавливаем степень
      big_set_scale(&tmp_num_int_part, tmp_scale_result);
      // устанавливаем знак
      big_set_sign(&tmp_num_int_part, sign_num_result);
      flag_result = convert_big_decimal_to_decimal(tmp_num_int_part, result);
    }
  }
  return flag_result;
}
