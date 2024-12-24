#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define MAX 20

typedef struct {
    float temp;
    float vlazh;
    int in_norm; /*Флаг состояния датчика (1 - в норме, 0 - вне нормы)*/
} Sensor;

typedef struct {
    float tempmin;
    float tempmax;
    float vlazhmin;
    float vlazhmax;
    int season_choice;
    int work_choice;
} Znach;

/*Объявление функций*/
void analyze_data(Sensor* sensors, int sensor_count, Znach* pole, int filter);
int new_sensors(int* sensor_count, Sensor* sensors, const char* filename);
int read(const char* filename, Sensor* sensors, int* sensor_count);
int limit(Znach* pole);
void generate(const char* filename, int count);
void analyze_min_max(Sensor* sensors, int sensor_count, const char* output_filename);

int main() {
    system("chcp 1251");
    puts("____________________________________________________________________________________");
    printf("| Вас приветствует программа Analysis! Я помогу вам в мониторинге данных с датчиков.|\n");
    puts("|___________________________________________________________________________________|");
    puts("*Также перед началом работы Вы можете протестрировать её на сгенерированных значениях\n");
    Sensor sensors[MAX];
    int sensor_count = 0;
    Znach pole = {0}; 
    int choice;
    printf("Выберите сезон:\n1 - Холодный\n2 - Тёплый\n> ");
    scanf("%d", &pole.season_choice);
    printf("Выберите степень работы:\n1 - Работа А\n2 - Работа Б\n> ");
    scanf("%d", &pole.work_choice);
    limit(&pole);  /*Установка пределов для температуры и влажности*/
    do {
        printf("\nВыберите опцию:\n");
        printf("1. Мониторинг данных с датчиков\n");
        printf("2. Генерация данных и мониторинг\n");
        printf("3. Выход\n");
        printf("> ");
        scanf("%d", &choice);
        switch (choice) {
        case 1: /* Мониторинг данных из файла text.txt*/
            if (read("text.txt", sensors, &sensor_count) != 0) {
                printf("Ошибка чтения данных из файла text.txt.\n");
                break;
            }
            printf("Выберите фильтр для отображения:\n");
            printf("1. Датчики в норме\n");
            printf("2. Датчики вне нормы\n");
            printf("3. Все датчики\n");
            printf("4. Анализ максимальных и минимальных значений\n");
            printf("> ");
            int filter;
            scanf("%d", &filter);
            if (filter == 4) {
                analyze_min_max(sensors, sensor_count, "analis.txt"); /*Анализ максимальных и минимальных значений*/
                break; 
            }
            analyze_data(sensors, sensor_count, &pole, filter);
            new_sensors(&sensor_count, sensors, "text.txt");
            break;
        case 2: /*Генерация данных и мониторинг из файла test.txt*/
            printf("Введите количество датчиков для генерации (не больше 20): ");
            int sensor_count_gen;
            scanf("%d", &sensor_count_gen);
            if (sensor_count_gen > MAX) {
                printf("Количество датчиков превышает максимум (%d). Будет сгенерировано %d датчиков.\n", MAX, MAX);
                sensor_count_gen = MAX;
            }
            generate("test.txt", sensor_count_gen);
            printf("Данные успешно сгенерированы .\n");
            if (read("test.txt",  sensors, &sensor_count) != 0) {
                printf("Ошибка чтения данных из файла 'test.txt'.\n");
                break;
            }
            analyze_data(sensors, sensor_count, &pole, 3);
            break;
        case 3: // Выход
            printf("Выход из программы.\n");
            break;
        default:
            printf("Неверный выбор. Попробуйте снова.\n");
            break;
        }
    } while (choice != 3);
    return 0;
}

