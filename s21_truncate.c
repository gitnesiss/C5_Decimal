#include "s21_decimal.h"

int subtraction_truncate(s21_decimal value_1, s21_decimal value_2,
                         s21_decimal *result);

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int flag_result = 0;
  int new_bit = 0;
  s21_decimal tmp_result = {0};
  // s21_big_decimal big_tmp_result = {0};
  s21_decimal dividend = {
      {value.bits[0], value.bits[1], value.bits[2], value.bits[3]}};  // делимое
  int type_first_num = value_type(
      dividend);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int tmp_num_scale = get_scale(dividend);  // степень числа value
  if (type_first_num == 0) {  // если это 0 - нормальное число
    if (tmp_num_scale != 0) {
      s21_decimal divisor = {{1, 0, 0, 0}};  // делитель = вычитаемое
      s21_decimal minuend = {0};             // уменьшаемое
      s21_decimal subtrahend = {0};  // вычитаемое = делитель
      s21_decimal subtraction = {0};  // разность делимого и делителя
      s21_decimal tmp_zero = {0};  // число ноль
      int sign_num_result =
          0;  // знак числа, который пойдёт в результируещее число
      // определяем знак числа, который пойдёт в итоговое значение
      if (get_sign(dividend) == 0) {
        sign_num_result = 0;
      } else {
        sign_num_result = 1;
      }
      set_sign_plus(&dividend);
      // проверка того что число меньше 1, но больше либо равно 0
      if (s21_is_less(dividend, divisor) == 1 &&
          s21_is_greater_or_equal(dividend, tmp_zero) == 1) {
        decimal_reset(&tmp_result);
        set_sign(&tmp_result, sign_num_result);
        *result = tmp_result;
      } else {
        mul_by_ten_change_scale(&divisor, tmp_num_scale);
        copy_from_decimal_to_decimal(
            divisor, &subtrahend);  // копируем делитель в вычитаемое
        int cur_bit_d = (D_NOT_SCALE - 1);  // текущий бит делимого
        int cur_bit_q = (D_NOT_SCALE - 1);  // текущий бит частного
        int counter_division = 0;  // счётчик количества действий деления
        while (cur_bit_d >= 0) {
          decimal_reset(
              &subtraction);  // Обнуляем структуру, которая содержит результат
                              // вычитания: minuend - subtrahend
          shift_left_1_bits(&minuend);  // Сдвигаем биты уменьшаемого влево на 1
          new_bit = get_bit(dividend, cur_bit_d);
          set_bit(&minuend, 0,
                  new_bit);  // Устанавливаем в 0-ой бит уменьшаемого
                             // (dividend), соответствующий бит из
                             // позиции текущего бита делимого
                             // (cur_bit_d) числа dividend
          subtraction_truncate(
              minuend, subtrahend,
              &subtraction);  // Вычитаем из dividend divisor, а результат
                              // записываем в subtraction
          if (counter_division == 0) {
            if (s21_is_greater_or_equal(subtraction, tmp_zero) ==
                1) {  // если subtraction (разность dividend и divisor) больше
                      // или равно 0
              set_bit(&tmp_result, cur_bit_q,
                      1);  // устанавливаем в структуру tmp_result_num (quotient
                           // - частное) в бит cur_bit_q бит равный 1
              decimal_reset(&minuend);  // очищаем структуру уменьшаемого
              copy_from_decimal_to_decimal(
                  subtraction, &minuend);  // копируем разность в уменьшаемое
              counter_division++;
              cur_bit_q--;
            }
          } else {
            if (s21_is_greater(subtraction, tmp_zero) ==
                1) {  // если разность больше 0
              set_bit(&tmp_result, cur_bit_q,
                      1);  // устанавливаем в текущий бит частного 1
              decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
              copy_from_decimal_to_decimal(
                  subtraction, &minuend);  // копируем в структуру уменьшаемого
                                           // остаток от вычитания (разность)
            } else if (s21_is_equal(subtraction, tmp_zero) ==
                       1) {  // если разность равна 0
              set_bit(&tmp_result, cur_bit_q,
                      1);  // устанавливаем в текущий бит частного 1
              decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
            } else {  // (big_is_less(subtraction, tmp_zero) == 1) если разность
                      // меньше 0
              set_bit(&tmp_result, cur_bit_q,
                      0);  // устанавливаем в текущий бит частного 0
            }
            counter_division++;
            cur_bit_q--;
          }
          cur_bit_d--;
        }
        // Функция сдвига битов вправо, чтобы правильно указать число в
        // quotient. Сначала в число quotient записывается число начиная с 95
        // бита до 0 бита (аналог частного при делении в столбик, частное
        // двигается вправо). Указатель counter_quotient показывает на какой
        // позиции остановилось деление. Соотвтетственно все биты числа quotient
        // левее этого указателя равны 0, теперь всё число нужно сдвинуть вправо
        // до 0-вого бита.
        for (int i = cur_bit_q; i >= 0; i--) {
          shift_right_1_bits(&tmp_result);
        }
        // устанавливаем знак
        set_sign(&tmp_result, sign_num_result);
        copy_from_decimal_to_decimal(tmp_result, result);
      }
    } else {
      copy_from_decimal_to_decimal(value, result);
    }
  } else {  // иначе это 1 - INF, 2 - N_INF, 3 - NAN
    if (type_first_num == 1) {
      set_number_is_infinity(result, 0);
    }
    if (type_first_num == 2) {
      set_number_is_infinity(result, 1);
    }
    if (type_first_num == 3) {
      set_number_is_not_a_number(result, 1);
    }
    flag_result = 1;
  }
  return flag_result;
}

int subtraction_truncate(s21_decimal value_1, s21_decimal value_2,
                         s21_decimal *result) {
  s21_decimal tmp_value_1 = {0};
  s21_decimal tmp_value_2 = {0};
  int flag_result = 0;
  int bit_current_first_num = 0;
  int bit_current_second_num = 0;
  int bit_sub = 0;
  int count_bit = 0;
  int cycle_stop = 0;
  if (s21_is_greater_or_equal(value_1, value_2) ==
      1) {  // если первое число больше второго
    copy_from_decimal_to_decimal(value_1, &tmp_value_1);
    copy_from_decimal_to_decimal(value_2, &tmp_value_2);
  } else {
    copy_from_decimal_to_decimal(value_1, &tmp_value_2);
    copy_from_decimal_to_decimal(value_2, &tmp_value_1);
    set_sign_minus(result);
  }
  for (int i = 0; i < D_NOT_SCALE && flag_result == 0; i++) {
    bit_current_first_num = get_bit(tmp_value_1, i);
    bit_current_second_num = get_bit(tmp_value_2, i);
    count_bit = i + 1;
    cycle_stop = 0;
    if (bit_current_first_num >=
        bit_current_second_num) {  // если бит первого числа больше или равен
                                   // биту второго числа
      bit_sub = bit_current_first_num - bit_current_second_num;
      set_bit(result, i, bit_sub);
    } else {  // иначе, если бит первого числа меньше бита второго числа
      while (cycle_stop == 0) {
        if (get_bit(tmp_value_1, count_bit) == 1) {
          set_bit(&tmp_value_1, count_bit, 0);
          set_bit(result, i, 1);
          cycle_stop = 1;
        } else {
          set_bit(&tmp_value_1, count_bit, 1);
        }
        count_bit++;
      }
    }
  }
  return flag_result;
}
