CC                =gcc
FLAGS             =-Wall -Wextra -Werror -std=c11
DEBUG_FLAGS       =$(CFLAGS) -g
CLAGS             =$(CFLAGS) -c # скомпилировать и собрать, но не линковать.
HEAD              =s21_decimal.h
HEADERS           =$(wildcard *.h)
SRC               =$(wildcard *.c)
OBJECTS           =$(patsubst %.c, %.o, $(SRC))
SANIT             =# -g3 -fsanitize=address
BRANCH_INFO       =--rc lcov_branch_coverage=1
REMOVE_DIRECTIVES =clean_mode=1
GCOV_FLAGS        =--coverage # == -fprofile-arcs -ftest-coverage 
OS                =$(shell uname)
FILE_TEST_SRC     =tests.c
FILE_TEST         =tests.out
GOAL              =s21_decimal.a

#-------- V Распознование системы и выбора флагов

ifeq ("$(OS)", "Linux")
	COMP_FLAGS_LINUX=-lm -lrt -lsubunit -lpthread -g
    TEST_FLAGS=$(shell pkg-config --libs check) #  -lcheck -lm -lsubunit -lpthread -lrt -g
    LEAKS_RUN=valgrind ./test
else
    TEST_FLAGS=-lcheck
    LEAKS_RUN=leaks -atExit -- ./test
endif

#-------- ^ Распознование системы и выбора флагов

all: s21_decimal.a 

# ar - программа (сокр. от archiver - архиватор) для создания статических библиотек. Она используется
#           для создания, модификации и просмотра объектных файлов в статических библиотеках, которые
#           в действительности представляют из себя простые архивы.
# c - заставляет создавать (от create) библиотеку, если ее нет.
# r - (от replace) заменяет старые объектные файлы новыми версиями.
# Программа ranlib добавит индекс к архиву и получится полноценная статическая библиотека объектных файлов.
# $@ - подставляет название цели
# $< - подставляет зависимости (разобрать отличие от $^)
# $^ - подставляет зависимости (разобрать отличие от $<)
# s21_decimal.a:
# 	$(CC) -c $(SRC)
# 	ar rcs $@ *.o
# 	ranlib $@

# s21_decimal.a:
# 	gcc -c *.c
# 	ar rcs $@ *.o
# 	ranlib $@

s21_decimal.a:
	gcc -c *.c $(COMP_FLAGS_LINUX)
	ar rcs $@ *.o
	ranlib $@

#---------------------------------------- V Тестирование V ----------------------------------------

# Создание файла tests.c для модульного тестирования на языке C с использованием библиотеки check.h.
# clean_mode=1 - checkmk не размещает соответствующие директивы #line в исходном коде.
$(FILE_TEST_SRC):
	@#checkmk tests > $(FILE_TEST_SRC)
	checkmk $(REMOVE_DIRECTIVES) tests > $(FILE_TEST_SRC)

# # тест для стандартной сборки
# test: $(FILE_TEST_SRC) s21*.o s21_decimal.a
# 	$(CC) $(FLAGS) $(SANIT) $(GCOV_FLAGS) $(FILE_TEST_SRC) s21*.c $(TEST_FLAGS) -o $(FILE_TEST)
# 	./$(FILE_TEST)

# тест для стандартной сборки
test: $(FILE_TEST_SRC) s21_decimal.a #s21*.o 
	$(CC) $(FLAGS) $(SANIT) $(GCOV_FLAGS) $(FILE_TEST_SRC) s21*.c $(TEST_FLAGS) -o $(FILE_TEST)
	./$(FILE_TEST)

# --coverage == -fprofile-arcs -ftest-coverage (при компиляции) и -lgcov (при связывании)
#         синонимы
# --coverage - создаёт файл *.gcno, который содержит базовую информацию для запуска Code Coverage
# теперь надо запустить программу (Win: s21_decimal.exe, в Unix: ./s21_decimal.out).
# После завершения программы, создаётся файл *.gcda, который содержит информацию о фактическом покрытии кода
# -r . - где искать исходные файлы с информацией о фактическом покрытии кода 
gcov_report: test
	gcovr -r . --html-details -o test_report_gcov.html
	open test_report_gcov.html

