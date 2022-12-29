#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#define DEC 128
#define D_NOT_SCALE 96
// #define B_D 224
// #define B_D_NOT_SCALE 192
#define B_D 320
#define B_D_NOT_SCALE 288

typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  // unsigned int bits[7];
  unsigned int bits[10];
} s21_big_decimal;

#include <limits.h>
#include <math.h>
#include <stdio.h>

/**
 * @brief Перечисление элементов массива из которых состоит число decimal.
 *
 * @param NORM стандартное число.
 * @param INF бесконечность положительная.
 * @param N_INF бесконечность отрицательная.
 * @param NaN несуществуещее число или чрезмерно малое число (не помещающееся в
 * decimal).
 * @param DOP_CODE
 */
enum { NORM, INF, N_INF, NaN, DOP_CODE };

/**
 * @brief Перечисление элементов массива из которых состоит число decimal.
 *
 * @param LOW элемент массива нижних разрядов числа.
 * @param MID элемент массива средних разрядов числа.
 * @param HIGH элемент массива верхних разрядов числа.
 * @param SCALE элемент массива содержащий иформацию о знаке числа и десятичной
 * степени.
 */
enum { LOW, MID, HIGH, SCALE };

/* -------------------------- Арифметические операторы
 * -------------------------- */

/**
 * @brief Сложение (addition). Сложение двух чисел.
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @param result s21_decimal.
 * @return 0 - OK. 1 - число слишком велико или равно бесконечности.
 * 2 - число слишком мало или равно отрицательной бесконечности. 3 - деление на
 * 0.
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Вычитание (subtraction). Вычитание из 1-ого числа 2-ое.
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @param result s21_decimal.
 * @return 0 - OK. 1 - число слишком велико или равно бесконечности.
 * 2 - число слишком мало или равно отрицательной бесконечности. 3 - деление на
 * 0.
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Умножение (multiplication). Перемножение двух чисел.
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @param result s21_decimal.
 * @return 0 - OK. 1 - число слишком велико или равно бесконечности.
 * 2 - число слишком мало или равно отрицательной бесконечности. 3 - деление на
 * 0.
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Деление (division). Делит 1-ое число на 2-ое.
 *
 * @param value_1 делимое - s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 делитель - s21_decimal - структура в виде {int, int, int,
 * int}.
 * @param result частное - s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - число слишком велико или равно бесконечности.
 * 2 - число слишком мало или равно отрицательной бесконечности. 3 - деление на
 * 0.
 */
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Остаток от деления (modulo). Выделяет остаток из 1-ого числа по
 * основанию 2-ого числа. Уточнение про операцию mod: Если в результате операции
 * произошло переполнение, то необходимо отбросить дробную часть (например,
 * 70,000,000,000,000,000,000,000,000,000 % 0.001 = 0.000).
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @param result s21_decimal.
 * @return 0 - OK. 1 - число слишком велико или равно бесконечности.
 * 2 - число слишком мало или равно отрицательной бесконечности. 3 - деление на
 * 0.
 */
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/* -------------------------- Арифметические операторы
 * -------------------------- */

/* ----------------------------- Операторы сравнения
 * ----------------------------- */

/**
 * @brief Сравнение: меньше (less).
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @return 0 - FALSE. 1 - TRUE.
 */
int s21_is_less(s21_decimal, s21_decimal);

/**
 * @brief Сравнение: меньше или равно (less or equal).
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @return 0 - FALSE. 1 - TRUE.
 */
int s21_is_less_or_equal(s21_decimal, s21_decimal);

/**
 * @brief Сравнение: больше (greater).
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @return 0 - FALSE. 1 - TRUE.
 */
int s21_is_greater(s21_decimal, s21_decimal);

/**
 * @brief Сравнение: больше или равно (greater or equal).
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @return 0 - FALSE. 1 - TRUE.
 */
int s21_is_greater_or_equal(s21_decimal, s21_decimal);

/**
 * @brief Сравнение: равно (equal).
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @return 0 - FALSE. 1 - TRUE.
 */
int s21_is_equal(s21_decimal, s21_decimal);

/**
 * @brief Сравнение: не равно (not equal).
 *
 * @param value_1 s21_decimal.
 * @param value_2 s21_decimal.
 * @return 0 - FALSE. 1 - TRUE.
 */