/* Анализирует данные с датчиков и выводит информацию об их состоянии. При необходимости указывает отклонения
* Sensor* sensors — указатель на массив структур Sensor, содержащих данные датчиков
* int sensor_count — количество датчиков
* Znach* pole — указатель на структуру типа Znach
* int filter — фильтр для отображения данных
* Выводит на экран: состояние каждого датчика (в норме или вне нормы), отклонения температуры или влажности (если есть)
*/
void analyze_data(Sensor* sensors, int sensor_count, Znach* pole, int filter) {
    for (int i = 0; i < sensor_count; i++) {
        int in_norm = (sensors[i].temp >= pole->tempmin && sensors[i].temp <= pole->tempmax &&
            sensors[i].vlazh >= pole->vlazhmin && sensors[i].vlazh <= pole->vlazhmax); /*проверка, находятся ли значения температуры и влажности в заданных диапазонах*/
        float temp_otkl = 0.0, vlazh_otkl = 0.0;
        /*Рассчитывание отклонений*/
        if (sensors[i].temp < pole->tempmin) {
            temp_otkl = pole->tempmin - sensors[i].temp; /*ниже нормы*/
        }
        else if (sensors[i].temp > pole->tempmax) {
            temp_otkl = sensors[i].temp - pole->tempmax; /*выше нормы*/
        }
        if (sensors[i].vlazh < pole->vlazhmin) {
            vlazh_otkl = pole->vlazhmin - sensors[i].vlazh; /*ниже нормы*/
        }
        else if (sensors[i].vlazh > pole->vlazhmax) {
            vlazh_otkl = sensors[i].vlazh - pole->vlazhmax; /*выше нормы*/
        }
        /*Фильтруем вывод*/
        if ((filter == 1 && in_norm) || (filter == 2 && !in_norm) || filter == 3) {
            printf("Датчик %d: Температура = %.2f, Влажность = %.2f\n", i + 1, sensors[i].temp, sensors[i].vlazh);
            if (!in_norm) {
                if (temp_otkl != 0.0) {
                    if (sensors[i].temp < pole->tempmin) {
                        printf("  Отклонение температуры: %.2f ниже минимума. Необходимо повысить температуру.\n", temp_otkl);
                    }
                    else {
                        printf("  Отклонение температуры: %.2f выше максимума. Необходимо понизить температуру.\n", temp_otkl);
                    }
                }
                if (vlazh_otkl != 0.0) {
                    if (sensors[i].vlazh < pole->vlazhmin) {
                        printf("  Отклонение влажности: %.2f ниже минимума. Необходимо повысить влажность.\n", vlazh_otkl);
                    }
                    else {
                        printf("  Отклонение влажности: %.2f выше максимума. Необходимо понизить влажность.\n", vlazh_otkl);
                    }
                }
            }
            else {
                printf("  В пределах нормы\n");
            }
        }
    }
}


/* Добавляет недостающие данные и записывает их в файл
* int* sensor_count — указатель на количество датчиков
* Sensor* sensors — указатель на массив структур Sensor, в который записываются недостающие данные
* const char* filename — имя файла, в который будут записываться данные
* Возвращает 1 при успешном выполнении, 0 при наличии ошибки
*/
int new_sensors(int* sensor_count, Sensor* sensors, const char* filename) {
    FILE* file = fopen(filename, "a"); /* Открываем файл для добавления значений*/
    if (file == NULL) {
        printf("Ошибка открытия файла для записи.\n");
        return 1;
    }
    for (int i = 0; i < *sensor_count; i++) {
        /*Если температуры нет*/
        if (sensors[i].temp == -1) {
            printf("Введите температуру для датчика %d: ", i + 1);
            scanf("%f", &sensors[i].temp);
            fprintf(file, "%.2f^\n", sensors[i].temp); /*Дописывание в файл*/
            printf("Недостающие данные успешно записаны в файл. Выберете опцию снова\n");
        }
        /*Если влажности нет*/
        if (sensors[i].vlazh == -1) {
            printf("Введите влажность для датчика %d: ", i + 1);
            scanf("%f", &sensors[i].vlazh);
            fprintf(file, "%.2f%%\n", sensors[i].vlazh); /*Дописывание в файл*/
            printf("Недостающие данные успешно записаны в файл. Выберете опцию снова\n");
        }
    }
    fclose(file);
    return 0;
}

/* Предназначена для чтения данных из файла, содержащего информацию о датчиках, и сохранения этих данных в массиве структур Sensor
* const char* filename - имя файла, который будет открыт при чтении
* Sensor* sensors - указатель на массив структур Sensor, куда будут сохранены считанные данные
* int* sensor_count - указатель на переменную, в которой будет храниться количество датчиков, считанных из файла
* Возвращает 0, если данные успешно считаны, 1 – если произошла ошибка при открытии или чтении файла.
*/
int read(const char* filename, Sensor* sensors, int* sensor_count) {
    char line[100];
    *sensor_count = 0; /*Счетчик датчиков*/
    int temp_read = 0, vlazh_read = 0; /*Флаги для отслеживания значений*/
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла.\n");
        return 1;
    }
    /*Считывание строк из файла и их запись в массив датчиков*/
    while (fgets(line, sizeof(line), file)) {
        if (*sensor_count >= MAX) {
            break; /*Предотвращение превышения максимального количества датчиков*/
        }
        /*Если строка содержит символ '^', то это температура*/
        if (strchr(line, '^')) {
            sensors[*sensor_count].temp = atof(line);
            temp_read = 1; /*Температура считана*/
            if (!vlazh_read) sensors[*sensor_count].vlazh = -1; /*Влажность пока не считана*/
        }
        /*Если строка содержит символ '%', то это влажность*/
        else if (strchr(line, '%')) {
            sensors[*sensor_count].vlazh = atof(line);
            vlazh_read = 1; /*Влажность считана*/
            if (!temp_read) sensors[*sensor_count].temp = -1; /*Температура пока не считана*/
        }
        /*Если оба значения получены — переход к следующему датчику*/
        if (temp_read && vlazh_read) {
            (*sensor_count)++;
            temp_read = vlazh_read = 0; /*Сбрасывание флагов для следующего датчика*/
        }
    }
    if (temp_read || vlazh_read) {
        (*sensor_count)++;
    }
    fclose(file);
    return 0;
}

