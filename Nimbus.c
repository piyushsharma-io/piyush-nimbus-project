#include <stdio.h>
#include <string.h>

struct Student {
    char id[20];
    char name[30];
};

struct Attendance {
    char id[20];
    char date[15];
    char status;
};

struct Student students[100];
struct Attendance records[2000];
int studentCount = 0, recordCount = 0;

void loadStudents();
void loadAttendance();
void registerStudent();
void markAttendance();
void viewAttendance();
void showDefaulters();
void showAnalytics();
void saveStudent(struct Student s);
void saveAttendance(struct Attendance a);
int findStudent(char id[]);
int dateExists(char date[]);
void printStudents(int index);
void printDefaulters(int index);
void displayMenu();

void loadStudents() {
    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL) return;
    studentCount = 0;
    while (studentCount < 100 && fscanf(fp, "%19s %29s", students[studentCount].id, students[studentCount].name) == 2)
        studentCount++;
    fclose(fp);
}

void loadAttendance() {
    FILE *fp = fopen("attendance.txt", "r");
    if (fp == NULL) return;
    recordCount = 0;
    while (recordCount < 2000 && fscanf(fp, "%19s %14s %c", records[recordCount].id, records[recordCount].date, &records[recordCount].status) == 3)
        recordCount++;
    fclose(fp);
}

void saveStudent(struct Student s) {
    FILE *fp = fopen("students.txt", "a");
    if (fp == NULL) {
        printf("Error opening student file.\n");
        return;
    }
    fprintf(fp, "%s %s\n", s.id, s.name);
    fclose(fp);
}

void saveAttendance(struct Attendance a) {
    FILE *fp = fopen("attendance.txt", "a");
    if (fp == NULL) {
        printf("Error opening attendance file.\n");
        return;
    }
    fprintf(fp, "%s %s %c\n", a.id, a.date, a.status);
    fclose(fp);
}

int findStudent(char id[]) {
    int i;
    for (i = 0; i < studentCount; i++)
        if (strcmp(students[i].id, id) == 0) return i;
    return -1;
}

int dateExists(char date[]) {
    int i;
    for (i = 0; i < recordCount; i++)
        if (strcmp(records[i].date, date) == 0) return 1;
    return 0;
}

void registerStudent() {
    struct Student s;
    if (studentCount >= 100) {
        printf("Student limit reached.\n");
        return;
    }
    printf("Enter student ID: ");
    scanf("%19s", s.id);
    if (findStudent(s.id) != -1) {
        printf("Student ID already exists.\n");
        return;
    }
    printf("Enter student name (no spaces): ");
    scanf("%29s", s.name);
    students[studentCount] = s;
    saveStudent(s);
    studentCount++;
    printf("Student registered successfully.\n");
}

void markAttendance() {
    char date[15], choice;
    int i;
    if (studentCount == 0) {
        printf("Register students first.\n");
        return;
    }
    if (recordCount + studentCount > 2000) {
        printf("Attendance record limit reached.\n");
        return;
    }
    printf("Enter lecture date (DD-MM-YYYY): ");
    scanf("%14s", date);
    if (dateExists(date)) {
        printf("Attendance for this date already exists.\n");
        return;
    }
    for (i = 0; i < studentCount; i++) {
        printf("%s (%s) - Present (P) or Absent (A): ", students[i].name, students[i].id);
        scanf(" %c", &choice);
        while (choice != 'P' && choice != 'p' && choice != 'A' && choice != 'a') {
            printf("Enter P or A: ");
            scanf(" %c", &choice);
        }
        strcpy(records[recordCount].id, students[i].id);
        strcpy(records[recordCount].date, date);
        records[recordCount].status = (choice == 'P' || choice == 'p') ? 'P' : 'A';
        saveAttendance(records[recordCount]);
        recordCount++;
    }
    printf("Attendance saved successfully.\n");
}

