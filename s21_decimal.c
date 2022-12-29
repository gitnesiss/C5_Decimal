#include "s21_decimal.h"

/* В ЭТОМ ФАЙЛЕ РАССПОЛОЖЕНЫ ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ */

void print_decimal_to_binary(s21_decimal num) {
  for (int i = 2; i >= 0; i--) {
    print_binary(sizeof(num.bits[i]), &num.bits[i]);
    printf(" ");
  }
  print_binary(sizeof(num.bits[SCALE]), &num.bits[SCALE]);
  puts("");
}

void print_binary(size_t const size, void const *const ptr) {
  unsigned char *b = (unsigned char *)ptr;
  unsigned char byte;
  int i, j;
  for (i = size - 1; i >= 0; i--) {
    for (j = 7; j >= 0; j--) {
      byte = (b[i] >> j) & 1;
      printf("%u", byte);
    }
    printf(" ");
  }
}

int get_bit(s21_decimal num, int index) {
  unsigned int mask_one_bits = 1;
  unsigned int mask = 1u << (index % 32);
  int value_bit_num =
      ((num.bits[index / 32] & mask) >> (index % 32)) & mask_one_bits;
  return value_bit_num;
}

int set_bit(s21_decimal *num, int index, int value) {
  int flag_result = 0;
  s21_decimal temp_num = {
      {num->bits[0], num->bits[1], num->bits[2], num->bits[3]}};
  if (((index >= 0) && (index < 128)) && ((value == 0) || (value == 1))) {
    int current_bit = get_bit(temp_num, index);
    unsigned int mask = 1u << (index % 32);
    if (current_bit == 0) {
      if (value == 1) {
        temp_num.bits[index / 32] |= mask;
      }
    } else {
      if (value == 0) {
        temp_num.bits[index / 32] ^= mask;
      }
    }
    copy_from_decimal_to_decimal(temp_num, num);
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int get_scale(s21_decimal num) {
  unsigned int value = num.bits[SCALE];
  int mask = 0x00FF0000;
  int number = (value & mask) >> 16;
  return number;
}

int set_scale(s21_decimal *num, int pow_dec) {
  int flag_result = 0;
  int mask_for_zero = 255 << 16;
  int mask = pow_dec << 16;
  num->bits[SCALE] |= mask_for_zero;  // обнуляем степень
  num->bits[SCALE] ^= mask_for_zero;  // обнуляем степень
  num->bits[SCALE] |= mask;  // устанавливаем нужную степень
  return flag_result;
}

int get_sign(s21_decimal num) {
  int index_sign = 127;
  return get_bit(num, index_sign);
}

int set_sign(s21_decimal *num, int value_sign) {
  int flag_result = 0;
  int index_sign = 127;
  set_bit(num, index_sign, value_sign);
  return flag_result;
}

int set_sign_minus(s21_decimal *num) {
  int flag_result = 0;
  int index_sign = 127;
  int sign = 1;
  set_bit(num, index_sign, sign);
  return flag_result;
}

int set_sign_plus(s21_decimal *num) {
  int flag_result = 0;
  int index_sign = 127;
  int sign = 0;
  set_bit(num, index_sign, sign);
  return flag_result;
}

int change_sign(s21_decimal *num) {
  int sign_num = get_sign(*num);
  if (sign_num == 0) {
    set_sign_minus(num);
  } else {
    set_sign_plus(num);
  }
  return 0;
}

int decimal_reset(s21_decimal *num) {
  int flag_result = 0;
  for (int i = 0; i < 4; i++) {
    num->bits[i] = 0;
  }
  return flag_result;
}

int opposite_num(s21_decimal value_1, s21_decimal value_2) {
  int flag_result = 1;  // FALSE = 0, TRUE = 1;
  s21_decimal tmp_value_1 = {
      {value_1.bits[0], value_1.bits[1], value_1.bits[2], value_1.bits[3]}};
  s21_decimal tmp_value_2 = {
      {value_2.bits[0], value_2.bits[1], value_2.bits[2], value_2.bits[3]}};
  s21_decimal tmp_value_2_negate = {0};
  s21_negate(tmp_value_2, &tmp_value_2_negate);
  if (s21_is_equal(tmp_value_1, tmp_value_2_negate) == 1) {
    flag_result = 1;
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int alignment_of_powers_of_decimal(s21_decimal *value_1, s21_decimal *value_2) {
  int flag_result = 0;
  int tmp_scale_num_1 = get_scale(*value_1);
  int tmp_scale_num_2 = get_scale(*value_2);
  if (tmp_scale_num_1 != 0 && tmp_scale_num_2 != 0) {
    if (tmp_scale_num_1 >= tmp_scale_num_2) {
      mul_by_ten_change_scale(value_1, tmp_scale_num_1);
      mul_by_ten_change_scale(value_2, tmp_scale_num_1);
    } else {  // tmp_scale_num_1 < tmp_scale_num_2
      mul_by_ten_change_scale(value_1, tmp_scale_num_2);
      mul_by_ten_change_scale(value_2, tmp_scale_num_2);
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int big_remove_extra_zero(s21_big_decimal *num) {
  int flag_result = 0;
  int flag_action = 0;
  int tmp_num_scale = big_get_scale(*num);
  int tmp_num_sign = big_get_sign(*num);
  if (tmp_num_scale != 0) {
    s21_big_decimal tmp_num = {0};
    s21_big_decimal tmp_num_result = {0};
    s21_big_decimal tmp_num_ten = {{10, 0, 0, 0}};
    copy_from_big_decimal_to_big_decimal(*num, &tmp_num);  // - 756 478,900
    big_set_sign(&tmp_num, 0);                             // 756 478,900
    big_set_scale(&tmp_num, 0);  // 756 478 900   scale = 3   sign = 1
    int cycle_stop = 0;
    while (cycle_stop == 0) {
      big_division_action(tmp_num, tmp_num_ten, &tmp_num_result,
                          1);  // определяем остаток
      if (big_num_is_zero(tmp_num_result) == 1 && tmp_num_scale != 0) {
        flag_action = 1;
        tmp_num_scale--;
        big_decimal_reset(&tmp_num_result);
        big_division_action(tmp_num, tmp_num_ten, &tmp_num_result, 0);
        copy_from_big_decimal_to_big_decimal(tmp_num_result, &tmp_num);
      } else {
        cycle_stop = 1;
      }
    }
    if (flag_action == 1) {
      copy_from_big_decimal_to_big_decimal(tmp_num, num);
      big_set_sign(num, tmp_num_sign);
      big_set_scale(num, tmp_num_scale);
    }
  }
  return flag_result;
}

int value_type(s21_decimal num) {
  int flag_result = 0;
  int cycle_stop = 0;
  int scale_num = get_scale(num);
  if (scale_num == 255) {
    for (int i = 0; i < 3 && cycle_stop == 0; i++) {  // проверка числа на NaN
      if (num.bits[i] != 0) {
        cycle_stop = 1;
        flag_result = 3;
      }
    }
    if (cycle_stop == 0) {
      int sign_num = get_sign(num);
      if (sign_num == 0) {
        flag_result = 1;
      } else {
        flag_result = 2;
      }
    }
  }
  return flag_result;
}

int num_is_zero(s21_decimal value) {
  int flag_result = 0;
  if (get_scale(value) == 255) {
    flag_result = 0;
  } else {
    if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0) {
      flag_result = 1;
    } else {
      flag_result = 0;
    }
  }
  return flag_result;
}

int num_is_inf(s21_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  if (get_scale(value) == 255 && get_sign(value) == 0) {
    for (int i = 0; i < 3 && flag_stop == 0; i++) {
      if (value.bits[i] != 0) {
        flag_result = 0;
        flag_stop = 1;
      } else {
        flag_result = 1;
      }
    }
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int num_is_n_inf(s21_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  if (get_scale(value) == 255 && get_sign(value) == 1) {
    for (int i = 0; i < 3 && flag_stop == 0; i++) {
      if (value.bits[i] != 0) {
        flag_result = 0;
        flag_stop = 1;
      } else {
        flag_result = 1;
      }
    }
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int num_is_nan(s21_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  if (get_scale(value) == 255) {
    for (int i = 0; i < 3 && flag_stop == 0; i++) {
      if (value.bits[i] != 0) {
        flag_result = 1;
        flag_stop = 1;
      } else {
        flag_result = 0;
      }
    }
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int set_number_is_infinity(s21_decimal *value, int sign) {
  int flag_result = 0;
  set_scale(value, 255);  // выставляем степень все биты 11111111
  for (int i = 0; i < 3;
       i++) {  // выставляем все элементы массива кроме bits[SCALE] как 0
    value->bits[i] = 0;
  }
  if (sign == 0) {
    set_sign_plus(value);
  } else {
    set_sign_minus(value);
  }
  return flag_result;
}

int set_number_is_not_a_number(s21_decimal *value, int sign) {
  int flag_result = 0;
  set_scale(value, 255);  // выставляем степень все биты 11111111
  for (int i = 0; i < 3;
       i++) {  // выставляем все элементы массива кроме bits[SCALE] как 0
    value->bits[i] = 1431655765;
  }
  if (sign == 0) {
    set_sign_plus(value);
  } else {
    set_sign_minus(value);
  }
  return flag_result;
}

int shift_left_1_bits(s21_decimal *value) {
  int flag_result = 0;
  if ((get_bit(*value, (D_NOT_SCALE - 1)) == 0)) {
    for (int i = (D_NOT_SCALE / 32 - 1); i >= 0; i--) {
      if (i != 0) {
        value->bits[i] = value->bits[i] << 1;
        set_bit(value, 32 * i, get_bit(*value, (32 * i) - 1));
      } else {
        value->bits[i] = value->bits[i] << 1;
      }
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int shift_left_3_bits(s21_decimal *value) {
  int flag_result = 0;
  if ((get_bit(*value, (D_NOT_SCALE - 1)) == 0) &&
      (get_bit(*value, (D_NOT_SCALE - 2)) == 0) &&
      (get_bit(*value, (D_NOT_SCALE - 3)) == 0)) {
    for (int i = (D_NOT_SCALE / 32 - 1); i >= 0; i--) {
      if (i != 0) {
        value->bits[i] = value->bits[i] << 3;
        set_bit(value, 32 * i + 2, get_bit(*value, 32 * i - 1));
        set_bit(value, 32 * i + 1, get_bit(*value, 32 * i - 2));
        set_bit(value, 32 * i, get_bit(*value, 32 * i - 3));
      } else {
        value->bits[i] = value->bits[i] << 3;
      }
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int shift_right_1_bits(s21_decimal *value) {
  int flag_result = 0;
  for (int i = 0; i < (D_NOT_SCALE / 32); i++) {
    value->bits[i] = value->bits[i] >> 1;
    if (i !=
        ((D_NOT_SCALE / 32) - 1)) {  // если элемент массива не равен последнему
                                     // элементу мантиссы (i != 2)
      set_bit(value, (32 * (i + 1) - 1), get_bit(*value, (32 * (i + 1))));
    } else {  // если это второй элемент массива (value.bits[2])
      set_bit(value, (D_NOT_SCALE - 1), 0);
    }
  }
  return flag_result;
}

int shift_right_3_bits(s21_decimal *value) {
  int flag_result = 0;

  for (int i = 0; i < (D_NOT_SCALE / 32); i++) {  // i = 0 ... 95
    value->bits[i] = value->bits[i] >> 3;         //
    if (i != ((D_NOT_SCALE / 32) - 1)) {
      set_bit(value, (32 * (i + 1) - 3),
              get_bit(*value, (32 * (i + 1))));  // 96 -> 94
      set_bit(value, (32 * (i + 1) - 2), get_bit(*value, (32 * (i + 1) + 1)));
      set_bit(value, (32 * (i + 1) - 1), get_bit(*value, (32 * (i + 1) + 2)));
    } else {
      for (int j = (D_NOT_SCALE - 3); j < D_NOT_SCALE; j++) {
        set_bit(value, j, 0);
      }
    }
  }
  return flag_result;
}

int mul_by_ten(s21_decimal *value, int how_many_times) {
  int flag_result = 0;
  int scale_num_value = get_scale(*value);
  int sign_num_value = get_sign(*value);
  int current_scale = scale_num_value;
  for (int i = 0; i < how_many_times && flag_result == 0; i++) {
    s21_decimal tmp_result = {0};
    s21_decimal tmp_num_shift_3 = {{value->bits[LOW], value->bits[MID],
                                    value->bits[HIGH], value->bits[SCALE]}};
    s21_decimal tmp_num_shift_1 = {{value->bits[LOW], value->bits[MID],
                                    value->bits[HIGH], value->bits[SCALE]}};
    if (get_bit(*value, (D_NOT_SCALE - 1)) == 1 ||
        get_bit(*value, (D_NOT_SCALE - 2)) == 1 ||
        get_bit(*value, (D_NOT_SCALE - 3)) == 1) {
      set_number_is_infinity(
          value, sign_num_value);  // устанавливаем число бесконечность
      if (sign_num_value == 0) {
        flag_result = 1;
      } else {
        flag_result = 2;
      }
    } else {
      flag_result = shift_left_3_bits(&tmp_num_shift_3);
      if (flag_result == 0) {
        shift_left_1_bits(&tmp_num_shift_1);
        flag_result = s21_add(tmp_num_shift_1, tmp_num_shift_3, &tmp_result);
        if (flag_result == 0 || flag_result == 1 || flag_result == 2) {
          current_scale++;
          set_scale(&tmp_result, current_scale);
          copy_from_decimal_to_decimal(tmp_result, value);
        }
      }
    }
  }
  return flag_result;
}

int mul_by_ten_change_scale(s21_decimal *value, int how_many_times) {
  int flag_result = 0;
  int cycle_stop = 0;
  for (int i = 0; i < how_many_times && flag_result == 0 && cycle_stop == 0;
       i++) {
    s21_decimal tmp_result = {0};
    s21_decimal tmp_num_shift_3 = {{value->bits[LOW], value->bits[MID],
                                    value->bits[HIGH], value->bits[SCALE]}};
    s21_decimal tmp_num_shift_1 = {{value->bits[LOW], value->bits[MID],
                                    value->bits[HIGH], value->bits[SCALE]}};
    int scale_num_value = get_scale(*value);
    int sign_num_value = get_sign(*value);
    if (scale_num_value >=
        how_many_times) {  // если степень decimal >= "сколько раз *10"
      scale_num_value -=
          how_many_times;  // из степени вычитаем "сколько раз *10"
      set_scale(value, scale_num_value);  // установить степень числа decimal
      cycle_stop = 1;
    } else {  // иначе (если степень deciaml < "сколько раз *10")
      how_many_times -= scale_num_value;
      if (get_bit(*value, 95) == 1 || get_bit(*value, 94) == 1 ||
          get_bit(*value, 93) ==
              1) {  // проверка что число переполнится при перемножении (+inf
                    // или -inf в зависимости от знака)
        flag_result = 1;
        set_number_is_infinity(
            value, sign_num_value);  // устанавливаем число бесконечность
      } else {
        flag_result = shift_left_3_bits(&tmp_num_shift_3);
        if (flag_result == 0) {
          flag_result = shift_left_1_bits(&tmp_num_shift_1);
          flag_result = s21_add(tmp_num_shift_1, tmp_num_shift_3, &tmp_result);
          if (flag_result == 0 || flag_result == 1 || flag_result == 2) {
            set_scale(&tmp_result, 0);
            copy_from_decimal_to_decimal(tmp_result, value);
          }
        }
      }
    }
  }
  return flag_result;
}

int div_by_ten(s21_decimal *value, int how_many_times) {
  s21_decimal value_1 = {{value->bits[LOW], value->bits[MID], value->bits[HIGH],
                          value->bits[SCALE]}};
  s21_decimal value_2 = {{10, 0, 0, 0}};
  s21_decimal result = {0};
  for (int i = 0; i < how_many_times; i++) {
    s21_div(value_1, value_2, &result);
    copy_from_decimal_to_decimal(result, &value_1);
  }
  copy_from_decimal_to_decimal(result, value);
  return 0;
}

int div_by_ten_change_scale(s21_decimal *value, int how_many_times) {
  s21_decimal value_1 = {{value->bits[LOW], value->bits[MID], value->bits[HIGH],
                          value->bits[SCALE]}};
  s21_decimal value_2 = {{10, 0, 0, 0}};
  s21_decimal result = {0};
  int scale_value = get_scale(value_1);
  for (int i = 0; i < how_many_times; i++) {
    if (scale_value > 0) {
      scale_value--;
    }
    s21_div(value_1, value_2, &result);
    copy_from_decimal_to_decimal(result, &value_1);
  }
  copy_from_decimal_to_decimal(result, value);
  return 0;
}

int big_div_by_ten(s21_big_decimal *value, int how_many_times) {
  s21_big_decimal value_1 = {0};
  s21_big_decimal value_2 = {{10, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal result = {0};
  copy_from_big_decimal_to_big_decimal(*value, &value_1);
  for (int i = 0; i < how_many_times; i++) {
    big_division_action(value_1, value_2, &result, 0);
    copy_from_big_decimal_to_big_decimal(result, &value_1);
  }
  copy_from_big_decimal_to_big_decimal(result, value);
  return 0;
}

int big_div_by_ten_change_scale(s21_big_decimal *value, int how_many_times) {
  int flag_result = 0;
  s21_big_decimal value_1 = {0};
  s21_big_decimal value_2 = {{10, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal result = {0};
  if (how_many_times > 0) {
    int value_scale = big_get_scale(*value);
    copy_from_big_decimal_to_big_decimal(*value, &value_1);
    for (int i = 0; i < how_many_times; i++) {
      if (value_scale > 0) {
        value_scale--;
      }
      big_division_action(value_1, value_2, &result, 0);
      copy_from_big_decimal_to_big_decimal(result, &value_1);
    }
    copy_from_big_decimal_to_big_decimal(result, value);
    big_set_scale(value, value_scale);
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int find_first_significant_bit(s21_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  for (int i = (D_NOT_SCALE - 1); i >= 0 && flag_stop == 0; i--) {
    if (value.bits[i] == 1) {
      flag_result = i;
      flag_stop = 1;
    }
  }
  return flag_result;
}

int big_find_first_significant_bit(s21_big_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  for (int i = (B_D_NOT_SCALE - 1); i >= 0 && flag_stop == 0; i--) {
    if (big_get_bit(value, i) == 1) {
      flag_result = i;
      flag_stop = 1;
    }
  }
  return flag_result;
}

int big_bank_rounding(s21_big_decimal *value, int scale_to_round) {
  s21_big_decimal tmp_num = *value;
  s21_big_decimal tmp_num_result = {0};
  s21_big_decimal value_to_round_next = {0};
  s21_big_decimal value_to_round_curr = {0};
  s21_big_decimal big_num_one = {
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0x00000000}};  // число 1 типа big_decimal
  s21_big_decimal big_num_two = {
      {2, 0, 0, 0, 0, 0, 0, 0, 0, 0x00000000}};  // число 1 типа big_decimal
  s21_big_decimal big_num_five = {
      {5, 0, 0, 0, 0, 0, 0, 0, 0, 0x00000000}};  // число 5 типа big_decimal
  s21_big_decimal big_num_ten = {
      {10, 0, 0, 0, 0, 0, 0, 0, 0, 0x00000000}};  // число 10 типа big_decimal
  int flag_result = 0;
  int tmp_num_scale = big_get_scale(tmp_num);
  int cycle = 0;
  big_set_sign_plus(&tmp_num);
  big_set_scale(&tmp_num, 0);
  if (big_num_is_zero(tmp_num) ==
      0) {  // 87 641 977 779,000 000 222 219 777 645654  10001010010
            // 00110010100000001101110101010101 11100111101111101001110100111000
            // 00011100110010001001110001010110
    if (scale_to_round < tmp_num_scale) {  // scale_to_round - разряд дробной
                                           // части, до которой нужно округлить
      cycle = tmp_num_scale - (scale_to_round + 1);
      while (cycle > 0) {
        cycle--;
        big_decimal_reset(&tmp_num_result);
        big_division_action(tmp_num, big_num_ten, &tmp_num_result, 0);
        copy_from_big_decimal_to_big_decimal(tmp_num_result, &tmp_num);
      }
      big_division_action(tmp_num, big_num_ten, &value_to_round_next,
                          1);  // определяем остаток в 29 разряд: 6 - 10 = -4,
                               // соответственно остаток 6
      big_decimal_reset(&tmp_num_result);
      big_division_action(tmp_num, big_num_ten, &tmp_num_result,
                          0);  // определяем число в мантисе до 28 степени
      copy_from_big_decimal_to_big_decimal(tmp_num_result, &tmp_num);
      big_division_action(tmp_num_result, big_num_ten, &value_to_round_curr,
                          1);  // определяем число (остаток) в 28 разряде
      big_decimal_reset(&tmp_num_result);
      if (big_is_greater_or_equal(value_to_round_next, big_num_five) ==
          1) {  // если остаток (число в 29 разряде) больше 5
        if (big_is_greater(value_to_round_curr, big_num_five) == 1) {
          norm_big_decimal(&tmp_num, &big_num_one, 1);
          big_addition_action(tmp_num, big_num_one,
                              &tmp_num_result);  // добавляем 1 в 28 разряд
        } else {
          big_division_action(tmp_num, big_num_two, &tmp_num_result, 1);
          if (big_is_equal(tmp_num_result, big_num_one) == 1) {
            norm_big_decimal(&tmp_num, &big_num_one, 1);
            big_addition_action(tmp_num, big_num_one,
                                &tmp_num_result);  // добавляем 1 в 28 разряд
          } else {
            copy_from_big_decimal_to_big_decimal(tmp_num, &tmp_num_result);
          }
        }
      } else {
        copy_from_big_decimal_to_big_decimal(tmp_num, &tmp_num_result);
      }
      copy_from_big_decimal_to_big_decimal(tmp_num_result, value);
      big_set_scale(value, scale_to_round);  // установка степени в число
    } else {
      flag_result = 1;
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

/* VVVVVVVVVVVVVVVVVVVVVVV Конвертер big_decimal в decimal
 * VVVVVVVVVVVVVVVVVVVVVVV */

// int convert_from_big_decimal_to_int(s21_decimal src, int *dst) {
//   int flag_result = 0;
//   int flag_stop = 0;
//   int sign = get_sign(src);
//   int scale = get_scale(src);
//   s21_decimal int_src;
//   if (scale < 29) {
//     s21_truncate(src, &int_src);
//     for (int i = 1; i < ((B_D_NOT_SCALE / 32) - 1) && flag_stop == 0; i++) {
//       if (int_src.bits[i] != 0) {
//         flag_result = 1;
//         flag_stop = 1;
//       }
//     }
//     if (flag_stop == 1) {
//       if (sign == 0) {
//         if (int_src.bits[0] <= 2147483647) {
//           *dst = int_src.bits[0];
//         } else {
//           flag_result = 1;
//         }
//       } else {
//         if (int_src.bits[0] <= 2147483648) {
//           *dst = int_src.bits[0];
//         } else {
//           flag_result = 1;
//         }
//       }
//     }
//   } else {
//     flag_result = 1;
//   }
//   return flag_result;
// }

int convert_big_decimal_to_decimal(s21_big_decimal value, s21_decimal *result) {
  s21_big_decimal tmp_num = {0};  // исходное число типа big_decimal
  s21_big_decimal tmp_num_int = {0};  // целая часть числа tmp_num
  s21_big_decimal temp = {0};         // временное число
  s21_big_decimal tmp_num_result = {0};  // число для расчётов
  s21_big_decimal big_num_one = {
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0x00000000}};  // число 1 типа big_decimal
  s21_big_decimal big_num_int_max = {{4294967291, 4294967295, 4294967295, 9, 0,
                                      0, 0, 0, 0,
                                      0x00010000}};  // макс.целое  79 228
                                                     // 162 514 264 337
                                                     // 593 543 950 335|,5
                                                     // => INF =     1001
                                                     // 11111111 11111111
                                                     // 11111111 11111111
                                                     // 11111111 11111111
                                                     // 11111111 11111111
                                                     // 11111111 11111111
                                                     // 11111111 11111011
  s21_big_decimal big_num_dec_max_nine = {{2684354555, 1836193738, 1126043566,
                                           1, 0, 0, 0, 0, 0,
                                           0x001D0000}};  // макс.дробное 0.999
                                                          // 999 999 999 999 999
                                                          // 999 999 999 9|5 =>
                                                          // 1 = 00000001
                                                          // 01000011 00011110
                                                          // 00001111 10101110
                                                          // 01101101 01110010
                                                          // 00010111 11001010
                                                          // 10011111 11111111
                                                          // 11111111 11111011
  s21_big_decimal big_num_dec_max_zero = {
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 0x001C0000}};
  s21_big_decimal big_num_dec_max = {{4294967295, 4294967295, 4294967295, 0, 0,
                                      0, 0, 0, 0,
                                      0x00000000}};  // максимальное число
                                                     // близкое к +INF
                                                     // типа big_decimal
  int flag_result = 0;
  int tmp_num_sign = 0;    // знак исходного числа
  int tmp_num_scale = 0;   // степень исходно числа
  int digits_tmp_num = 0;  // количество разрядов в числе
  int digits_tmp_num_int = 0;  // количество разрядов в целой части числа
  int digits_tmp_num_dec_max =
      0;  // количество разрядов в дробной части числа максимальное разрешенное
  int cycle = 0;
  int digits_max = 0;
  copy_from_big_decimal_to_big_decimal(value, &tmp_num);
  tmp_num_sign = big_get_sign(tmp_num);
  tmp_num_scale = big_get_scale(tmp_num);
  big_set_sign_plus(&tmp_num);
  // выделяем целую часть числа
  flag_result = big_truncate(tmp_num, &tmp_num_int);
  // если выделенная целая часть числа равна 0
  if (big_num_is_zero(tmp_num_int) == 1) {  // работаем как с числом 0,0...218
    // если степень исходного числа больше или равна 29
    if (tmp_num_scale >= 29) {  // если степень исходного числа >= 29
      if (big_is_greater_or_equal(tmp_num, big_num_dec_max_nine) ==
          1) {  // если число больше 0.9 999 999 999 999 999 999 999 999 999|5
                // => 1
        copy_from_big_decimal_to_big_decimal(
            big_num_one, &tmp_num_result);  // устанавливаем еденицу
      } else {
        digits_max = 28;
        digits_tmp_num = big_digits_in_mantis(
            tmp_num);  // разряды дробной части (величина степени мантисы),
                       // которую надо вычесть из степени исходного числа

        cycle = tmp_num_scale - digits_max;
        if (cycle <= 0) {
          copy_from_big_decimal_to_decimal(tmp_num, result);
        } else {  // cycle > 0
          big_div_by_ten_change_scale(&tmp_num, cycle - 1);
          int digits_zero = digits_max - (tmp_num_scale - digits_tmp_num);
          if (digits_zero > 0) {
            big_bank_rounding(&tmp_num, 28);
            copy_from_big_decimal_to_big_decimal(tmp_num, &tmp_num_result);
          } else {
            set_number_is_not_a_number(result, tmp_num_sign);
            flag_result = 3;
          }
        }
      }
    } else {  // tmp_num_scale < 29, копируем значение
      copy_from_big_decimal_to_big_decimal(tmp_num, &tmp_num_result);
    }
  } else {  // работаем как с числом 3456,0...218
    // если целая часть числа попадает в диапазон [-7.9х10^28, 7.9х10^28]
    //  big_is_greater_or_equal(tmp_num, big_num_int_max) == 1
    if (big_is_greater(tmp_num_int, big_num_dec_max_zero) == 1 &&
        big_is_less_or_equal(tmp_num_int, big_num_dec_max) == 1) {
      if (big_is_greater_or_equal(tmp_num, big_num_int_max) == 1) {
        if (tmp_num_sign == 0) {
          set_number_is_infinity(result, 0);
          flag_result = 1;
        } else {
          set_number_is_infinity(result, 1);
          flag_result = 2;
        }
      } else {
        if (tmp_num_scale > 0) {  // если scale исходного числа > 0
          digits_max = 29;  // макс. кол-во разрядов, помещающихся в число
          digits_tmp_num = big_digits_in_mantis(
              tmp_num);  // кол-во разрядов в исходном числе
          digits_tmp_num_int = big_digits_in_mantis(
              tmp_num_int);  // кол-во разрядов в целой части
          digits_tmp_num_dec_max =
              29 - digits_tmp_num_int;  // кол-во разрядов в дробной части,
                                        // которая поместится в число
          if (digits_tmp_num_dec_max <= tmp_num_scale) {
            cycle = digits_tmp_num -
                    (digits_tmp_num_int +
                     digits_tmp_num_dec_max);  // количество разрядов которые
                                               // убираем у числа до 29 разряда
            copy_from_big_decimal_to_big_decimal(tmp_num, &temp);
            big_div_by_ten_change_scale(&temp, cycle);
            big_set_scale(&temp, 0);
            if (big_is_greater(temp, big_num_dec_max) == 1) {
              tmp_num_scale--;
              big_bank_rounding(&tmp_num, digits_tmp_num_dec_max - 1);
            } else {
              big_bank_rounding(&tmp_num, digits_tmp_num_dec_max);
            }
            copy_from_big_decimal_to_big_decimal(tmp_num, &tmp_num_result);
          } else {
            copy_from_big_decimal_to_big_decimal(tmp_num, &tmp_num_result);
          }
        } else {  // если scale <= 0
          copy_from_big_decimal_to_big_decimal(tmp_num, &tmp_num_result);
        }
      }
    } else {  // иначе, выставляем значение бесконечности
      if (tmp_num_sign == 0) {
        set_number_is_infinity(result, 0);
        flag_result = 1;
      } else {
        set_number_is_infinity(result, 1);
        flag_result = 2;
      }
    }
  }
  if (flag_result == 0) {
    big_set_sign(&tmp_num_result, tmp_num_sign);
    copy_from_big_decimal_to_decimal(tmp_num_result, result);
  }
  return flag_result;
}

/* ^^^^^^^^^^^^^^^^^^^^^^^ Конвертер big_decimal в decimal
 * ^^^^^^^^^^^^^^^^^^^^^^^ */

/* VVVVVVVVVVVVVVVVVVVVVVVVVVVVV Функции копирования
 * VVVVVVVVVVVVVVVVVVVVVVVVVVVVV */

int copy_from_decimal_to_decimal(s21_decimal value_1, s21_decimal *value_2) {
  int flag_result = 0;
  for (int i = 0; i < 4; i++) {
    value_2->bits[i] = value_1.bits[i];
  }
  return flag_result;
}

int copy_from_decimal_to_big_decimal(s21_decimal value_1,
                                     s21_big_decimal *value_2) {
  int flag_result = 0;
  for (int i = 0; i < 3; i++) {
    value_2->bits[i] = value_1.bits[i];
  }
  value_2->bits[((B_D - 1) / 32)] = value_1.bits[((DEC - 1) / 32)];
  return flag_result;
}

int copy_from_big_decimal_to_decimal(s21_big_decimal value_1,
                                     s21_decimal *value_2) {
  int flag_result = 0;
  for (int i = (D_NOT_SCALE / 32 - 1); i >= 0; i--) {
    value_2->bits[i] = value_1.bits[i];
  }
  value_2->bits[(D_NOT_SCALE / 32)] = value_1.bits[(B_D_NOT_SCALE / 32)];
  return flag_result;
}

int copy_from_big_decimal_to_big_decimal(s21_big_decimal value_1,
                                         s21_big_decimal *value_2) {
  int flag_result = 0;
  for (int i = 0; i < (B_D / 32); i++) {
    value_2->bits[i] = value_1.bits[i];
  }
  return flag_result;
}

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Функции копирования
 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* VVVVVVVVVVVVVVVVVVVVVVVVVVVV Функции  нормализации
 * VVVVVVVVVVVVVVVVVVVVVVVVVVVV */

int norm_from_decimal_to_big_decimal(s21_decimal value_1, s21_decimal value_2,
                                     s21_big_decimal *big_value_1,
                                     s21_big_decimal *big_value_2,
                                     int math_action) {
  s21_decimal tmp_1_num = {
      {value_1.bits[0], value_1.bits[1], value_1.bits[2], value_1.bits[3]}};
  s21_decimal tmp_2_num = {
      {value_2.bits[0], value_2.bits[1], value_2.bits[2], value_2.bits[3]}};
  int flag_result =
      0;  // флаг: показать прошла ли нормализация 0 - FALSE, 1 - TRUE.
  int action = math_action;
  int sign_first_num = get_sign(value_1);
  int sign_second_num = get_sign(value_2);
  int scale_first_num = get_scale(value_1);
  int scale_second_num = get_scale(value_2);
  int tmp_scale_result_val_1 = 0;
  int tmp_scale_result_val_2 = 0;
  // настраиваем числа и знаки
  copy_from_decimal_to_big_decimal(
      tmp_1_num,
      big_value_1);  // копируем первое число во временное первое число
  copy_from_decimal_to_big_decimal(
      tmp_2_num,
      big_value_2);  // копируем второе число во временное второе число
  big_set_sign_plus(big_value_1);  // устанавливаем знак "+" у первого числа
  big_set_sign_plus(big_value_2);  // устанавливаем знак "+" у второго числа
  if (scale_first_num != scale_second_num) {
    if (scale_first_num > scale_second_num) {
      big_mul_by_ten_change_scale(big_value_1, scale_first_num);
      big_mul_by_ten_change_scale(big_value_2, scale_first_num);
      tmp_scale_result_val_1 = scale_first_num;
      tmp_scale_result_val_2 = scale_first_num;
    } else {
      big_mul_by_ten_change_scale(big_value_1, scale_second_num);
      big_mul_by_ten_change_scale(big_value_2, scale_second_num);
      tmp_scale_result_val_1 = scale_second_num;
      tmp_scale_result_val_2 = scale_second_num;
    }
    flag_result = 1;
  }
  if (action == 2 || action == 4 || action == 5) {
    big_set_scale(big_value_1, 0);
    big_set_scale(big_value_2, 0);
    while (big_is_less(*big_value_1, *big_value_2) == 1) {
      big_mul_by_ten_change_scale(big_value_1, 1);
      tmp_scale_result_val_1++;
    }
    flag_result = 1;
  }
  if (flag_result == 1) {
    big_set_scale(big_value_1, tmp_scale_result_val_1);
    big_set_scale(big_value_2, tmp_scale_result_val_2);
    big_set_sign(big_value_1, sign_first_num);
    big_set_sign(big_value_2, sign_second_num);
  }
  return flag_result;
}

int norm_big_decimal(s21_big_decimal *value_1, s21_big_decimal *value_2,
                     int math_action) {
  s21_big_decimal tmp_1_num = {0};
  s21_big_decimal tmp_2_num = {0};
  int flag_result =
      0;  // флаг: показать прошла ли нормализация 0 - FALSE, 1 - TRUE.
  int action = math_action;
  int sign_first_num = big_get_sign(*value_1);
  int sign_second_num = big_get_sign(*value_2);
  int scale_first_num = big_get_scale(*value_1);
  int scale_second_num = big_get_scale(*value_2);
  int tmp_scale_result_val_1 = 0;
  int tmp_scale_result_val_2 = 0;
  // настраиваем числа и знаки
  copy_from_big_decimal_to_big_decimal(
      *value_1, &tmp_1_num);  // копируем первое число во временное первое число
  copy_from_big_decimal_to_big_decimal(
      *value_2, &tmp_2_num);  // копируем второе число во временное второе число
  big_set_sign_plus(&tmp_1_num);  // устанавливаем знак "+" у первого числа
  big_set_sign_plus(&tmp_2_num);  // устанавливаем знак "+" у второго числа
  if (scale_first_num != scale_second_num) {
    if (scale_first_num > scale_second_num) {
      big_mul_by_ten_change_scale(&tmp_1_num, scale_first_num);
      big_mul_by_ten_change_scale(&tmp_2_num, scale_first_num);
      tmp_scale_result_val_1 = scale_first_num;
      tmp_scale_result_val_2 = scale_first_num;
    } else {
      big_mul_by_ten_change_scale(&tmp_1_num, scale_second_num);
      big_mul_by_ten_change_scale(&tmp_2_num, scale_second_num);
      tmp_scale_result_val_1 = scale_second_num;
      tmp_scale_result_val_2 = scale_second_num;
    }
    flag_result = 1;
  }
  if (action == 2 || action == 4 || action == 5) {
    while (big_is_less(tmp_1_num, tmp_2_num) == 1) {
      big_mul_by_ten_change_scale(&tmp_1_num, 1);
      tmp_scale_result_val_1++;
    }
    flag_result = 1;
  }
  if (flag_result == 1) {
    big_set_scale(&tmp_1_num, tmp_scale_result_val_1);
    big_set_scale(&tmp_2_num, tmp_scale_result_val_2);
    big_set_sign(&tmp_1_num, sign_first_num);
    big_set_sign(&tmp_2_num, sign_second_num);
  }
  copy_from_big_decimal_to_big_decimal(tmp_1_num, value_1);
  copy_from_big_decimal_to_big_decimal(tmp_2_num, value_2);
  return flag_result;
}

/* ^^^^^^^^^^^^^^^ Функции  нормализации ^^^^^^^^^^^^^^^ */

/* VVVVVVVVVV Функции математических действий VVVVVVVVVV */

// int subtraction_action(s21_decimal value_1, s21_decimal value_2,
//                        s21_decimal *result) {
//   int flag_result = 0;
//   int bit_current_first_num = 0;
//   int bit_current_second_num = 0;
//   int bit_sub = 0;
//   int count_bit = 0;
//   int cycle_stop = 0;
//   for (int i = 0; i < D_NOT_SCALE && flag_result == 0; i++) {
//     bit_current_first_num = get_bit(value_1, i);
//     bit_current_second_num = get_bit(value_2, i);
//     count_bit = i + 1;
//     cycle_stop = 0;
//     if (bit_current_first_num >=
//         bit_current_second_num) {  // если бит первого числа больше или равен
//                                    // биту второго числа
//       bit_sub = bit_current_first_num - bit_current_second_num;
//       set_bit(result, i, bit_sub);
//     } else {  // иначе, если бит первого числа меньше бита второго числа
//       while (cycle_stop == 0) {
//         if (get_bit(value_1, count_bit) == 1) {
//           set_bit(&value_1, count_bit, 0);
//           set_bit(result, i, 1);
//           cycle_stop = 1;
//         } else {
//           set_bit(&value_1, count_bit, 1);
//         }
//         count_bit++;
//       }
//     }
//   }
//   return flag_result;
// }

int big_addition_action(s21_big_decimal term_1, s21_big_decimal term_2,
                        s21_big_decimal *result) {
  int flag_result = 0;  // 361400724   695075222 178956970
  int bit_current_first_num = 0;
  int bit_current_second_num = 0;
  int bit_summ = 0;
  int bit_next_num = 0;
  int big_sign_first_num = big_get_sign(term_1);  // знак первого числа
  int big_sign_second_num = big_get_sign(term_2);  // знак второго числа
  if (big_sign_first_num == big_sign_second_num) {
    int big_scale_first_num = big_get_scale(term_1);  // степень первого числа
    int big_scale_second_num = big_get_scale(term_2);  // степень второго числа
    // если степени равны или выравнены
    if (big_scale_first_num == big_scale_second_num) {
      for (int i = 0; i < B_D_NOT_SCALE; i++) {
        bit_current_first_num = big_get_bit(term_1, i);
        bit_current_second_num = big_get_bit(term_2, i);
        bit_summ = 0;
        bit_summ =
            bit_current_first_num + bit_current_second_num + bit_next_num;
        if (bit_summ == 3) {
          bit_next_num = 1;
          big_set_bit(result, i, 1);
        } else if (bit_summ == 2) {
          bit_next_num = 1;
          big_set_bit(result, i, 0);
        } else if (bit_summ == 1) {
          bit_next_num = 0;
          big_set_bit(result, i, 1);
        } else {
          bit_next_num = 0;
          big_set_bit(result, i, 0);
        }
      }
      // устанавливаем в result.bits[HIGH] знак и степень
      big_set_scale(result, big_scale_first_num);
      big_set_sign(result, big_sign_first_num);
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int big_subtraction_action(s21_big_decimal value_1, s21_big_decimal value_2,
                           s21_big_decimal *result) {
  s21_big_decimal tmp_value_1 = {0};
  s21_big_decimal tmp_value_2 = {0};
  int flag_result = 0;
  int bit_current_first_num = 0;
  int bit_current_second_num = 0;
  int bit_sub = 0;
  int count_bit = 0;
  int cycle_stop = 0;
  int big_sign_first_num = big_get_sign(value_1);  // знак первого числа
  int big_sign_second_num = big_get_sign(value_2);  // знак второго числа
  if (big_is_greater_or_equal(tmp_value_1, tmp_value_2) == 1 &&
      big_sign_first_num == big_sign_second_num) {
    // правильное выставление числа, выставлем числа так чтоб первым стояло
    // число с большим значением
    if (big_sign_first_num == 0) {
      if (big_is_greater_or_equal(value_1, value_2) ==
          1) {  // если первое число больше второго
        copy_from_big_decimal_to_big_decimal(value_1, &tmp_value_1);
        copy_from_big_decimal_to_big_decimal(value_2, &tmp_value_2);
        big_set_sign_plus(result);
      } else {
        copy_from_big_decimal_to_big_decimal(value_1, &tmp_value_2);
        copy_from_big_decimal_to_big_decimal(value_2, &tmp_value_1);
        big_set_sign_minus(result);
      }
    } else {
      if (big_is_greater_or_equal(value_1, value_2) ==
          1) {  // если первое число больше второго
        copy_from_big_decimal_to_big_decimal(value_1, &tmp_value_2);
        copy_from_big_decimal_to_big_decimal(value_2, &tmp_value_1);
        big_set_sign_minus(result);
      } else {
        copy_from_big_decimal_to_big_decimal(value_1, &tmp_value_1);
        copy_from_big_decimal_to_big_decimal(value_2, &tmp_value_2);
        big_set_sign_plus(result);
      }
    }
    // цикл вычитания
    for (int i = 0; i < B_D_NOT_SCALE && flag_result == 0; i++) {
      bit_current_first_num = big_get_bit(tmp_value_1, i);
      bit_current_second_num = big_get_bit(tmp_value_2, i);
      count_bit = i + 1;
      cycle_stop = 0;
      if (bit_current_first_num >=
          bit_current_second_num) {  // если бит первого числа больше или равен
                                     // биту второго числа
        bit_sub = bit_current_first_num - bit_current_second_num;
        big_set_bit(result, i, bit_sub);
      } else {  // иначе, если бит первого числа меньше бита второго числа
        while (cycle_stop == 0) {
          if (big_get_bit(tmp_value_1, count_bit) == 1) {
            big_set_bit(&tmp_value_1, count_bit, 0);
            big_set_bit(result, i, 1);
            cycle_stop = 1;
          } else {
            big_set_bit(&tmp_value_1, count_bit, 1);
          }
          count_bit++;
        }
      }
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int big_division_action(s21_big_decimal value_1, s21_big_decimal value_2,
                        s21_big_decimal *result, int kind_of_num) {
  s21_big_decimal dividend = {0};  // dividend - делимое
  s21_big_decimal divisor = {0};  // divisor - делитель = вычитаемое
  s21_big_decimal tmp_result_num = {
      0};  // quotient - частное, результат деления чисел типа big decimal
  s21_big_decimal minuend = {0};     // уменьшаемое
  s21_big_decimal subtrahend = {0};  // вычитаемое = делитель
  s21_big_decimal subtraction = {0};  // разность делимого и делителя
  s21_big_decimal tmp_num_zero = {0};  // ноль
  s21_big_decimal remainder = {0};     // остаток
  s21_big_decimal divisor_opposit = {
      0};  // число противоположное по знаку делителю
  int flag_result = 0;
  int cur_bit_d = (B_D_NOT_SCALE - 1);  // текущий бит делимого
  int cur_bit_q = (B_D_NOT_SCALE - 1);  // текущий бит частного
  int counter_division = 0;  // счётчик количества действий деления
  int scale_1_num = big_get_scale(value_1);
  copy_from_big_decimal_to_big_decimal(
      value_1, &dividend);  // копируем делитель в вычитаемое
  copy_from_big_decimal_to_big_decimal(
      value_2, &divisor);  // копируем делитель в вычитаемое
  copy_from_big_decimal_to_big_decimal(
      divisor, &subtrahend);  // копируем делитель в вычитаемое
  while (cur_bit_d >= 0) {
    big_decimal_reset(
        &subtraction);  // Обнуляем структуру, которая содержит результат
                        // вычитания: minuend - subtrahend
    big_shift_left_1_bits(&minuend);  // Сдвигаем биты уменьшаемого влево на 1
    big_set_bit(
        &minuend, 0,
        big_get_bit(
            dividend,
            cur_bit_d));  // Устанавливаем в 0-ой бит уменьшаемого (dividend),
                          // соответствующий бит из позиции текущего бита
                          // делимого (cur_bit_d) числа dividend
    big_subtraction_action(minuend, subtrahend,
                           &subtraction);  // Вычитаем из dividend divisor, а
                                           // результат записываем в subtraction
    if (counter_division == 0) {
      if (big_is_greater_or_equal(subtraction, tmp_num_zero) ==
          1) {  // если subtraction (разность dividend и divisor) больше или
                // равно 0
        big_set_bit(&tmp_result_num, cur_bit_q,
                    1);  // устанавливаем в структуру tmp_result_num (quotient -
                         // частное) в бит cur_bit_q бит равный 1
        big_decimal_reset(&minuend);  // очищаем структуру уменьшаемого
        copy_from_big_decimal_to_big_decimal(
            subtraction, &minuend);  // копируем разность в уменьшаемое
        counter_division++;
        cur_bit_q--;
      }
    } else {
      if (big_is_greater(subtraction, tmp_num_zero) ==
          1) {  // если разность больше 0
        big_set_bit(&tmp_result_num, cur_bit_q,
                    1);  // устанавливаем в текущий бит частного 1
        if (cur_bit_d != 0) {
          big_decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
        }
        copy_from_big_decimal_to_big_decimal(
            subtraction, &minuend);  // копируем в структуру уменьшаемого
                                     // остаток от вычитания (разность)
      } else if (big_is_equal(subtraction, tmp_num_zero) ==
                 1) {  // если разность равна 0
        big_set_bit(&tmp_result_num, cur_bit_q,
                    1);  // устанавливаем в текущий бит частного 1
        if (cur_bit_d != 0) {
          big_decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
        }
      } else {  // (big_is_less(subtraction, tmp_num_zero) == 1) если разность
                // меньше 0
        big_set_bit(&tmp_result_num, cur_bit_q,
                    0);  // устанавливаем в текущий бит частного 0
      }
      counter_division++;
      cur_bit_q--;
    }
    cur_bit_d--;
  }
  for (int i = cur_bit_q; i >= 0; i--) {
    big_shift_right_1_bits(&tmp_result_num);
  }
  // выдача результата в зависимости от требуемого вида результата
  if (kind_of_num == 1) {  // возвращает остаток от деления
    // определяем остаток
    copy_from_big_decimal_to_big_decimal(subtrahend, &divisor_opposit);
    big_change_sign(&divisor_opposit);
    if (big_is_equal(subtraction, divisor_opposit) == 1 ||
        big_num_is_zero(subtraction) == 1) {
      big_decimal_reset(&remainder);
    } else if (big_is_greater(subtraction, tmp_num_zero) == 1) {
      copy_from_big_decimal_to_big_decimal(subtraction, &remainder);
    } else {  // -subtrahend < subtraction < 0
      copy_from_big_decimal_to_big_decimal(minuend, &remainder);
    }
    copy_from_big_decimal_to_big_decimal(remainder, result);
    big_set_scale(result, scale_1_num);
  } else {  // возвращает целое число
    copy_from_big_decimal_to_big_decimal(tmp_result_num, result);
    big_set_scale(result, scale_1_num);
  }
  return flag_result;
}

/* ^^^^^^^ Функции математических действий ^^^^^^^ */

int big_digits_in_mantis(s21_big_decimal value) {
  int digits = 0;
  s21_big_decimal tmp_num = {0};
  s21_big_decimal tmp_num_zero = {0};
  s21_big_decimal tmp_num_ten = {{10, 0, 0, 0, 0, 0, 0, 0, 0, 0x00000000}};
  s21_big_decimal tmp_num_result = {0};
  copy_from_big_decimal_to_big_decimal(value, &tmp_num);
  while (big_is_greater(tmp_num, tmp_num_zero) == 1) {
    big_division_action(tmp_num, tmp_num_ten, &tmp_num_result, 0);
    copy_from_big_decimal_to_big_decimal(tmp_num_result, &tmp_num);
    digits++;
  }
  return digits;
}

void print_big_decimal_to_binary(s21_big_decimal num) {
  for (int i = (B_D_NOT_SCALE / 32 - 1); i >= 0; i--) {
    print_binary(sizeof(num.bits[i]), &num.bits[i]);
    printf(" ");
  }
  print_binary(sizeof(num.bits[B_D / 32 - 1]), &num.bits[B_D / 32 - 1]);
  puts("");
}

int big_get_bit(s21_big_decimal num, int index) {
  unsigned int mask_one_bits = 1;
  unsigned int mask = 1u << (index % 32);
  int value_bit_num =
      ((num.bits[index / 32] & mask) >> (index % 32)) & mask_one_bits;
  return value_bit_num;
}

int big_set_bit(s21_big_decimal *num, int index, int value) {
  int flag_result = 0;
  int current_bit = big_get_bit(*num, index);
  unsigned int mask = 1u << (index % 32);
  if (current_bit == 0) {
    if (value == 1) {
      num->bits[index / 32] |= mask;
    }
  } else {
    if (value == 0) {
      num->bits[index / 32] ^= mask;
    }
  }
  return flag_result;
}

int big_get_scale(s21_big_decimal num) {
  unsigned int value = num.bits[((B_D - 1) / 32)];
  int mask = 0x00FF0000;
  int number = (value & mask) >> 16;
  return number;
}

int big_set_scale(s21_big_decimal *num, int pow_dec) {
  int flag_result = 0;
  int mask_for_zero = 255 << 16;
  int mask = pow_dec << 16;
  num->bits[((B_D - 1) / 32)] |= mask_for_zero;  // обнуляем степень
  num->bits[((B_D - 1) / 32)] ^= mask_for_zero;  // обнуляем степень
  num->bits[((B_D - 1) / 32)] |= mask;  // устанавливаем нужную степень
  return flag_result;
}

int big_get_sign(s21_big_decimal num) {
  int index_sign = (B_D - 1);
  return big_get_bit(num, index_sign);
}

int big_set_sign(s21_big_decimal *num, int value_sign) {
  int flag_result = 0;
  int index_sign = (B_D - 1);
  big_set_bit(num, index_sign, value_sign);
  return flag_result;
}

int big_set_sign_minus(s21_big_decimal *num) {
  int flag_result = 0;
  int index_sign = (B_D - 1);
  int sign = 1;
  big_set_bit(num, index_sign, sign);
  return flag_result;
}

int big_set_sign_plus(s21_big_decimal *num) {
  int flag_result = 0;
  int index_sign = (B_D - 1);
  int sign = 0;
  big_set_bit(num, index_sign, sign);
  return flag_result;
}

int big_change_sign(s21_big_decimal *num) {
  int sign_num = big_get_sign(*num);
  if (sign_num == 0) {
    big_set_sign_minus(num);
  } else {
    big_set_sign_plus(num);
  }
  return 0;
}

int final_sign(s21_decimal value_1, s21_decimal value_2) {
  int sign = 0;
  if ((get_sign(value_1) == 0 && get_sign(value_2) == 0) ||
      (get_sign(value_1) == 1 && get_sign(value_2) == 1)) {
    sign = 0;
  } else {
    sign = 1;
  }
  return sign;
}

int big_decimal_reset(s21_big_decimal *num) {
  int flag_result = 0;
  for (int i = 0; i < (B_D / 32); i++) {
    num->bits[i] = 0;
  }
  return flag_result;
}

int big_shift_left_1_bits(s21_big_decimal *value) {
  int flag_result = 0;
  if (value != NULL) {
    if ((big_get_bit(*value, (B_D_NOT_SCALE - 1)) == 0)) {
      for (int i = (B_D_NOT_SCALE / 32 - 1); i >= 0; i--) {
        if (i != 0) {
          value->bits[i] = value->bits[i] << 1;
          big_set_bit(value, 32 * i, big_get_bit(*value, (32 * i) - 1));
        } else {
          value->bits[i] = value->bits[i] << 1;
        }
      }
    } else {
      flag_result = 1;
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int big_shift_left_3_bits(s21_big_decimal *value) {
  int flag_result = 0;
  if (value != NULL) {
    if ((big_get_bit(*value, (B_D_NOT_SCALE - 1)) == 0) &&
        (big_get_bit(*value, (B_D_NOT_SCALE - 2)) == 0) &&
        (big_get_bit(*value, (B_D_NOT_SCALE - 3)) == 0)) {
      for (int i = (B_D_NOT_SCALE / 32 - 1); i >= 0; i--) {
        if (i != 0) {
          value->bits[i] = value->bits[i] << 3;
          big_set_bit(value, 32 * i + 2, big_get_bit(*value, 32 * i - 1));
          big_set_bit(value, 32 * i + 1, big_get_bit(*value, 32 * i - 2));
          big_set_bit(value, 32 * i, big_get_bit(*value, 32 * i - 3));
        } else {
          value->bits[i] = value->bits[i] << 3;
        }
      }
    } else {
      flag_result = 1;
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int big_shift_right_1_bits(s21_big_decimal *value) {
  int flag_result = 0;
  if (value != NULL) {
    for (int i = 0; i < (B_D_NOT_SCALE / 32); i++) {
      value->bits[i] = value->bits[i] >> 1;
      if (i != ((B_D_NOT_SCALE / 32) - 1)) {
        big_set_bit(value, (32 * (i + 1) - 1),
                    big_get_bit(*value, (32 * (i + 1))));
      } else {
        big_set_bit(value, (B_D_NOT_SCALE - 1), 0);
      }
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int big_shift_right_3_bits(s21_big_decimal *value) {
  int flag_result = 0;
  if (value != NULL) {
    for (int k = 0; k < 3; k++) {
      for (int i = 0; i < (B_D_NOT_SCALE / 32); i++) {
        value->bits[i] = value->bits[i] >> 1;
        if (i != ((B_D_NOT_SCALE / 32) - 1)) {
          big_set_bit(value, (32 * (i + 1) - 3),
                      big_get_bit(*value, (32 * (i + 1))));
          big_set_bit(value, (32 * (i + 1) - 2),
                      big_get_bit(*value, (32 * (i + 1) + 1)));
          big_set_bit(value, (32 * (i + 1) - 1),
                      big_get_bit(*value, (32 * (i + 1) + 2)));
        } else {
          for (int j = (B_D_NOT_SCALE - 3); j < B_D_NOT_SCALE; j++) {
            big_set_bit(value, j, 0);
          }
        }
      }
    }
  } else {
    flag_result = 1;
  }
  return flag_result;
}

int big_mul_by_ten(s21_big_decimal *value, int how_many_times) {
  int flag_result = 0;
  int scale_num_value = big_get_scale(*value);
  int sign_num_value = big_get_sign(*value);
  int current_scale = scale_num_value;
  for (int i = 0; i < how_many_times && flag_result == 0; i++) {
    s21_big_decimal tmp_result = {0};
    s21_big_decimal tmp_num_shift_3 = {0};
    s21_big_decimal tmp_num_shift_1 = {0};
    copy_from_big_decimal_to_big_decimal(*value, &tmp_num_shift_3);
    copy_from_big_decimal_to_big_decimal(*value, &tmp_num_shift_1);
    if (big_get_bit(*value, (B_D_NOT_SCALE - 1)) == 1 ||
        big_get_bit(*value, (B_D_NOT_SCALE - 2)) == 1 ||
        big_get_bit(*value, (B_D_NOT_SCALE - 3)) ==
            1) {  // проверка что число переполнится при перемножении (+inf или
                  // -inf в зависимости от знака)
      flag_result = 1;
      big_set_number_is_infinity(
          value, sign_num_value);  // устанавливаем число бесконечность
    } else {
      flag_result = big_shift_left_3_bits(&tmp_num_shift_3);
      if (flag_result == 0) {
        big_shift_left_1_bits(&tmp_num_shift_1);
        big_addition_action(tmp_num_shift_3, tmp_num_shift_1, &tmp_result);
        if (flag_result == 0 || flag_result == 1 || flag_result == 2) {
          current_scale++;
          big_set_scale(&tmp_result, current_scale);
          big_set_sign(&tmp_result, sign_num_value);
          copy_from_big_decimal_to_big_decimal(tmp_result, value);
        }
      }
    }
  }
  return flag_result;
}

int big_mul_by_ten_change_scale(s21_big_decimal *value, int how_many_times) {
  int flag_result = 0;
  int cycle_stop = 0;
  for (int i = 0; i < how_many_times && flag_result == 0 && cycle_stop == 0;
       i++) {
    s21_big_decimal tmp_result = {0};
    s21_big_decimal tmp_num_shift_3 = {0};
    s21_big_decimal tmp_num_shift_1 = {0};
    int scale_num_value = big_get_scale(*value);
    int sign_num_value = big_get_sign(*value);
    copy_from_big_decimal_to_big_decimal(*value, &tmp_num_shift_3);
    copy_from_big_decimal_to_big_decimal(*value, &tmp_num_shift_1);
    if (scale_num_value >=
        how_many_times) {  // если степень decimal >= "сколько раз *10"
      scale_num_value -=
          how_many_times;  // из степени вычитаем "сколько раз *10"
      big_set_scale(value,
                    scale_num_value);  // установить степень числа big_decimal
      cycle_stop = 1;
    } else {  // иначе (если степень deciaml < "сколько раз *10")
      how_many_times -= scale_num_value;
      if (big_get_bit(*value, (B_D_NOT_SCALE - 1)) == 1 ||
          big_get_bit(*value, (B_D_NOT_SCALE - 2)) == 1 ||
          big_get_bit(*value, (B_D_NOT_SCALE - 3)) ==
              1) {  // проверка что число переполнится при перемножении (+inf
                    // или -inf в зависимости от знака)
        flag_result = 1;
        big_set_number_is_infinity(
            value, sign_num_value);  // устанавливаем число бесконечность
      } else {
        flag_result = big_shift_left_3_bits(&tmp_num_shift_3);
        if (flag_result == 0) {
          flag_result = big_shift_left_1_bits(&tmp_num_shift_1);
          big_addition_action(tmp_num_shift_1, tmp_num_shift_3, &tmp_result);
          // flag_result = s21_add(tmp_num_shift_1, tmp_num_shift_3,
          // &tmp_result);
          if (flag_result == 0 || flag_result == 1 || flag_result == 2) {
            big_set_scale(&tmp_result, 0);
            copy_from_big_decimal_to_big_decimal(tmp_result, value);
          }
        }
      }
    }
  }
  return flag_result;
}

int big_set_number_is_infinity(s21_big_decimal *value, int sign) {
  int flag_result = 0;
  big_set_scale(value, 255);  // выставляем степень все биты 11111111
  for (int i = 0; i < (B_D_NOT_SCALE / 32);
       i++) {  // выставляем все элементы массива кроме bits[9] как 0
    value->bits[i] = 0;
  }
  if (sign == 0) {
    big_set_sign_plus(value);
  } else {
    big_set_sign_minus(value);
  }
  return flag_result;
}

int big_set_number_is_not_a_number(s21_big_decimal *value, int sign) {
  int flag_result = 0;
  big_set_scale(value, 255);  // выставляем степень все биты 11111111
  for (int i = 0; i < (B_D_NOT_SCALE / 32);
       i++) {  // выставляем все элементы массива кроме bits[SCALE]
    value->bits[i] = 1431655765;
  }
  if (sign == 0) {
    big_set_sign_plus(value);
  } else {
    big_set_sign_minus(value);
  }
  return flag_result;
}

int big_value_type(s21_big_decimal value) {
  int flag_result = 0;
  int cycle_stop = 0;
  int scale_num = big_get_scale(value);
  if (scale_num == 255) {
    for (int i = 0; i < (B_D_NOT_SCALE / 32) && cycle_stop == 0;
         i++) {  // проверка числа на NaN
      if (value.bits[i] != 0) {
        cycle_stop = 1;
        flag_result = 3;
      }
    }
    if (cycle_stop == 0) {
      int sign_num = big_get_sign(value);
      if (sign_num == 0) {
        flag_result = 1;
      } else {
        flag_result = 2;
      }
    }
  }
  return flag_result;
}

int big_num_is_zero(s21_big_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  if (big_get_scale(value) == 255) {
    flag_result = 0;
  } else {
    for (int i = 0; i < (B_D_NOT_SCALE / 32) && flag_stop == 0; i++) {
      if (value.bits[i] == 0) {
        flag_result = 1;
      } else {
        flag_result = 0;
        flag_stop = 1;
      }
    }
  }
  return flag_result;
}

int big_num_is_inf(s21_big_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  if (big_get_scale(value) == 255 && big_get_sign(value) == 0) {
    for (int i = 0; i < ((B_D / 32) - 1) && flag_stop == 0; i++) {
      if (value.bits[i] != 0) {
        flag_result = 0;
        flag_stop = 1;
      } else {
        flag_result = 1;
      }
    }
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int big_num_is_n_inf(s21_big_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  if (big_get_scale(value) == 255 && big_get_sign(value) == 1) {
    for (int i = 0; i < ((B_D / 32) - 1) && flag_stop == 0; i++) {
      if (value.bits[i] != 0) {
        flag_result = 0;
        flag_stop = 1;
      } else {
        flag_result = 1;
      }
    }
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int big_num_is_nan(s21_big_decimal value) {
  int flag_result = 0;
  int flag_stop = 0;
  if (big_get_scale(value) == 255) {
    for (int i = 0; i < ((B_D / 32) - 1) && flag_stop == 0; i++) {
      if (value.bits[i] != 0) {
        flag_result = 1;
        flag_stop = 1;
      } else {
        flag_result = 0;
      }
    }
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int big_is_greater(s21_big_decimal value_1, s21_big_decimal value_2) {
  s21_big_decimal big_num_1 = {0};
  s21_big_decimal big_num_2 = {0};
  int flag_result = 0;
  int flag_stop = 0;
  int sign_first_num = big_get_sign(value_1);
  int sign_second_num = big_get_sign(value_2);
  copy_from_big_decimal_to_big_decimal(value_1, &big_num_1);
  copy_from_big_decimal_to_big_decimal(value_2, &big_num_2);
  norm_big_decimal(&big_num_1, &big_num_2, 0);
  if (sign_first_num > sign_second_num) {
    flag_result = 0;
  } else if (sign_first_num < sign_second_num) {
    flag_result = 1;
  } else {  // sign_first_num == sign_second_num
    for (int i = 0; i < B_D_NOT_SCALE && flag_stop == 0; i++) {
      if ((big_get_bit(big_num_1, ((B_D_NOT_SCALE - 1) - i)) >
           big_get_bit(big_num_2, ((B_D_NOT_SCALE - 1) - i)))) {
        if (sign_first_num == 0) {
          flag_result = 1;
          flag_stop = 1;
        } else {  // sign_first_num == 1
          flag_result = 0;
          flag_stop = 1;
        }
      } else if ((big_get_bit(big_num_1, ((B_D_NOT_SCALE - 1) - i)) <
                  big_get_bit(big_num_2, ((B_D_NOT_SCALE - 1) - i)))) {
        if (sign_first_num == 0) {
          flag_result = 0;
          flag_stop = 1;
        } else {  // sign_first_num == 1
          flag_result = 1;
          flag_stop = 1;
        }
      } else {
        continue;
      }
    }
  }
  return flag_result;
}

int big_is_less(s21_big_decimal value_1, s21_big_decimal value_2) {
  int flag_result = 0;
  int flag_stop = 0;
  for (int i = 0; i < B_D_NOT_SCALE && flag_stop == 0; i++) {
    if ((big_get_bit(value_1, ((B_D_NOT_SCALE - 1) - i)) <
         big_get_bit(value_2, ((B_D_NOT_SCALE - 1) - i)))) {
      flag_result = 1;
      flag_stop = 1;
    } else if ((big_get_bit(value_1, ((B_D_NOT_SCALE - 1) - i)) >
                big_get_bit(value_2, ((B_D_NOT_SCALE - 1) - i)))) {
      flag_result = 0;
      flag_stop = 1;
    } else {
      continue;
    }
  }
  return flag_result;
}

int big_is_equal(s21_big_decimal value_1, s21_big_decimal value_2) {
  s21_big_decimal big_num_1 = {0};
  s21_big_decimal big_num_2 = {0};
  int flag_result = 1;
  int flag_stop = 0;
  int sign_first_num = big_get_sign(value_1);
  int sign_second_num = big_get_sign(value_2);
  int type_first_num = big_value_type(
      value_1);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_second_num = big_value_type(
      value_2);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  copy_from_big_decimal_to_big_decimal(value_1, &big_num_1);
  copy_from_big_decimal_to_big_decimal(value_2, &big_num_2);
  norm_big_decimal(&big_num_1, &big_num_2, 0);
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
    if (big_num_is_zero(value_1) == 1 && big_num_is_zero(value_2) == 1) {
      flag_result = 1;
    } else {
      if (sign_first_num !=
          sign_second_num) {  // Знак первого числа "+", а второго "-".
                              // Значение, указывающее на знак первого числа (0
                              // = "+") меньше знака второго (1 = "-").
        flag_result = 0;  // выставляем флаг FALSE = 0
      } else {
        for (int i = 0; i < B_D_NOT_SCALE && flag_stop == 0; i++) {
          if ((big_get_bit(big_num_1, ((B_D_NOT_SCALE - 1) - i)) !=
               big_get_bit(big_num_2, ((B_D_NOT_SCALE - 1) - i)))) {
            flag_result = 0;
            flag_stop = 1;
          } else {
            continue;
          }
        }
      }
    }
  }
  return flag_result;
}

int big_is_greater_or_equal(s21_big_decimal value_1, s21_big_decimal value_2) {
  int flag_result = 0;
  if (big_is_greater(value_1, value_2) == 1 ||
      big_is_equal(value_1, value_2) == 1) {
    flag_result = 1;
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int big_is_less_or_equal(s21_big_decimal value_1, s21_big_decimal value_2) {
  int flag_result = 0;
  if (big_is_greater(value_1, value_2) == 0) {
    flag_result = 1;
  } else {
    flag_result = 0;
  }
  return flag_result;
}

int big_mul(s21_big_decimal value_1, s21_big_decimal value_2,
            s21_big_decimal *result) {
  int flag_result = 0;

  big_decimal_reset(result);
  s21_big_decimal tmp_1_num = value_1;  // 1-ое входящее decimal число
  s21_big_decimal tmp_2_num = value_2;  // 2-ое входящее decimal число
  s21_big_decimal mult_1 = {0};         // множитель 1
  s21_big_decimal mult_2 = {0};         // множитель 2
  s21_big_decimal tmp_result_num = {0};  // произведение двух множителей
  s21_big_decimal term_1 = {0};  // слагаемое 1
  s21_big_decimal term_2 = {0};  // слагаемое 2
  s21_big_decimal summ = {0};    // сумма
  int sign_num_result = 0;  // знак числа, который пойдёт в результируещее число
  int tmp_scale_mult_1 = 0;  // степень первого числа
  int tmp_scale_mult_2 = 0;  // степень второго числа
  int scale_result = 0;  // итоговое значение степени при расчётах
  int first_bit_mult_2 = 0;  // первый значащий бит 2-ого множителя
  // copy_from_big_decimal_to_big_decimal(
  //     value_1, &tmp_1_num);  // копируем первое число во временное первое
  //     число
  // copy_from_big_decimal_to_big_decimal(
  //     value_2, &tmp_2_num);  // копируем второе число во временное второе
  //     число
  int type_first_num = big_value_type(
      tmp_1_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_second_num = big_value_type(
      tmp_2_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN

  // проверка на -INF, +INF, NAN
  if ((type_first_num == 0 && big_num_is_zero(tmp_2_num) == 1) ||
      (big_num_is_zero(tmp_1_num) == 1 && type_second_num == 0)) {
    big_decimal_reset(result);
  } else if (((type_first_num == 0 && big_num_is_zero(tmp_1_num) == 0 &&
               big_get_sign(tmp_1_num) == 0) &&
              type_second_num == 1) ||
             ((type_first_num == 0 && big_num_is_zero(tmp_1_num) == 0 &&
               big_get_sign(tmp_1_num) == 1) &&
              type_second_num == 2) ||
             (type_first_num == 1 &&
              (type_second_num == 0 && big_num_is_zero(tmp_2_num) == 0 &&
               big_get_sign(tmp_2_num) == 0)) ||
             (type_first_num == 1 && type_second_num == 1) ||
             (type_first_num == 2 && type_second_num == 2) ||
             (type_first_num == 2 &&
              (type_second_num == 0 && big_num_is_zero(tmp_2_num) == 0 &&
               big_get_sign(tmp_2_num) == 1))) {
    big_set_number_is_infinity(result, 0);
    flag_result = 1;
  } else if (((type_first_num == 0 && big_num_is_zero(tmp_1_num) == 0 &&
               big_get_sign(tmp_1_num) == 0) &&
              type_second_num == 2) ||
             ((type_first_num == 0 && big_num_is_zero(tmp_1_num) == 0 &&
               big_get_sign(tmp_1_num) == 1) &&
              type_second_num == 1) ||
             (type_first_num == 1 &&
              (type_second_num == 0 && big_num_is_zero(tmp_2_num) == 0 &&
               big_get_sign(tmp_2_num) == 1)) ||
             (type_first_num == 1 && type_second_num == 2) ||
             (type_first_num == 2 && type_second_num == 1) ||
             (type_first_num == 2 &&
              (type_second_num == 0 && big_num_is_zero(tmp_2_num) == 0 &&
               big_get_sign(tmp_2_num) == 0))) {
    big_set_number_is_infinity(result, 1);
    flag_result = 2;
  } else if ((big_num_is_zero(tmp_1_num) == 1 && type_second_num == 1) ||
             (big_num_is_zero(tmp_1_num) == 1 && type_second_num == 2) ||
             (type_first_num == 1 && big_num_is_zero(tmp_2_num) == 1) ||
             (type_first_num == 2 && big_num_is_zero(tmp_2_num) == 1) ||
             (type_first_num == 3) || (type_second_num == 3)) {
    big_set_number_is_not_a_number(result, 1);
    flag_result = 3;
  } else {
    // определяем знак числа, который пойдёт в итоговое значение
    if ((big_get_sign(tmp_1_num) == 0 && big_get_sign(tmp_2_num) == 0) ||
        (big_get_sign(tmp_1_num) == 1 && big_get_sign(tmp_2_num) == 1)) {
      sign_num_result = 0;
    } else {
      sign_num_result = 1;
    }
    copy_from_big_decimal_to_big_decimal(
        tmp_1_num, &mult_1);  // копируем первое число во временное первое число
    copy_from_big_decimal_to_big_decimal(
        tmp_2_num, &mult_2);  // копируем второе число во временное второе число
    // определяем степень числа, которая пойдёт в итоговое значение
    tmp_scale_mult_1 = big_get_scale(mult_1);
    tmp_scale_mult_2 = big_get_scale(mult_2);
    scale_result = tmp_scale_mult_1 + tmp_scale_mult_2;
    // настраиваем числа, степени и знаки
    big_set_scale(&mult_1, 0);  // устанавливаем степень 1-ого множителя 0
    big_set_scale(&mult_2, 0);  // устанавливаем степень 2-ого множителя 0
    big_set_sign_plus(&mult_1);  // устанавливаем знак "+" у 1-ого множителя
    big_set_sign_plus(&mult_2);  // устанавливаем знак "+" у 2-ого множителя
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
    copy_from_big_decimal_to_big_decimal(tmp_result_num, result);
  }
  return flag_result;
}

int big_div_result_int(s21_big_decimal value_1, s21_big_decimal value_2,
                       s21_big_decimal *result) {
  s21_big_decimal tmp_1_num = {0};  // dividend - делимое
  s21_big_decimal tmp_2_num = {0};  // divisor - делитель = вычитаемое
  s21_big_decimal dividend = {0};  // dividend - делимое
  s21_big_decimal divisor = {0};  // divisor - делитель = вычитаемое
  s21_big_decimal tmp_result_num = {
      0};  // quotient - частное, результат деления чисел типа big decimal
  s21_big_decimal minuend = {0};     // уменьшаемое
  s21_big_decimal subtrahend = {0};  // вычитаемое = делитель
  s21_big_decimal subtraction = {0};  // разность делимого и делителя
  s21_big_decimal tmp_zero_num = {0};  // ноль
  copy_from_big_decimal_to_big_decimal(value_1, &tmp_1_num);
  copy_from_big_decimal_to_big_decimal(value_2, &tmp_2_num);
  int flag_result = 0;
  int sign_num_result = 0;  // знак числа, который пойдёт в результируещее число
  int type_first_num = big_value_type(
      tmp_1_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int type_second_num = big_value_type(
      tmp_2_num);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int tmp_scale_dividend = 0;
  int tmp_scale_divisor = 0;
  // проверка на -INF, +INF, NAN, num/0
  if (type_first_num == 1 || type_first_num == 2 || type_first_num == 3 ||
      type_second_num == 3 || big_num_is_zero(tmp_2_num) == 1) {
    big_set_number_is_not_a_number(result, 1);
    flag_result = 3;
  } else if ((big_num_is_zero(tmp_1_num) == 1 &&
              (type_second_num == 0 && big_num_is_zero(tmp_2_num) == 0)) ||
             (big_num_is_zero(tmp_1_num) == 1 && (type_second_num == 1)) ||
             (big_num_is_zero(tmp_1_num) == 1 && (type_second_num == 2))) {
    big_decimal_reset(result);
    big_set_sign(result, sign_num_result);
    // иначе, если оба числа просто числа
  } else {
    // определяем знак числа, который пойдёт в итоговое значение
    if ((big_get_sign(tmp_1_num) == 0 && big_get_sign(tmp_2_num) == 0) ||
        (big_get_sign(tmp_1_num) == 1 && big_get_sign(tmp_2_num) == 1)) {
      sign_num_result = 0;
    } else {
      sign_num_result = 1;
    }
    // настраиваем числа и знаки
    copy_from_big_decimal_to_big_decimal(
        tmp_1_num,
        &dividend);  // копируем первое число во временное первое число
    copy_from_big_decimal_to_big_decimal(
        tmp_2_num,
        &divisor);  // копируем второе число во временное второе число
    big_set_sign_plus(&dividend);  // устанавливаем знак "+" у первого числа
    big_set_sign_plus(&divisor);  // устанавливаем знак "+" у второго числа
    tmp_scale_dividend = big_get_scale(dividend);
    tmp_scale_divisor = big_get_scale(divisor);
    if (big_get_scale(dividend) > big_get_scale(divisor)) {
      big_mul_by_ten_change_scale(&dividend, tmp_scale_dividend);
      big_mul_by_ten_change_scale(&divisor, tmp_scale_dividend);
    } else if (big_get_scale(dividend) < big_get_scale(divisor)) {
      big_mul_by_ten_change_scale(&dividend, tmp_scale_divisor);
      big_mul_by_ten_change_scale(&divisor, tmp_scale_divisor);
    }
    if (big_is_greater(dividend, divisor) == 1) {
      copy_from_big_decimal_to_big_decimal(
          divisor, &subtrahend);  // копируем делитель в вычитаемое
      int cur_bit_d = (B_D_NOT_SCALE - 1);  // текущий бит делимого
      int cur_bit_q = (B_D_NOT_SCALE - 1);  // текущий бит частного
      int counter_division = 0;  // счётчик количества действий деления
      // цикл деления для получения целого числа
      while (cur_bit_d >= 0) {
        big_decimal_reset(
            &subtraction);  // Обнуляем структуру, которая содержит результат
                            // вычитания: minuend - subtrahend
        big_shift_left_1_bits(
            &minuend);  // Сдвигаем биты уменьшаемого влево на 1
        big_set_bit(&minuend, 0, big_get_bit(dividend, cur_bit_d));
        big_subtraction_action(
            minuend, subtrahend,
            &subtraction);  // Вычитаем из dividend divisor, а результат
                            // записываем в subtraction
        if (counter_division == 0) {
          if (big_is_greater_or_equal(subtraction, tmp_zero_num) ==
              1) {  // если subtraction (разность dividend и divisor) больше или
                    // равно 0
            big_set_bit(
                &tmp_result_num, cur_bit_q,
                1);  // устанавливаем в структуру tmp_result_num (quotient -
                     // частное) в бит cur_bit_q бит равный 1
            big_decimal_reset(&minuend);  // очищаем структуру уменьшаемого
            copy_from_big_decimal_to_big_decimal(
                subtraction, &minuend);  // копируем разность в уменьшаемое
            counter_division++;
            cur_bit_q--;
          }
        } else {
          if (big_is_greater(subtraction, tmp_zero_num) ==
              1) {  // если разность больше 0
            big_set_bit(&tmp_result_num, cur_bit_q,
                        1);  // устанавливаем в текущий бит частного 1
            big_decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
            copy_from_big_decimal_to_big_decimal(
                subtraction, &minuend);  // копируем в структуру уменьшаемого
                                         // остаток от вычитания (разность)
          } else if (big_is_equal(subtraction, tmp_zero_num) ==
                     1) {  // если разность равна 0
            big_set_bit(&tmp_result_num, cur_bit_q,
                        1);  // устанавливаем в текущий бит частного 1
            big_decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
          } else {  // (big_is_less(subtraction, tmp_zero_num) == 1) если
                    // разность меньше 0
            big_set_bit(&tmp_result_num, cur_bit_q,
                        0);  // устанавливаем в текущий бит частного 0
          }
          counter_division++;
          cur_bit_q--;
        }
        cur_bit_d--;
      }
      for (int i = cur_bit_q; i >= 0; i--) {
        big_shift_right_1_bits(&tmp_result_num);
      }
      big_set_sign(&tmp_result_num, sign_num_result);
      big_set_scale(&tmp_result_num, 0);
      copy_from_big_decimal_to_big_decimal(tmp_result_num, result);
    } else if (big_is_equal(dividend, divisor) == 1) {
      big_decimal_reset(result);
      big_set_sign(result, sign_num_result);
      result->bits[0] = 1;
    } else {
      big_decimal_reset(result);
    }
  }
  return flag_result;
}

int big_truncate(s21_big_decimal value, s21_big_decimal *result) {
  int flag_result = 0;
  int type_first_num = big_value_type(
      value);  // 0 - нормальное число, 1 - INF, 2 - N_INF, 3 - NAN
  int tmp_num_scale = big_get_scale(value);  // степень числа value
  if (type_first_num == 0) {  // если это 0 - нормальное число
    if (tmp_num_scale != 0) {
      s21_big_decimal dividend = {0};  // делимое
      s21_big_decimal divisor = {{1, 0, 0, 0}};  // делитель = вычитаемое
      s21_big_decimal minuend = {0};             // уменьшаемое
      s21_big_decimal subtrahend = {0};  // вычитаемое = делитель
      s21_big_decimal subtraction = {0};  // разность делимого и делителя
      s21_big_decimal tmp_zero = {0};  // число ноль
      int sign_num_result = 0;
      copy_from_big_decimal_to_big_decimal(value, &dividend);
      // определяем знак числа, который пойдёт в итоговое значение
      if (big_get_sign(dividend) == 0) {
        sign_num_result = 0;
      } else {
        sign_num_result = 1;
      }
      big_set_sign_plus(&dividend);
      // проверка того что число меньше 1, но больше либо равно 0
      if (big_is_less(dividend, divisor) == 1 &&
          big_is_greater_or_equal(dividend, tmp_zero) == 1) {
        big_decimal_reset(result);
        big_set_sign(result, sign_num_result);
      } else {
        big_mul_by_ten_change_scale(&divisor, tmp_num_scale);
        copy_from_big_decimal_to_big_decimal(
            divisor, &subtrahend);  // копируем делитель в вычитаемое
        int cur_bit_d = (B_D_NOT_SCALE - 1);  // текущий бит делимого
        int cur_bit_q = (B_D_NOT_SCALE - 1);  // текущий бит частного
        int counter_division = 0;  // счётчик количества действий деления
        while (cur_bit_d >= 0) {
          big_decimal_reset(
              &subtraction);  // Обнуляем структуру, которая содержит результат
                              // вычитания: minuend - subtrahend
          big_shift_left_1_bits(
              &minuend);  // Сдвигаем биты уменьшаемого влево на 1
          big_set_bit(&minuend, 0, big_get_bit(dividend, cur_bit_d));
          big_subtraction_action(minuend, subtrahend, &subtraction);
          if (counter_division == 0) {
            if (big_is_greater_or_equal(subtraction, tmp_zero) == 1) {
              big_set_bit(result, cur_bit_q, 1);
              big_decimal_reset(&minuend);  // очищаем структуру уменьшаемого
              copy_from_big_decimal_to_big_decimal(
                  subtraction, &minuend);  // копируем разность в уменьшаемое
              counter_division++;
              cur_bit_q--;
            }
          } else {
            if (big_is_greater(subtraction, tmp_zero) ==
                1) {  // если разность больше 0
              big_set_bit(result, cur_bit_q,
                          1);  // устанавливаем в текущий бит частного 1
              big_decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
              copy_from_big_decimal_to_big_decimal(subtraction, &minuend);
            } else if (big_is_equal(subtraction, tmp_zero) ==
                       1) {  // если разность равна 0
              big_set_bit(result, cur_bit_q,
                          1);  // устанавливаем в текущий бит частного 1
              big_decimal_reset(&minuend);  // обнуляем структуру уменьшаемого
            } else {  // (big_is_less(subtraction, tmp_zero) == 1) если разность
                      // меньше 0
              big_set_bit(result, cur_bit_q,
                          0);  // устанавливаем в текущий бит частного 0
            }
            counter_division++;
            cur_bit_q--;
          }
          cur_bit_d--;
        }
        for (int i = cur_bit_q; i >= 0; i--) {
          big_shift_right_1_bits(result);
        }
        // устанавливаем знак
        big_set_sign(result, sign_num_result);
      }
    } else {
      copy_from_big_decimal_to_big_decimal(value, result);
    }
  } else {  // иначе это 1 - INF, 2 - N_INF, 3 - NAN
    if (type_first_num == 1) {
      big_set_number_is_infinity(result, 0);
      flag_result = 1;
    }
    if (type_first_num == 2) {
      big_set_number_is_infinity(result, 1);
      flag_result = 2;
    }
    if (type_first_num == 3) {
      big_set_number_is_not_a_number(result, 1);
      flag_result = 3;
    }
  }
  return flag_result;
}