/* Задает границы для температуры и влажности в зависимости от выбранных пользователем параметров сезона и степени работы
* Znach* pole - указатель на структуру типа Znach
* Возвращает 0 В случае успешной работы. Если данные о сезоне или степени работы неверны, возвращает 1
*/
int limit(Znach* pole) {
    pole->vlazhmin = 40.0;
    pole->vlazhmax = 60.0;
    if (pole->season_choice == 1 && pole->work_choice == 1) {
        pole->tempmin = 22.0; pole->tempmax = 24.0;
    }
    else if (pole->season_choice == 1 && pole->work_choice == 2) {
        pole->tempmin = 21.0; pole->tempmax = 23.0;
    }
    else if (pole->season_choice == 2 && pole->work_choice == 1) {
        pole->tempmin = 23.0; pole->tempmax = 25.0;
    }
    else if (pole->season_choice == 2 && pole->work_choice == 2) {
        pole->tempmin = 22.0; pole->tempmax = 24.0;
    }
    else {
        printf("Неверные данные.\n");
        return 1;
    }
    return 0;
}

/* Генерирует тестовые данные для датчиков и записывает эти данные в файл
* const char* filename — файл, в который будут записаны сгенерированные данные
* int count — количество датчиков, для которых будут сгенерированы данные
* Выводит сообщение об ошибке в случае проблем с открытием файла
*/
void generate(const char* filename, int count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Ошибка открытия файла для записи.\n");
        return 1;
    }
    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        float temp = 20.0 + rand() % 11; /*Температура от 20 до 30 градусов*/
        float vlazh = 40.0 + rand() % 21; /*Влажность от 40 до 60 процентов*/
        fprintf(file, "%.2f^\n", temp);
        fprintf(file, "%.2f%%\n", vlazh);
    }
    fclose(file);
}

/* Анализирует данные с датчиков, определяя максимальные и минимальные значения среди всех датчиков. Затем она выводит эти данные на экран и записывает результаты в файл
* Sensor* sensors — указатель на массив датчиков
* int sensor_count — количество датчиков
* const char* output_filename — имя файла, в который будут записаны результаты анализа
*/
void analyze_min_max(Sensor* sensors, int sensor_count, const char* output_filename) {
    if (sensor_count == 0) {
        printf("Нет данных для анализа.\n");
        return 1;
    }
    int max_temp_index = 0, min_temp_index = 0, max_vlazh_index = 0, min_vlazh_index = 0;
    for (int i = 1; i < sensor_count; i++) {
        if (sensors[i].temp > sensors[max_temp_index].temp) max_temp_index = i;
        if (sensors[i].temp < sensors[min_temp_index].temp) min_temp_index = i;
        if (sensors[i].vlazh > sensors[max_vlazh_index].vlazh) max_vlazh_index = i;
        if (sensors[i].vlazh < sensors[min_vlazh_index].vlazh) min_vlazh_index = i;
    }
    // Вывод на экран результатов анализа
    printf("\nРезультаты анализа датчиков:\n");
    printf("Максимальная температура: Датчик %d, Температура = %.2f\n", max_temp_index + 1, sensors[max_temp_index].temp);
    printf("Минимальная температура: Датчик %d, Температура = %.2f\n", min_temp_index + 1, sensors[min_temp_index].temp);
    printf("_________________________________________________________\n");
    printf("Максимальная влажность: Датчик %d, Влажность = %.2f\n", max_vlazh_index + 1, sensors[max_vlazh_index].vlazh);
    printf("Минимальная влажность: Датчик %d, Влажность = %.2f\n", min_vlazh_index + 1, sensors[min_vlazh_index].vlazh);
    /*Запись результатов в файл*/
    FILE* file = fopen(output_filename, "w");
    if (file == NULL) {
        printf("Ошибка открытия файла для записи результатов анализа.\n");
        return 1;
    }
    fprintf(file, "Результаты анализа датчиков:\n");
    fprintf(file, "Максимальная температура: Датчик %d, Температура = %.2f\n", max_temp_index + 1, sensors[max_temp_index].temp);
    fprintf(file, "Минимальная температура: Датчик %d, Температура = %.2f\n", min_temp_index + 1, sensors[min_temp_index].temp);
    fprintf(file, "Максимальная влажность: Датчик %d, Влажность = %.2f\n", max_vlazh_index + 1, sensors[max_vlazh_index].vlazh);
    fprintf(file, "Минимальная влажность: Датчик %d, Влажность = %.2f\n", min_vlazh_index + 1, sensors[min_vlazh_index].vlazh);
    fclose(file);
    printf("Результаты успешно записаны в файл '%s'.\n", output_filename);
}


