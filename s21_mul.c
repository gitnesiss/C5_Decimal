#include "s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag_result = 0;
  // s21_decimal tmp_1_num = {{value_1.bits[0], value_1.bits[1],
  // value_1.bits[2],
  //                           value_1.bits[3]}};  // 1-ое входящее decimal
  //                           число
  // s21_decimal tmp_2_num = {{value_2.bits[0], value_2.bits[1],
  // value_2.bits[2],
  //                           value_2.bits[3]}};  // 2-ое входящее decimal
  //                           число
  s21_decimal tmp_1_num = value_1;  // 1-ое входящее decimal число
  s21_decimal tmp_2_num = value_2;  // 2-ое входящее decimal число
  s21_decimal tmp_result = {0};
  s21_big_decimal mult_1 = {0};  // множитель 1
  s21_big_decimal mult_2 = {0};  // множитель 2
  s21_big_decimal tmp_result_num = {0};  // произведение двух множителей
  s21_big_decimal term_1 = {0};  // слагаемое 1
  s21_big_decimal term_2 = {0};  // слагаемое 2
  s21_big_decimal summ = {0};    // сумма
  int sign_num_result = 0;  // знак числа, который пойдёт в результируещее число
  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_first_num = value_type(tmp_1_num);
  int type_second_num = value_type(tmp_2_num);
  int tmp_scale_mult_1 = 0;  // степень первого числа
  int tmp_scale_mult_2 = 0;  // степень второго числа
  int scale_result = 0;  // итоговое значение степени при расчётах
  int first_bit_mult_2 = 0;  // первый значащий бит 2-ого множителя

  // проверка на -INF, +INF, NAN
  if ((type_first_num == 0 && num_is_zero(tmp_2_num) == 1) ||
      (num_is_zero(tmp_1_num) == 1 && type_second_num == 0)) {
    decimal_reset(&tmp_result);
    *result = tmp_result;
    // copy_from_decimal_to_decimal(tmp_result, result);
  } else if (((type_first_num == 0 && num_is_zero(tmp_1_num) == 0 &&
               get_sign(tmp_1_num) == 0) &&
              type_second_num == 1) ||
             ((type_first_num == 0 && num_is_zero(tmp_1_num) == 0 &&
               get_sign(tmp_1_num) == 1) &&
              type_second_num == 2) ||
             (type_first_num == 1 &&
              (type_second_num == 0 && num_is_zero(tmp_2_num) == 0 &&
               get_sign(tmp_2_num) == 0)) ||
             (type_first_num == 1 && type_second_num == 1) ||
             (type_first_num == 2 && type_second_num == 2) ||
             (type_first_num == 2 &&
              (type_second_num == 0 && num_is_zero(tmp_2_num) == 0 &&
               get_sign(tmp_2_num) == 1))) {
    set_number_is_infinity(&tmp_result, 0);
    *result = tmp_result;
    // copy_from_decimal_to_decimal(tmp_result, result);
    flag_result = 1;
  } else if (((type_first_num == 0 && num_is_zero(tmp_1_num) == 0 &&
               get_sign(tmp_1_num) == 0) &&
              type_second_num == 2) ||
             ((type_first_num == 0 && num_is_zero(tmp_1_num) == 0 &&
               get_sign(tmp_1_num) == 1) &&
              type_second_num == 1) ||
             (type_first_num == 1 &&
              (type_second_num == 0 && num_is_zero(tmp_2_num) == 0 &&
               get_sign(tmp_2_num) == 1)) ||
             (type_first_num == 1 && type_second_num == 2) ||
             (type_first_num == 2 && type_second_num == 1) ||
             (type_first_num == 2 &&
              (type_second_num == 0 && num_is_zero(tmp_2_num) == 0 &&
               get_sign(tmp_2_num) == 0))) {
    set_number_is_infinity(&tmp_result, 1);
    *result = tmp_result;
    // copy_from_decimal_to_decimal(tmp_result, result);
    flag_result = 2;
  } else if ((num_is_zero(tmp_1_num) == 1 && type_second_num == 1) ||
             (num_is_zero(tmp_1_num) == 1 && type_second_num == 2) ||
             (type_first_num == 1 && num_is_zero(tmp_2_num) == 1) ||
             (type_first_num == 2 && num_is_zero(tmp_2_num) == 1) ||
             (type_first_num == 3) || (type_second_num == 3)) {
    set_number_is_not_a_number(&tmp_result, 1);
    *result = tmp_result;
    // copy_from_decimal_to_decimal(tmp_result, result);
    flag_result = 3;
  } else {
    // определяем знак числа, который пойдёт в итоговое значение
    sign_num_result = final_sign(tmp_1_num, tmp_2_num);
    // копируем первое число во временное первое число
    copy_from_decimal_to_big_decimal(tmp_1_num, &mult_1);
    // копируем второе число во временное второе число
    copy_from_decimal_to_big_decimal(tmp_2_num, &mult_2);
    // определяем степень числа, которая пойдёт в итоговое значение
    tmp_scale_mult_1 = big_get_scale(mult_1);
    tmp_scale_mult_2 = big_get_scale(mult_2);
    scale_result = tmp_scale_mult_1 + tmp_scale_mult_2;
    // настраиваем числа, степени и знаки
    big_set_scale(&mult_1, 0);  // устанавливаем степень 1-ого множителя 0
    big_set_scale(&mult_2, 0);  // устанавливаем степень 2-ого множителя 0
    big_set_sign_plus(&mult_1);  // устанавливаем знак "+" у 1-ого множителя
    big_set_sign_plus(&mult_2);  // устанавливаем знак "+" у 2-ого множителя
    // узнаём номер бита (index) с которого начинаются значащие биты делителя
    // (на каком бите встречается первая еденица начиная с 95 бита и до 0
    // включительно)
    first_bit_mult_2 = big_find_first_significant_bit(mult_2);
    int count_cycle = 0;  // счётчик циклов
    while (count_cycle <= first_bit_mult_2) {
      if (big_get_bit(mult_2, count_cycle) == 1) {
        copy_from_big_decimal_to_big_decimal(mult_1, &term_2);
        for (int k = 0; k < count_cycle; k++) {
          big_shift_left_1_bits(&term_2);
        }
        copy_from_big_decimal_to_big_decimal(summ, &term_1);
        big_decimal_reset(&summ);
        big_addition_action(term_1, term_2, &summ);
      }
      count_cycle++;
    }
    copy_from_big_decimal_to_big_decimal(summ, &tmp_result_num);
    big_set_sign(&tmp_result_num, sign_num_result);
    big_set_scale(&tmp_result_num, scale_result);
    flag_result = convert_big_decimal_to_decimal(tmp_result_num, result);
  }

  return flag_result;
}