int s21_is_not_equal(s21_decimal, s21_decimal);

/* ----------------------------- Операторы сравнения
 * ----------------------------- */

/* ------------------------------- Преобразователи
 * ------------------------------- */

/**
 * @brief Функция преобразования числа типа int в тип decimal.
 *
 * @param src - входное целое (int) число.
 * @param dst s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка конвертации.
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst);

/**
 * @brief Функция преобразования числа с плавающей точкой float в тип decimal.
 * Уточнение про преобразование числа типа float:
 * - Если числа слишком малы (0 < |x| < 1e-28), вернуть ошибку и значение,
 * равное 0.
 * - Если числа слишком велики (|x| > 79,228,162,514,264,337,593,543,950,335)
 * или равны бесконечности, вернуть ошибку.
 * - При обработке числа с типом float преобразовывать все содержащиеся в нём
 * цифры.
 *
 * @param src входное число с плавающей точкой (float).
 * @param dst s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка конвертации.
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst);

/**
 * @brief Функция преобразования числа типа decimal в тип int. Уточнение про
 * преобразование из числа типа decimal в тип int:
 * - Если в числе типа decimal есть дробная часть, то её следует отбросить
 * (например, 0.9 преобразуется 0).
 *
 * @param src входное число типа decimal, s21_decimal - структура в виде {int,
 * int, int, int}.
 * @param dst целое число типа int.
 * @return 0 - OK. 1 - ошибка конвертации.
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst);

/**
 * @brief Функция преобразования числа типа decimal в тип float.
 *
 * @param src входное число типа decimal, s21_decimal - структура в виде {int,
 * int, int, int}.
 * @param dst число типа float.
 * @return 0 - OK. 1 - ошибка конвертации.
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst);

/* ------------------------------- Преобразователи
 * ------------------------------- */

/* ------------------------------- Другие функции
 * ------------------------------- */

/*
Округляет указанное Decimal число до ближайшего целого числа в сторону
отрицательной бесконечности. 0 - OK. 1 - ошибка вычисления.
*/
int s21_floor(s21_decimal value, s21_decimal *result);

/*
Округляет Decimal до ближайшего целого числа.
0 - OK.
1 - ошибка вычисления.
*/
int s21_round(s21_decimal value, s21_decimal *result);

/*
Возвращает целую часть числа decimal.
Любые дробные цифры отбрасываются, включая конечные нули.
0 - OK.
1 - ошибка вычисления.
Например: float = 142,23 ==s21_truncate()=> int 142.
*/
int s21_truncate(s21_decimal value, s21_decimal *result);

/*
Инвертирование числа. Возвращает результат умножения указанного Decimal на -1.
0 - OK.
1 - ошибка вычисления.
*/
int s21_negate(s21_decimal value, s21_decimal *result);

/* ------------------------------- Другие функции
 * ------------------------------- */

/* -------------------------------- Свои функции
 * -------------------------------- */

/**
 * @brief Функция проверяет число decimal на корректность по степени и знаку.
 * Проверка осуществляется с помощью прохода по битам num.bits[3] при этом
 * проверяются биты с 0(96) по 15(112) включительно и с 21(118) по 30(126) на
 * равенство 0 и отдельно проверяется степень, которая должна быть в диапазоне
 * >=0 и <=28.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - число некорректное (неверно записаны степень и знак).
 */
int number_is_correct(s21_decimal *num);

/**
 * @brief Функция выводит на экран число типа decimal в двоичном виде. Принимает
 * на вход структуру с массивом из 4-ёх unsigned int элементов.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 */
void print_decimal_to_binary(s21_decimal num);

/**
 * @brief Функция выводит на экран любое значение в двоичном виде. Пример:
 * print_binary(sizeof(a), &a). Принимает на вход размер типа переменной и адрес
 * самой переменной.
 *
 * @param size size_t.
 * @param ptr указатель.
 */
void print_binary(size_t const size, void const *const ptr);

/**
 * @brief Функция возвращает значение бита, указанного индекса.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @param index int - номер бита от 0 до 127, в который устанавливается value.
 * @return значение бита из указанного индекса.
 */
int get_bit(s21_decimal num, int index);

