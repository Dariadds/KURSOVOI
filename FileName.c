#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define MAX 20

typedef struct {
    float temp;
    float vlazh;
    int in_norm; /*���� ��������� ������� (1 - � �����, 0 - ��� �����)*/
} Sensor;

typedef struct {
    float tempmin;
    float tempmax;
    float vlazhmin;
    float vlazhmax;
    int season_choice;
    int work_choice;
} Znach;

/*���������� �������*/
void analyze_data(Sensor* sensors, int sensor_count, Znach* pole, int filter);
int new_sensors(int* sensor_count, Sensor* sensors, const char* filename);
int read(const char* filename, Sensor* sensors, int* sensor_count);
int limit(Znach* pole);
void generate(const char* filename, int count);
void analyze_min_max(Sensor* sensors, int sensor_count, const char* output_filename);

int main() {
    system("chcp 1251");
    puts("____________________________________________________________________________________");
    printf("| ��� ������������ ��������� Analysis! � ������ ��� � ����������� ������ � ��������.|\n");
    puts("|___________________________________________________________________________________|");
    puts("*����� ����� ������� ������ �� ������ ��������������� � �� ��������������� ���������\n");
    Sensor sensors[MAX];
    int sensor_count = 0;
    Znach pole = {0}; 
    int choice;
    printf("�������� �����:\n1 - ��������\n2 - Ҹ����\n> ");
    scanf("%d", &pole.season_choice);
    printf("�������� ������� ������:\n1 - ������ �\n2 - ������ �\n> ");
    scanf("%d", &pole.work_choice);
    limit(&pole);  /*��������� �������� ��� ����������� � ���������*/
    do {
        printf("\n�������� �����:\n");
        printf("1. ���������� ������ � ��������\n");
        printf("2. ��������� ������ � ����������\n");
        printf("3. �����\n");
        printf("> ");
        scanf("%d", &choice);
        switch (choice) {
        case 1: /* ���������� ������ �� ����� text.txt*/
            if (read("text.txt", sensors, &sensor_count) != 0) {
                printf("������ ������ ������ �� ����� text.txt.\n");
                break;
            }
            printf("�������� ������ ��� �����������:\n");
            printf("1. ������� � �����\n");
            printf("2. ������� ��� �����\n");
            printf("3. ��� �������\n");
            printf("4. ������ ������������ � ����������� ��������\n");
            printf("> ");
            int filter;
            scanf("%d", &filter);
            if (filter == 4) {
                analyze_min_max(sensors, sensor_count, "analis.txt"); /*������ ������������ � ����������� ��������*/
                break; 
            }
            analyze_data(sensors, sensor_count, &pole, filter);
            new_sensors(&sensor_count, sensors, "text.txt");
            break;
        case 2: /*��������� ������ � ���������� �� ����� test.txt*/
            printf("������� ���������� �������� ��� ��������� (�� ������ 20): ");
            int sensor_count_gen;
            scanf("%d", &sensor_count_gen);
            if (sensor_count_gen > MAX) {
                printf("���������� �������� ��������� �������� (%d). ����� ������������� %d ��������.\n", MAX, MAX);
                sensor_count_gen = MAX;
            }
            generate("test.txt", sensor_count_gen);
            printf("������ ������� ������������� .\n");
            if (read("test.txt",  sensors, &sensor_count) != 0) {
                printf("������ ������ ������ �� ����� 'test.txt'.\n");
                break;
            }
            analyze_data(sensors, sensor_count, &pole, 3);
            break;
        case 3: // �����
            printf("����� �� ���������.\n");
            break;
        default:
            printf("�������� �����. ���������� �����.\n");
            break;
        }
    } while (choice != 3);
    return 0;
}