# Составление отчёта о Code Coverage с помощью lcov:
# lcov -t "prog000_Proba004.out" -o test.info -c -d . --rc lcov_branch_coverage=1
# -t <name> - устанавливает имя отчёта, при измерении покрытия кода тестами можно указать имя теста
#			 или набора тестов
# -o <name> - устанавливает имя выходного файла с промежуточной информацией
# -c        - указывает, что lcov должен использовать существующие данные о coverage
# -d <path> - устанавливает каталог, в котором надо искать данные о coverage, и мы передаём текущий каталог “.”
# --rc lcov_branch_coverage=1 - получения отчёта с информацией о выборе ветвлений
# Генерация отчёта в виде HTML-страницы:
# genhtml -o report fizzbuzz.info --rc lcov_branch_coverage=1
# -o <path/nameDir> - имя каталога куда будет сложен сформированный html-отчёт
# отчёт lcov для стандартной сборки
lcov_report: test
	lcov -t "$(FILE_TEST)" -o test_report_lcov.info -c -d . $(BRANCH_INFO)
	genhtml -o report_lcov test_report_lcov.info $(BRANCH_INFO)
	open report_lcov/index.html


#---------------------------------------- ^ Тестирование ^ ----------------------------------------

#------------------------------------------ V Очистка V -------------------------------------------

# -f Удалить файлы без подтверждения, независимо от разрешений файла. Если файл не существует, не отображайть
#           диагностическое сообщение и не изменяйте статус выхода, чтобы отразить ошибку. Опция
#           переопределяет все предыдущие опции -i.
# -r Удалить папку без подтверждения.
# -v Выведет имена удаляемых файлов.
# @ в начале команде, не отображает команду в терминале.
clean: clean_lib clean_test clean_coverage
	@rm -f *.out CPPLINT.cfg

clean_lib:
	@rm -f *.o *.a

clean_test:
	@rm -f tests.c s21_tests.c tests.out s21_tests.out
	@rm -f *.gcno *.gcda

clean_coverage:
	@rm -rf report_lcov main.out.dSYM
	@rm -f *.gcov *.html *.css test_report_lcov.info

#------------------------------------------ ^ Очистка ^ -------------------------------------------

#--------------------------------------- V Проверка кода V ----------------------------------------

checks: check

check:
	cp ../materials/linters/.clang-format ./
	clang-format -i *.c
	clang-format -i *.h
	clang-format -n *.c
	clang-format -n *.h
	rm -f .clang-format

# checks: linter leaks check
# Линтер - программа проверяющая код на соответствие стандартам. Стиль и чистота кода.
# cp -f - если существующий файл назначения не может быть открыт, удалить его и повторите попытку.
# linter:
# ifeq ("","$(wildcard ./CPPLINT.py)") # если файла нет, копируем его.
# 	@cp -f ../materials/linters/CPPLINT.cfg ./CPPLINT.cfg
# endif
# 	python3 ../materials/linters/cpplint.py --extensions=c *.c *.h

# Утечки
# #CK_FORK=no leaks -atExit -- <запускаемый файл(./main)> -lhe for <файл с кодом (main.c)>
# leaks:
# 	$(LEAKS_RUN)
# 	CK_FORK=no leaks -atExit -- ./s21_grep -lhe for main.c

valgrind:
	valgrind -s --log-file="valgrind_log_01.txt" --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose  ./tests.out

# Флаги для cppcheck
# -q не показывать отчёты по проверке.
# --enable=all - включаются дополнительные проверки. Идентификаторы:
# * all - включаются все проверки. Рекомендуется использовать --enable=all только при сканировании
#           всей программы, потому что она включает проверки unusedFunction.
# * unusedFunction - проверка неиспользуемых функций. Рекомендуется включать тогда, когда
#           сканируется вся программа.

#--------------------------------------- ^ Проверка кода ^ ----------------------------------------