/**
 * @brief Функция устанавливает значение бита в выбранный индекс.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @param index int - номер бита от 0 до 127, в который устанавливается value.
 * @param value int - (имеет только два значения 0 = "+" или 1 = "-").
 * @return 0 - OK. 1 - ошибка установки бита.
 */
int set_bit(s21_decimal *num, int index, int value);

/**
 * @brief Функция возвращает значение равное степени числа decimal.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при очистке знака.
 */
int get_scale(s21_decimal num);

/**
 * @brief Функция устанавливает значение степени для числа decimal.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @param pow_dec степень числа, которую следует установить.
 * @return 0 - OK. 1 - ошибка при установке значения степени.
 */
int set_scale(s21_decimal *num, int pow_dec);

/**
 * @brief Функция возвращает бит, отвечающий за знак (0 = "+" или 1 = "-") числа
 * decimal.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - при уловии, что бит указывающий на знак равен 0 ("+") или 1 -
 * если бит указывающий на знак, равен 1 ("-").
 */
int get_sign(s21_decimal num);

/**
 * @brief Функция устанавливает бит (указанный в значение value), отвечающий за
 * знак (0 = "+" или 1 = "-") числа decimal.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @param value_sign int - 0 = "+" или 1 = "-".
 * @return 0 - OK. 1 - ошибка при установке знака.
 */
int set_sign(s21_decimal *num, int value_sign);

/**
 * @brief Функция устанавливает бит (1 = "-") в бит, отвечающий за знак числа
 * decimal.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка установки флага.
 */
int set_sign_minus(s21_decimal *num);

/**
 * @brief Функция устанавливает бит (0 = "+") в бит, отвечающий за знак числа
 * decimal.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при очистке знака.
 */
int set_sign_plus(s21_decimal *num);

/**
 * @brief Функция изменяет знак числа decimal, заменяет исходный бит знака на
 * противоположный, тем самым меняет знак числа decimal.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при замене знака.
 */
int change_sign(s21_decimal *num);

/**
 * @brief Функция определяет итоговый знак числа из value_1 и value_2.
 *
 * @param value_1 s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @return значение 0 - "+" или 1 - "-".
 */
int final_sign(s21_decimal value_1, s21_decimal value_2);

/**
 * @brief Функция обнуляет все биты числа decimal. s21_decimal num = {0};
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при обнулении структуры.
 */
int decimal_reset(s21_decimal *num);

/**
 * @brief Функция проверяет является ли второе число равным, но противоположным
 * по знаку первому
 *
 * @param value_1 s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - FALSE. 1 - TRUE.
 */
int opposite_num(s21_decimal value_1, s21_decimal value_2);

/**
 * @brief Функция нормализации - выравнивание разрядностей десятичной дроби,
 * чтобы в дальнейшем можно было производить с числами различные действия. (При
 * нормализации происходит умножение "мантиссы" на 10 и увеличение степени на 1,
 * таким образом начальное значение числа не изменяется.)
 *
 * @param value_1 s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при нормализации.
 */
int decimal_normalization(s21_decimal *value_1, s21_decimal *value_2);

/**
 * @brief Функция для выравнивания степеней между числами value_1 и value_2.
 *
 * @param value_1 s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при обнулении структуры.
 */
int alignment_of_powers_of_decimal(s21_decimal *value_1, s21_decimal *value_2);

/**
 * @brief Функция для выделения дробной части числа типа s21_decimal. Например:
 * decimal = 142,23 => s21_efpond() => int 23. (efpond - Extract Fractional Part
 * Of Number Decimal)
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @param result s21_decimal - структура в виде {int, int, int, int}. Результат
 * работы функции - число без десятичной дроби.
 * @return 0 - OK. 1 - ошибка при обнулении структуры.
 */
int efpond(s21_decimal num, s21_decimal *result);

/**
 * @brief Функция удалаяет лишние нули на конце числа и соответствующе меняет
 * степень числа. Например: 457,2300 (exp = 4) => 457,23 (exp = 2).
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при обнулении структуры.
 */
int big_remove_extra_zero(s21_big_decimal *num);

/**
 * @brief Функция проверяет какой тип значения в структуре числа decimal: 0 -
 * нормальное число, 1 - +INF, 2 - -INF, 3 - NAN
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - нормальное число. 1 - +INF. 2 - -INF. 3 - NAN.
 */
int value_type(s21_decimal num);