/* ����������� ������ � �������� � ������� ���������� �� �� ���������. ��� ������������� ��������� ����������
* Sensor* sensors � ��������� �� ������ �������� Sensor, ���������� ������ ��������
* int sensor_count � ���������� ��������
* Znach* pole � ��������� �� ��������� ���� Znach
* int filter � ������ ��� ����������� ������
* ������� �� �����: ��������� ������� ������� (� ����� ��� ��� �����), ���������� ����������� ��� ��������� (���� ����)
*/
void analyze_data(Sensor* sensors, int sensor_count, Znach* pole, int filter) {
    for (int i = 0; i < sensor_count; i++) {
        int in_norm = (sensors[i].temp >= pole->tempmin && sensors[i].temp <= pole->tempmax &&
            sensors[i].vlazh >= pole->vlazhmin && sensors[i].vlazh <= pole->vlazhmax); /*��������, ��������� �� �������� ����������� � ��������� � �������� ����������*/
        float temp_otkl = 0.0, vlazh_otkl = 0.0;
        /*������������� ����������*/
        if (sensors[i].temp < pole->tempmin) {
            temp_otkl = pole->tempmin - sensors[i].temp; /*���� �����*/
        }
        else if (sensors[i].temp > pole->tempmax) {
            temp_otkl = sensors[i].temp - pole->tempmax; /*���� �����*/
        }
        if (sensors[i].vlazh < pole->vlazhmin) {
            vlazh_otkl = pole->vlazhmin - sensors[i].vlazh; /*���� �����*/
        }
        else if (sensors[i].vlazh > pole->vlazhmax) {
            vlazh_otkl = sensors[i].vlazh - pole->vlazhmax; /*���� �����*/
        }
        /*��������� �����*/
        if ((filter == 1 && in_norm) || (filter == 2 && !in_norm) || filter == 3) {
            printf("������ %d: ����������� = %.2f, ��������� = %.2f\n", i + 1, sensors[i].temp, sensors[i].vlazh);
            if (!in_norm) {
                if (temp_otkl != 0.0) {
                    if (sensors[i].temp < pole->tempmin) {
                        printf("  ���������� �����������: %.2f ���� ��������. ���������� �������� �����������.\n", temp_otkl);
                    }
                    else {
                        printf("  ���������� �����������: %.2f ���� ���������. ���������� �������� �����������.\n", temp_otkl);
                    }
                }
                if (vlazh_otkl != 0.0) {
                    if (sensors[i].vlazh < pole->vlazhmin) {
                        printf("  ���������� ���������: %.2f ���� ��������. ���������� �������� ���������.\n", vlazh_otkl);
                    }
                    else {
                        printf("  ���������� ���������: %.2f ���� ���������. ���������� �������� ���������.\n", vlazh_otkl);
                    }
                }
            }
            else {
                printf("  � �������� �����\n");
            }
        }
    }
}


/* ��������� ����������� ������ � ���������� �� � ����
* int* sensor_count � ��������� �� ���������� ��������
* Sensor* sensors � ��������� �� ������ �������� Sensor, � ������� ������������ ����������� ������
* const char* filename � ��� �����, � ������� ����� ������������ ������
* ���������� 1 ��� �������� ����������, 0 ��� ������� ������
*/
int new_sensors(int* sensor_count, Sensor* sensors, const char* filename) {
    FILE* file = fopen(filename, "a"); /* ��������� ���� ��� ���������� ��������*/
    if (file == NULL) {
        printf("������ �������� ����� ��� ������.\n");
        return 1;
    }
    for (int i = 0; i < *sensor_count; i++) {
        /*���� ����������� ���*/
        if (sensors[i].temp == -1) {
            printf("������� ����������� ��� ������� %d: ", i + 1);
            scanf("%f", &sensors[i].temp);
            fprintf(file, "%.2f^\n", sensors[i].temp); /*����������� � ����*/
            printf("����������� ������ ������� �������� � ����. �������� ����� �����\n");
        }
        /*���� ��������� ���*/
        if (sensors[i].vlazh == -1) {
            printf("������� ��������� ��� ������� %d: ", i + 1);
            scanf("%f", &sensors[i].vlazh);
            fprintf(file, "%.2f%%\n", sensors[i].vlazh); /*����������� � ����*/
            printf("����������� ������ ������� �������� � ����. �������� ����� �����\n");
        }
    }
    fclose(file);
    return 0;
}

/* ������������� ��� ������ ������ �� �����, ����������� ���������� � ��������, � ���������� ���� ������ � ������� �������� Sensor
* const char* filename - ��� �����, ������� ����� ������ ��� ������
* Sensor* sensors - ��������� �� ������ �������� Sensor, ���� ����� ��������� ��������� ������
* int* sensor_count - ��������� �� ����������, � ������� ����� ��������� ���������� ��������, ��������� �� �����
* ���������� 0, ���� ������ ������� �������, 1 � ���� ��������� ������ ��� �������� ��� ������ �����.
*/
int read(const char* filename, Sensor* sensors, int* sensor_count) {
    char line[100];
    *sensor_count = 0; /*������� ��������*/
    int temp_read = 0, vlazh_read = 0; /*����� ��� ������������ ��������*/
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("������ �������� �����.\n");
        return 1;
    }
    /*���������� ����� �� ����� � �� ������ � ������ ��������*/
    while (fgets(line, sizeof(line), file)) {
        if (*sensor_count >= MAX) {
            break; /*�������������� ���������� ������������� ���������� ��������*/
        }
        /*���� ������ �������� ������ '^', �� ��� �����������*/
        if (strchr(line, '^')) {
            sensors[*sensor_count].temp = atof(line);
            temp_read = 1; /*����������� �������*/
            if (!vlazh_read) sensors[*sensor_count].vlazh = -1; /*��������� ���� �� �������*/
        }
        /*���� ������ �������� ������ '%', �� ��� ���������*/
        else if (strchr(line, '%')) {
            sensors[*sensor_count].vlazh = atof(line);
            vlazh_read = 1; /*��������� �������*/
            if (!temp_read) sensors[*sensor_count].temp = -1; /*����������� ���� �� �������*/
        }
        /*���� ��� �������� �������� � ������� � ���������� �������*/
        if (temp_read && vlazh_read) {
            (*sensor_count)++;
            temp_read = vlazh_read = 0; /*����������� ������ ��� ���������� �������*/
        }
    }
    if (temp_read || vlazh_read) {
        (*sensor_count)++;
    }
    fclose(file);
    return 0;
}