void printStudents(int index) {
    if (index >= studentCount) return;
    printf("%s - %s\n", students[index].id, students[index].name);
    printStudents(index + 1);
}

void viewAttendance() {
    int i, j, total, present;
    float percentage;
    if (studentCount == 0) {
        printf("No students registered.\n");
        return;
    }
    printf("\n--- STUDENT ATTENDANCE ---\n");
    printStudents(0);
    for (i = 0; i < studentCount; i++) {
        total = 0;
        present = 0;
        for (j = 0; j < recordCount; j++)
            if (strcmp(students[i].id, records[j].id) == 0) {
                total++;
                if (records[j].status == 'P') present++;
            }
        percentage = total > 0 ? (float)present / total * 100 : 0;
        printf("\nID: %s | Name: %s\n", students[i].id, students[i].name);
        printf("Lectures recorded: %d\n", total);
        printf("Present: %d | Absent: %d\n", present, total - present);
        printf("Attendance: %.2f%%\n", percentage);
    }
}

void printDefaulters(int index) {
    int i, total = 0, present = 0;
    float percentage;
    if (index >= studentCount) return;
    for (i = 0; i < recordCount; i++)
        if (strcmp(students[index].id, records[i].id) == 0) {
            total++;
            if (records[i].status == 'P') present++;
        }
    percentage = total > 0 ? (float)present / total * 100 : 0;
    if (total > 0 && percentage < 75)
        printf("%s (%s): %.2f%%\n", students[index].name, students[index].id, percentage);
    printDefaulters(index + 1);
}

void showDefaulters() {
    printf("\n--- STUDENTS BELOW 75%% ---\n");
    if (studentCount == 0) {
        printf("No students registered.\n");
        return;
    }
    printDefaulters(0);
}

void showAnalytics() {
    int i, j, lectureTotal, lecturePresent, totalPresent = 0, totalRecords = 0;
    float average;
    printf("\n--- LECTURE-WISE ANALYTICS ---\n");
    for (i = 0; i < recordCount; i++) {
        int alreadyCounted = 0;
        for (j = 0; j < i; j++)
            if (strcmp(records[i].date, records[j].date) == 0) {
                alreadyCounted = 1;
                break;
            }
        if (alreadyCounted) continue;
        lectureTotal = 0;
        lecturePresent = 0;
        for (j = 0; j < recordCount; j++)
            if (strcmp(records[i].date, records[j].date) == 0) {
                lectureTotal++;
                if (records[j].status == 'P') lecturePresent++;
            }
        printf("Date: %s | Present: %d/%d | Attendance: %.2f%%\n", records[i].date, lecturePresent, lectureTotal, lectureTotal > 0 ? (float)lecturePresent / lectureTotal * 100 : 0);
        totalPresent += lecturePresent;
        totalRecords += lectureTotal;
    }
    printf("\n--- CLASS SUMMARY ---\n");
    if (totalRecords > 0) {
        average = (float)totalPresent / (float)totalRecords * 100;
        printf("Total attendance records: %d\n", totalRecords);
        printf("Total present entries: %d\n", totalPresent);
        printf("Average class attendance: %.2f%%\n", average);
    } else printf("No attendance records available.\n");
}

void displayMenu() {
    int choice;
    do {
        printf("\n===== STUDENT ATTENDANCE SYSTEM =====\n");
        printf("1. Register Student\n2. Mark Attendance\n3. View Attendance\n4. Show Defaulters\n5. View Analytics\n6. Exit\nEnter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Restart the program.\n");
            return;
        }
        switch (choice) {
            case 1: registerStudent(); break;
            case 2: markAttendance(); break;
            case 3: viewAttendance(); break;
            case 4: showDefaulters(); break;
            case 5: showAnalytics(); break;
            case 6: printf("Exiting Attendance System.\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);
}

int main() {
    loadStudents();
    loadAttendance();
    displayMenu();
    return 0;
}