/**
 * @brief Функция проверяет в числе decimal лежит число равное 0-ю или нет.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - FALSE. 1 - TRUE.
 */
int num_is_zero(s21_decimal value);

/**
 * @brief Функция проверяет, что это число является +INF.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - FALSE. 1 - TRUE.
 */
int num_is_inf(s21_decimal value);

/**
 * @brief Функция проверяет, что это число является -INF.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - FALSE. 1 - TRUE.
 */
int num_is_n_inf(s21_decimal value);

/**
 * @brief Функция проверяет, что это число является NAN.
 *
 * @param num s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - FALSE. 1 - TRUE.
 */
int num_is_nan(s21_decimal value);

/**
 * @brief Функция устанавливает число бесконечность. В зависимости от второго
 * аргумента будет -INF или +INF.
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @param sign int - знак у числа бесконечности: 0 - "+", 1 - "-".
 * @return 0 - OK. 1 - ошибка при установлении числа бесконечность.
 */
int set_number_is_infinity(s21_decimal *value, int sign);

/**
 * @brief Функция устанавливает число NaN. В зависимости от второго аргумента
 * будет -NaN или +NaN.
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @param sign int - знак у числа NaN: 0 - "+", 1 - "-".
 * @return 0 - OK. 1 - ошибка при установлении числа бесконечность.
 */
int set_number_is_not_a_number(s21_decimal *value, int sign);

/**
 * @brief Функция сдвигает все биты числа decimal влево на 1. << 1.
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при попытке сдвига структуры влево на 1 бит.
 */
int shift_left_1_bits(s21_decimal *value);

/**
 * @brief Функция сдвигает все биты числа decimal влево на 3. << 3.
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при попытке сдвига структуры влево на 3 бита.
 */
int shift_left_3_bits(s21_decimal *value);

/**
 * @brief Функция сдвигает все биты числа decimal вправо на 1. >> 1.
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при попытке сдвига структуры вправо на 1 бит.
 */
int shift_right_1_bits(s21_decimal *value);

/**
 * @brief Функция сдвигает все биты числа decimal вправо на 3. >> 3.
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при попытке сдвига структуры вправо на 3 бита.
 */
int shift_right_3_bits(s21_decimal *value);

/**
 * @brief Функция умножения числа decimal на 10^(how_many_times). При этом
 * меняется степень числа (в большую сторону) и "мантисса", но значение не
 * меняется: 15,3 (степень 1) * 10^2 => 15,300 (степень 3).
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @param how_many_times значение степени.
 * @return 0 - OK. 1 - превышение значения +INF. 2 - превышение значения -INF.
 */
int mul_by_ten(s21_decimal *value, int how_many_times);

/**
 * @brief Функция умножения числа decimal на 10^(how_many_times). При этом
 * меняются степень (в меньшую сторону до нуля), "мантисса" (если степень равна
 * 0) и значение числа: 15,3 (степень 1) * 10^2 => 1530 (степень 0)
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @param how_many_times значение степени.
 * @return 0 - OK. 1 - ошибка при умножении числа s21_decimal на
 * 10^(how_many_times).
 */
int mul_by_ten_change_scale(s21_decimal *value, int how_many_times);

/**
 * @brief Функция деления числа decimal на 10^(how_many_times).
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @param how_many_times значение степени.
 * @return 0 - OK. 1 - ошибка при делении числа s21_decimal на
 * 10^(how_many_times).
 */
int div_by_ten(s21_decimal *value, int how_many_times);

int subtraction_action(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result);

int big_subtraction_action(s21_big_decimal value_1, s21_big_decimal value_2,
                           s21_big_decimal *result);

int big_addition_action(s21_big_decimal value_1, s21_big_decimal value_2,
                        s21_big_decimal *result);

/**
 * @brief Функция для конвертирования числа типа s21_big_decimal в число типа
 * s21_decimal. Конвертирование происходит с использованием банковского
 * округления.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при нормализации.
 */
int convert_big_decimal_to_decimal(s21_big_decimal value, s21_decimal *result);

