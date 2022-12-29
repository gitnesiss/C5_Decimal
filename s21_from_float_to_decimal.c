#include "s21_decimal.h"

typedef struct {
  int sign;
  int mantissa;
  int scale;
} consruct;

void s21_components_float(consruct *float_src, float src);
void s21_double_src(consruct *float_src, double *number);
void s21_normalize_float(double number, int *decimal_src, int *decimal_scale);

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  decimal_reset(dst);
  int flag_result = 0;
  consruct float_src = {0};
  s21_big_decimal temp_dst = {0};
  s21_big_decimal tmp_num_one = {{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}};  // число один
  s21_big_decimal tmp_num_ten = {
      {10, 0, 0, 0, 0, 0, 0, 0, 0, 0}};  // число десять
  s21_big_decimal tmp_num_five = {
      {5, 0, 0, 0, 0, 0, 0, 0, 0}};  // граничное число
  s21_big_decimal remaind = {0};
  s21_big_decimal temp_result = {0};
  if (src == 0) {
    decimal_reset(dst);
  } else {
    s21_components_float(
        &float_src, src);  // Функция вернет ошибку (1) если скейл будет больше
                           // 95, так как децимал вмещает 2 в 95 степени
    if (float_src.scale > 95 || float_src.scale < -95) {
      flag_result = 1;
    } else {
      double number = 0;
      s21_double_src(&float_src, &number);
      int decimal_src = 0;
      int decimal_scale = 0;
      s21_normalize_float(number, &decimal_src, &decimal_scale);
      s21_from_int_to_decimal(decimal_src, dst);
      copy_from_decimal_to_big_decimal(*dst, &temp_dst);
      if (decimal_scale < 0) {
        mul_by_ten_change_scale(dst, (-decimal_scale));
      } else {
        while (decimal_scale > 28) {
          big_decimal_reset(&remaind);
          big_division_action(temp_dst, tmp_num_ten, &remaind,
                              1);  // выделяем остаток
          big_division_action(temp_dst, tmp_num_ten, &temp_result,
                              0);  // выделяем целую часть
          copy_from_big_decimal_to_big_decimal(temp_result, &temp_dst);
          decimal_scale--;  // уменьшаем scale
          if (big_is_greater_or_equal(remaind, tmp_num_five)) {
            big_addition_action(temp_dst, tmp_num_one, &temp_result);
            copy_from_big_decimal_to_big_decimal(temp_result, &temp_dst);
          }
        }
        copy_from_big_decimal_to_decimal(temp_dst, dst);
        set_scale(dst, decimal_scale);
      }
      if (float_src.sign == 1) {
        set_sign_minus(dst);
      }
    }
  }
  return flag_result;
}

//  Функция разбора флоата на знак, мантиссу, скейл
void s21_components_float(consruct *float_src, float src) {
  unsigned int fbits =
      *((unsigned *)&src);  //  Записываем побитовое значение флоат в инт
  //  Записываем значения битов в float_src->sign, float_src->scale,
  //  float_src->mantissa
  float_src->sign =
      (fbits & 0x80000000) >> 31;  //  Первый бит во флоате это знак, 0 - плюс,
                                   //  1 - минус, записываем в float_src->sign
  float_src->scale = (fbits & 2139095040) >>
                     23;  //  01111111100000000000000000000000 С 2 по 9 биты во
                          //  флоате это скейл, записываем в float_src->scale
  float_src->mantissa =
      fbits & 8388607;  //  00000000011111111111111111111111 С 10 по 32 биты во
                        //  флоате это скейл, записываем в float_src->mantissa
  float_src->scale -= 127;  //  Вычисляем чистый скейл, вычитаем 127, так как
                            //  скейл в памяти записывается на 127 больше
}

//  Функция расчета числа типа double из полученных мантисы и скейла входящего
//  числа float
void s21_double_src(consruct *float_src, double *number) {
  *number = pow(2, float_src->scale);
  for (unsigned int mask = 4194304, i = 0; mask;
       mask >>= 1, i++) {  //  mask = 10000000000000000000000
    int asd = (float_src->scale - 1 - i);
    if (!!(float_src->mantissa & mask)) {
      *number += pow(2, asd);
    }
  }
}

void s21_normalize_float(double number, int *decimal_src, int *decimal_scale) {
  while (number > 1e7) {
    number /= 10;
    *decimal_scale -= 1;
  }
  while (number < 1e6) {
    number *= 10;
    *decimal_scale += 1;
  }
  number = floor(number + 0.5);
  while (fmod(number, 10.0) == 0) {
    number /= 10;
    *decimal_scale -= 1;
  }
  *decimal_src = number;
}
