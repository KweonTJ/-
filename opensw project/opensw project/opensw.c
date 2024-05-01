#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
//#include <io.h> // UTF-8을 활성화 하기 위해서는 필요한 라이브러리

int get_first_day_of_the_year(int year);

struct Date
{
    int dd;
    int mm;
    int yy;
};
struct Date date;

struct R
{
    int dd;
    int mm;
    char title[100];
};

struct Remainde
{
    int dd;
    int mm;
    char note[50];
};

struct Note
{
    int dd;
    int mm;
    struct Date date;
    char title[100];
};

COORD xy = { 0,0 };

/*void setUTF8Mode() // 한글 입력을 위한 코드, 작동하지 않아서 주석처리
{
    int result1 = _setmode(_fileno(stdout), _O_U8TEXT);
    int result2 = _setmode(_fileno(stdin), _O_U8TEXT);

    if (result1 == -1 || result2 == -1)
    {
        printf("Error setting UTF-8 mode.\n");
        exit(EXIT_FAILURE);
    }
}*/

void chearInputBuffer()
{
    int c;
    while((c=getchar()) != '\n' && c != EOF){}
}

void gotoxy(int x, int y)
{
    xy.X = x;
    xy.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
}

void SetColor(int ForgC)
{
    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
    return;
}

void ClearColor()
{
    SetColor(15);
}

void ClearConsoleToColors(int ForgC, int BackC)
{
    WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD coord = { 0, 0 };

    DWORD count;

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    SetConsoleTextAttribute(hStdOut, wColor);
    if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        FillConsoleOutputCharacter(hStdOut, (TCHAR)32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);

        FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
        SetConsoleCursorPosition(hStdOut, coord);
    }
    return;
}

void SetColorAndBackground(int ForgC, int BackC)
{
    WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
    return;
}

int check_leapYear(int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        return 1;
    return 0;

}