/**
 * @brief Функция нормализации - выравнивание разрядностей десятичной дроби,
 * чтобы в дальнейшем можно было производить с числами различные действия. (При
 * нормализации происходит умножение "мантиссы" на 10 и увеличение степени на 1,
 * таким образом начальное значение числа не изменяется.) Эта функция
 * применяется при математических действиях (сложение, вычитание, деление,
 * умножение, остатка от деления). 0-all (кроме 2, 4, 5). 1-add. 2-sub. 3-mul.
 * 4-div. 5-mod.
 *
 * @param value_1 s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @param big_value_1 s21_big_decimal - структура в виде {int, int, int, int,
 * int, int, int}.
 * @param big_value_2 s21_big_decimal - структура в виде {int, int, int, int,
 * int, int, int}.
 * @param math_action int - математическое действие (0-all (кроме 2, 4, 5).
 * 1-add. 2-sub. 3-mul. 4-div. 5-mod.).
 * @return 0 - нормализация не прошла. 1 - ОК. Нормализация прошла успешно.
 */
int norm_from_decimal_to_big_decimal(s21_decimal value_1, s21_decimal value_2,
                                     s21_big_decimal *big_value_1,
                                     s21_big_decimal *big_value_2,
                                     int math_action);

/**
 * @brief Функция нормализации - выравнивание разрядностей десятичной дроби,
 * чтобы в дальнейшем можно было производить с числами различные действия. (При
 * нормализации происходит умножение "мантиссы" на 10 и увеличение степени на 1,
 * таким образом начальное значение числа не изменяется.) Эта функция
 * применяется при математических действиях (сложение, вычитание, деление,
 * умножение, остатка от деления). 0-all (кроме 2, 4, 5). 1-add. 2-sub. 3-mul.
 * 4-div. 5-mod.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param value_2 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param math_action int - математическое действие (0-all (кроме 2, 4, 5).
 * 1-add. 2-sub. 3-mul. 4-div. 5-mod.).
 * @return 0 - нормализация не прошла. 1 - ОК. Нормализация прошла успешно.
 */
int norm_big_decimal(s21_big_decimal *value_1, s21_big_decimal *value_2,
                     int math_action);

/**
 * @brief Функция выводит на экран число типа s21_big_decimal в двоичном виде.
 * Принимает на вход структуру с массивом из 7-и unsigned int элементов.
 *
 * @param num s21_big_decimal - структура в виде {int, int, int, int, int, int,
 * int}.
 */
void print_big_decimal_to_binary(s21_big_decimal num);

int big_get_bit(s21_big_decimal num, int index);

int big_set_bit(s21_big_decimal *num, int index, int value);

int big_get_scale(s21_big_decimal num);

int big_set_scale(s21_big_decimal *num, int pow_dec);

int big_get_sign(s21_big_decimal num);

int big_set_sign(s21_big_decimal *num, int value_sign);

int big_set_sign_minus(s21_big_decimal *num);

int big_set_sign_plus(s21_big_decimal *num);

int big_change_sign(s21_big_decimal *num);

int big_decimal_reset(s21_big_decimal *num);

int big_shift_left_1_bits(s21_big_decimal *value);

int big_shift_left_3_bits(s21_big_decimal *value);

int big_shift_right_1_bits(s21_big_decimal *value);

int big_shift_right_3_bits(s21_big_decimal *value);

/**
 * @brief Функция копирует число типа s21_decimal value_1 в число типа
 * s21_decimal value_2.
 *
 * @param value_1 s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при попытке копирования.
 */
int copy_from_decimal_to_decimal(s21_decimal value_1, s21_decimal *value_2);

/**
 * @brief Функция копирует число типа s21_decimal value_1 в число типа
 * s21_big_decimal value_2.
 *
 * @param value_1 s21_decimal - структура в виде {int, int, int, int}.
 * @param value_2 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @return 0 - OK. 1 - ошибка при попытке копирования.
 */
int copy_from_decimal_to_big_decimal(s21_decimal value_1,
                                     s21_big_decimal *value_2);

/**
 * @brief Функция копирует число типа s21_big_decimal value_1 в число типа
 * s21_decimal value_2.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param value_2 s21_decimal - структура в виде {int, int, int, int}.
 * @return 0 - OK. 1 - ошибка при попытке копирования.
 */
int copy_from_big_decimal_to_decimal(s21_big_decimal value_1,
                                     s21_decimal *value_2);

/**
 * @brief Функция копирует число типа s21_big_decimal value_1 в число типа
 * s21_big_decimal value_2.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param value_2 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @return 0 - OK. 1 - ошибка при попытке копирования.
 */