/* ������ ������� ��� ����������� � ��������� � ����������� �� ��������� ������������� ���������� ������ � ������� ������
* Znach* pole - ��������� �� ��������� ���� Znach
* ���������� 0 � ������ �������� ������. ���� ������ � ������ ��� ������� ������ �������, ���������� 1
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
        printf("�������� ������.\n");
        return 1;
    }
    return 0;
}

/* ���������� �������� ������ ��� �������� � ���������� ��� ������ � ����
* const char* filename � ����, � ������� ����� �������� ��������������� ������
* int count � ���������� ��������, ��� ������� ����� ������������� ������
* ������� ��������� �� ������ � ������ ������� � ��������� �����
*/
void generate(const char* filename, int count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("������ �������� ����� ��� ������.\n");
        return 1;
    }
    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        float temp = 20.0 + rand() % 11; /*����������� �� 20 �� 30 ��������*/
        float vlazh = 40.0 + rand() % 21; /*��������� �� 40 �� 60 ���������*/
        fprintf(file, "%.2f^\n", temp);
        fprintf(file, "%.2f%%\n", vlazh);
    }
    fclose(file);
}

/* ����������� ������ � ��������, ��������� ������������ � ����������� �������� ����� ���� ��������. ����� ��� ������� ��� ������ �� ����� � ���������� ���������� � ����
* Sensor* sensors � ��������� �� ������ ��������
* int sensor_count � ���������� ��������
* const char* output_filename � ��� �����, � ������� ����� �������� ���������� �������
*/
void analyze_min_max(Sensor* sensors, int sensor_count, const char* output_filename) {
    if (sensor_count == 0) {
        printf("��� ������ ��� �������.\n");
        return 1;
    }
    int max_temp_index = 0, min_temp_index = 0, max_vlazh_index = 0, min_vlazh_index = 0;
    for (int i = 1; i < sensor_count; i++) {
        if (sensors[i].temp > sensors[max_temp_index].temp) max_temp_index = i;
        if (sensors[i].temp < sensors[min_temp_index].temp) min_temp_index = i;
        if (sensors[i].vlazh > sensors[max_vlazh_index].vlazh) max_vlazh_index = i;
        if (sensors[i].vlazh < sensors[min_vlazh_index].vlazh) min_vlazh_index = i;
    }
    // ����� �� ����� ����������� �������
    printf("\n���������� ������� ��������:\n");
    printf("������������ �����������: ������ %d, ����������� = %.2f\n", max_temp_index + 1, sensors[max_temp_index].temp);
    printf("����������� �����������: ������ %d, ����������� = %.2f\n", min_temp_index + 1, sensors[min_temp_index].temp);
    printf("_________________________________________________________\n");
    printf("������������ ���������: ������ %d, ��������� = %.2f\n", max_vlazh_index + 1, sensors[max_vlazh_index].vlazh);
    printf("����������� ���������: ������ %d, ��������� = %.2f\n", min_vlazh_index + 1, sensors[min_vlazh_index].vlazh);
    /*������ ����������� � ����*/
    FILE* file = fopen(output_filename, "w");
    if (file == NULL) {
        printf("������ �������� ����� ��� ������ ����������� �������.\n");
        return 1;
    }
    fprintf(file, "���������� ������� ��������:\n");
    fprintf(file, "������������ �����������: ������ %d, ����������� = %.2f\n", max_temp_index + 1, sensors[max_temp_index].temp);
    fprintf(file, "����������� �����������: ������ %d, ����������� = %.2f\n", min_temp_index + 1, sensors[min_temp_index].temp);
    fprintf(file, "������������ ���������: ������ %d, ��������� = %.2f\n", max_vlazh_index + 1, sensors[max_vlazh_index].vlazh);
    fprintf(file, "����������� ���������: ������ %d, ��������� = %.2f\n", min_vlazh_index + 1, sensors[min_vlazh_index].vlazh);
    fclose(file);
    printf("���������� ������� �������� � ���� '%s'.\n", output_filename);
}