int checkNote(int dd, int mm)
{
    FILE* fp;
    struct R R;

    fp = fopen("note.txt", "rb");
    if (fp == NULL)
    {
        printf("Error in Opening the file");
        return -1;
    }
    while (fread(&R, sizeof(R), 1, fp) == 1)
    {
        if (R.dd == dd && R.mm == mm)
        {
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void AddNote();
void showNote();
void deleteNote();

void remove_newline(char* str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

void AddNote() // 노트 입력 void
{
    FILE* fp;
    struct Note newNote = { 0 };

    fp = fopen("note.txt", "a+");
    if (fp == NULL)
    {
        printf("Error in Opening the file");
        return;
    }

    system("cls");
    gotoxy(5, 3);
    printf("Enter the date(DD/MM): ");
    scanf_s("%d%d", &newNote.dd, &newNote.mm);
    
    gotoxy(5, 4);
    printf("Enter the Note(50 character max): ");
    while (getchar() != '\n');

    fgets(newNote.title, sizeof(newNote.title), stdin);
    remove_newline(newNote.title);

    gotoxy(5, 5);
    printf("Your Note : %s", newNote.title);

    /*if (fprintf(fp, "%02d/%02d %s\n", R.dd, R.mm, R.note) < 0)
    {
        printf("Error in writing to the file.\n");
        fclose(fp); // 파일 닫기
        return;
    }*/
    if (fwrite(&newNote, sizeof(newNote), 1, fp) != 1)
    {
        printf("Error in writing to the file.\n");
        fclose(fp); // 파일 닫기
        return;
    }

    fflush(fp);    // 파일 닫기
    fclose(fp);

    gotoxy(5, 8);
    printf("Note added successfully.\n");
    gotoxy(5, 9);
    printf("Press any key to continue...");
    (void)_getch();
}

/*void showNote(int mm) // 기존 showNote 저장된 노트를 불러 올 수 없어서 새로 작성하였다.
{
    FILE* fp;
    int i = 0, isFound = 0;

    system("cls");
    fp = fopen("note.txt", "r");
    if (fp == NULL)
    {
        printf("Error in opening the file");
        return;
    }

    rewind(fp);

    //while (fscanf(fp, "%d %d %[^\n]", &R.dd, &R.mm, R.note) == 3)
    while (fread(&R, sizeof(R), 1, fp) == 1)
    {
        if (R.mm == mm)
        {
            gotoxy(10, 5 + i);
            printf("Note %d Day = %d: %s", i + 1, R.dd, R.note);
            isFound = 1;
            i++;
        }
    }
    if (isFound == 0)
    { 
        gotoxy(10, 5);
        printf("This Month contains no note");
    }
    gotoxy(10, 7 + i);
    printf("Press any key to back.......");
    (void)_getch();

    fclose(fp);
}*/

void showNote() // 노트 불러오기 void
{
    FILE* fp;
    struct Note temp = { 0 };
    int isFound = 0;

    system("cls");
    fp = fopen("note.txt", "rb");
    if (fp == NULL)
    {
        printf("Error in opening the file");
        return;
    }

    printf("Saved Notes:\n\n");

    while (fread(&temp, sizeof(temp), 1, fp) == 1)
    {
        printf("Date: %02d/%02d\n", temp.dd, temp.mm);
        printf("Note: %s\n\n", temp.title);
        isFound = 1;
    }
    
    if (!isFound)
    {
        printf("No notes found.\n");
    }

    printf("Press any key to go back...");
    (void)_getch();

    fclose(fp);
}

void deleteNote() // 노트 삭제 void
{
    FILE* fp, * tmpFile;
    int isFound = 0, noteToDelete;
    struct Note temp = { 0 };

    system("cls");
    fp = fopen("note.txt", "rb");
    if (fp == NULL)
    {
        printf("Error in opening the file");
        return;
    }

    printf("Select the note you want to delete:\n\n");

    // Display all notes with an index for each
    int index = 1;
    while (fread(&temp, sizeof(temp), 1, fp) == 1)
    {
        printf("%d. Date: %02d/%02d\n", index, temp.dd, temp.mm); // 메모 날짜 출력
        printf("   Note: %s\n\n", temp.title);
        index++;
    }

    //fseek(fp, 0, SEEK_SET);

    fclose(fp);

    // Ask user to select note to delete
    printf("Enter the index of the note you want to delete (0 to cancel): ");
    scanf_s("%d", &noteToDelete);

    if (noteToDelete == 0)
    {
        printf("Deletion canceled.\n");
        return;
    }

    fp = fopen("note.txt", "rb");
    tmpFile = fopen("temp.txt", "wb");
    if (fp == NULL || tmpFile == NULL)
    {
        printf("Error in opening the file");
        return;
    }

    index = 1;
    // Rewrite all notes except the selected one to a temporary file
    while (fread(&temp, sizeof(temp), 1, fp) == 1)
    {
        if (index != noteToDelete)
        {
            fwrite(&temp, sizeof(temp), 1, tmpFile);
        }
        else
        {
            isFound = 1;
        }
        index++;
    }

    fclose(fp);
    fclose(tmpFile);

    // Delete original file and rename temporary file
    remove("note.txt");

    if (rename("temp.txt", "note.txt") == 0)
    {
        printf("Note deleted successfully.\n");
    }
    else
    {
        printf("Note not found.\n");
    }

    printf("Press any key to continue...");
    (void)_getch();
}

int main()
{
    //setUTF8Mode();  // 한글화 작업을 하기 위해 넣었지만, 작동하지 않아 주석처리
    char* months[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
    int i, j, total_days, week_day, year, space_counter = 0;
    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    int choice;
    while (1) 
    {
        system("cls");
        printf("     <God's Time Table> \n");
        printf("1. Check full year calendar \n"); // 전체 달력 확인
        printf("2. Add special note \n"); // 메모 작성 및 저장
        printf("3. Check saved notes \n"); // 저장된 메모 확인
        printf("4. Delete saved notes \n"); // 완료된 메모 삭제
        printf("5. Exit Program \n"); // 프로그램 종료
        printf("Enter your choice>>  "); // 번호 선택
        scanf_s("%d", &choice);
        system("cls");
        switch (choice)
        {
        case 1:
            printf("Enter Year>> ");
            scanf_s("%d", &year);
            printf("\n\n********** Welcome to %d **********\n\n", year);

            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            {
                days_in_month[1] = 29;
            }

            week_day = get_first_day_of_the_year(year);

            for (i = 0; i < 12; i++)
            {
                printf("\n\n\n\n  ---------------- %s ---------------\n", months[i]);
                printf("\n   Sun   Mon   Tue   Wed   Thu   Fri   Sat\n\n");

                for (space_counter = 1; space_counter <= week_day; space_counter++)
                {
                    printf("      ");
                }

                total_days = days_in_month[i];
                for (j = 1; j <= total_days; j++)
                {
                    printf("%6d", j);
                    week_day++;
                    if (week_day > 6)
                    {
                        week_day = 0;
                        printf("\n");
                    }
                }
            }
            printf("\nPress any key to continue......");
            (void)_getch();
            break;

        case 2:
            AddNote();
            break;

        case 3:
            showNote();
            break;

        case 4:
            deleteNote();
            break;

        case 5:
            exit(0);
            break;

        }

    }
    return 0;

}

int get_first_day_of_the_year(int year)
{
    int day = (year * 365 + ((year - 1) / 4) - ((year - 1) / 100) + ((year - 1) / 400)) % 7;

    return day;
}