int copy_from_big_decimal_to_big_decimal(s21_big_decimal value_1,
                                         s21_big_decimal *value_2);

/**
 * @brief Функция умножения числа big_decimal на 10^(how_many_times). При этом
 * меняется степень числа (в большую сторону) и "мантисса", но значение не
 * меняется: 15,3 (степень 1) * 10^2 => 15,300 (степень 3).
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @param how_many_times значение степени.
 * @return 0 - OK. 1 - превышение значения +INF. 2 - превышение значения -INF.
 */
int big_mul_by_ten(s21_big_decimal *value, int how_many_times);

/**
 * @brief Функция умножения числа big_decimal на 10^(how_many_times). При этом
 * меняются степень (в меньшую сторону до нуля), "мантисса" (если степень равна
 * 0) и значение числа: 15,3 (степень 1) * 10^2 => 1530 (степень 0)
 *
 * @param value s21_decimal - структура в виде {int, int, int, int}.
 * @param how_many_times значение степени.
 * @return 0 - OK. 1 - ошибка при умножении числа s21_decimal на
 * 10^(how_many_times).
 */
int big_mul_by_ten_change_scale(s21_big_decimal *value, int how_many_times);

int big_set_number_is_infinity(s21_big_decimal *value, int sign);

int big_set_number_is_not_a_number(s21_big_decimal *value, int sign);

int big_value_type(s21_big_decimal value);

int big_num_is_zero(s21_big_decimal value);

int big_num_is_inf(s21_big_decimal value);

int big_num_is_n_inf(s21_big_decimal value);

int big_num_is_nan(s21_big_decimal value);

int big_is_greater(s21_big_decimal value_1, s21_big_decimal value_2);

int big_is_less(s21_big_decimal value_1, s21_big_decimal value_2);

int big_is_equal(s21_big_decimal value_1, s21_big_decimal value_2);

int big_is_greater_or_equal(s21_big_decimal value_1, s21_big_decimal value_2);

int big_is_less_or_equal(s21_big_decimal value_1, s21_big_decimal value_2);

/**
 * @brief Функция деления. Производится деления первого числа на второе, в
 * результат уходит только целая часть числа.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param value_2 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param result s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @return 0 - OK. 1 - ошибка при нормализации.
 */
int big_div_result_int(s21_big_decimal value_1, s21_big_decimal value_2,
                       s21_big_decimal *result);

int big_mod(s21_big_decimal value_1, s21_big_decimal value_2,
            s21_big_decimal *result);

/**
 * @brief Функция возвращает целую часть числа типа s21_big_decimal. дробная
 * часть отбрасывается.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param result s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @return 0 - OK. 1 - ошибка при выделении целой части числа.
 */
int big_truncate(s21_big_decimal value, s21_big_decimal *result);

/**
 * @brief Функция действия деления. В зависимости от выбранной опции
 * kind_of_num: 0 - в результат запишится целая часть числа. 1 - остаток от
 * деления.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param value_2 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param result s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @param kind_of_num int - вид числа, который нужно получить (0 - в результат
 * запишится целая часть числа. 1 - остаток от деления.).
 * @return 0 - OK. 1 - ошибка при делении.
 */
int big_division_action(s21_big_decimal value_1, s21_big_decimal value_2,
                        s21_big_decimal *result, int kind_of_num);

int big_mul(s21_big_decimal value_1, s21_big_decimal value_2,
            s21_big_decimal *result);

/**
 * @brief Функция определяет количество разрядов в мантисе.
 *
 * @param value_1 s21_big_decimal - структура в виде {int, int, int, int, int,
 * int, int}.
 * @return int - значение, содержащее количество разрядов в мантисе.
 */
int big_digits_in_mantis(s21_big_decimal value);

int find_first_significant_bit(s21_decimal value);

int big_find_first_significant_bit(s21_big_decimal value);

int big_bank_rounding(s21_big_decimal *value, int scale_to_round);

int big_div_by_ten(s21_big_decimal *value, int how_many_times);

int big_div_by_ten_change_scale(s21_big_decimal *value, int how_many_times);

int div_by_ten_change_scale(s21_decimal *value, int how_many_times);

/* -------------------------------- Свои функции
 * -------------------------------- */

#endif /* SRC_S21_DECIMAL_H_ */
