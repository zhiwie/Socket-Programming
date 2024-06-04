#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BOOKS 10
#define MAX_STUDENTS 10
#define FILENAME_SIZE 1000
#define MAX_LINE 2000
#define NUM_FILES 4

double penaltyValue = 0;  //declare penaltyValue as a global variable
double showPenalty = 0;

struct currentUser{
    char name[50];
    char ID[6];//maximum size of 9 including NULL character
};

struct Book {
    char ISBN[14]; 
    char title[100];
    char author[100];
    int year;
    int num_available;
    int total;
};

struct Date {
    char day[3];
    char month[3];
    char year[5];
};

struct StudentDetails {
    char ID[7]; // Adjusted size to hold 6 characters plus null terminator
    char name[50]; // Adjusted size to hold 50 characters plus null terminator
    int num_borrowed;
    int num_returned;
    char book_returned[1000][100];
    char book_borrowed[1000][100]; // Assuming a student can borrow and return up to 1000 books
    struct Date date_borrowed[MAX_BOOKS];//to store date of borrowed for each book
};

struct FileData {
    char changeList[FILENAME_SIZE];
    char temp_changeList[FILENAME_SIZE];
};

int searching_books() {
    struct Book books[MAX_BOOKS]; // 10 books registered in the system
    char searchISBN[20];

    int i, found;
    FILE *bookPtr = fopen("book_list.txt", "r");
    if (bookPtr == NULL) {
        puts("Error opening book_list.txt");
        puts("Please ensure that you have downloaded book_list.txt successfully");
        return 1;
    }

    for (i = 0; i < MAX_BOOKS; i++) {
        if (fscanf(bookPtr, "%s %99s %99s %d %d %d", books[i].ISBN, books[i].title, books[i].author, &books[i].year, &books[i].num_available, &books[i].total) != 6) {
            printf("Error reading data from line %i in book_list.txt\n", i + 1);
            puts("Please ensure that book_list.txt is the original and complete version");
            return 1;
        }
    }

    fclose(bookPtr);

    printf("Enter ISBN of the book: ");
    scanf("%s", searchISBN);

    found = 0;
    for (i = 0; i < MAX_BOOKS; i++) {
        if (strcmp(searchISBN, books[i].ISBN) == 0) {
            found = 1;
            puts("\nBook Found!!\n\n");

            printf("Title of the Book: %s\n", books[i].title);
            printf("Author of the Book: %s\n", books[i].author);
            printf("ISBN: %s\n", books[i].ISBN);
            printf("Year of publication: %d\n", books[i].year);
            printf("Number of copies available: %d\n", books[i].num_available);
            printf("Total number of copies: %d\n", books[i].total);
            break;
        }
    }

    if (found == 0) {
        puts("\nBook not found\n\n");
    }
    printf("\nRedirecting you to main page...\n\n");
    return 0;
}

int borrow_return(int *num_borrow, int *num_return) {
    struct Book *books = malloc(MAX_BOOKS *sizeof(struct Book));//10 books registered in the system
    struct StudentDetails *students = malloc(MAX_STUDENTS *sizeof(struct StudentDetails));//10 students registered in the system

    if (books == NULL || students == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    char ID[7]; // Adjusted size to hold 6 characters plus null terminator
    char action;
    int i, j;
    int studentFound=0;
    int bookFoundAvailable=0;

    char title[100];//array to store book title to be borrow/return entered by user

    FILE *bookListPtr = fopen("book_list.txt", "r");
    FILE *studentPtr = fopen("students.txt", "r");
    FILE *student_borrowPtr = fopen("student_borrow_list.txt", "r");
    FILE *dateBorrowPtr = fopen("date_borrowed.txt","r");

    if (studentPtr==NULL|| student_borrowPtr==NULL|| bookListPtr==NULL|| dateBorrowPtr==NULL) {
        fprintf(stderr,"Error opening 'students.txt' or 'student_borrow_list.txt' or 'book_list.txt or 'date_borrowed.txt'");
        return 1;;//add code to redirect to main page
    }

    for (i = 0; i < MAX_STUDENTS; i++) {
        for (j = 0; j < MAX_BOOKS; j++) {
            strcpy(students[i].book_borrowed[j], "");
            strcpy(students[i].date_borrowed[j].day, "");
            strcpy(students[i].date_borrowed[j].month, "");
            strcpy(students[i].date_borrowed[j].year, "");
        }
        if (fscanf(studentPtr, "%6s %49s %d %d", students[i].ID, students[i].name, &students[i].num_borrowed, &students[i].num_returned) != 4) {
            printf("Error reading data from line %d in students.txt\n", i+1);
            return 1;
        }

        for (j = 0; j < students[i].num_borrowed; j++) {
            if (fscanf(student_borrowPtr, "%99s", students[i].book_borrowed[j]) != 1) {
                printf("Error reading title of borrowed book for student %s in student_borrow_list.txt\n", students[i].ID);
                return 1;
            }

            if (fscanf(dateBorrowPtr, "%s %s %s ", students[i].date_borrowed[j].day, students[i].date_borrowed[j].month, students[i].date_borrowed[j].year) != 3) {
                printf("Error reading borrowed dates from line %d in date_borrowed.txt\n", i+1);
                return 1;
            }
        }

        if (fscanf(bookListPtr, "%s %99s %99s %d %d %d", books[i].ISBN, books[i].title, books[i].author, &books[i].year, &books[i].num_available, &books[i].total) != 6) {
            printf("Error reading data from line %d in book_list.txt\n", i+1);
            // Handle error appropriately
            return 1;
        }
    }
//ends checking the file
fclose(bookListPtr);
fclose(student_borrowPtr);
fclose(dateBorrowPtr);
fclose(studentPtr);
//starts checking the previous record of student
    do {
        printf("Enter student ID: ");
        scanf("%6s", ID);//with maximum size of 6 including NULL char
        //rewind(studentPtr); //reset pointer to the beginning of the file
                            //restart the search of the student

        for (i=0; i<MAX_STUDENTS; i++) {//checks 10 students registered in the system one by one
            if (strcmp(ID, students[i].ID) == 0) {//found an exact match, student ID is valid
                studentFound=1; // updates 0 to 1, indicates that student is found in the system
                printf("Student Name: %s\n", students[i].name);
                printf("Books Borrowed:\t\t\t\t\tDate of Borrow:\n");//add code to display date of borrow beside the book borrowed

                for (j=0; j<MAX_BOOKS; j++) {//only maximum 10 books can be borrowed
                    if (students[i].book_borrowed[j][0]!=NULL) {//printing borrowed books (preceding column) of one student (same line)
                        printf("%d-%-46s%s %s %s\n", j+1, students[i].book_borrowed[j], students[i].date_borrowed[j].day, students[i].date_borrowed[j].month, students[i].date_borrowed[j].year);
                    }
                }
                break;//break for loop to prevent unnecessary iterations through the remaining students
            }
        }
        //student is still not found after looping the 10 students registered in system
        if (!studentFound) {
            printf("\nNo such student found with the entered ID: %s \nPlease enter a valid student ID\n", ID);
        }
    } while (!studentFound); //continue the loop until a valid student ID is entered
//ends showing the list books borrowed by the student

//starts borrow and return attampt and update the file manually at the same time
do{
        printf("Enter b for borrow / r for return: ");//add code to prompt user to enter valid input
        scanf(" %c", &action);                                                               //assume that only one book is allowed to be borrow/return at a time
        action=tolower(action);

        switch (action) {
            case 'b':
                puts("------Borrow Platform------");
                do {
                    printf("Enter the exact title of the book to borrow: ");
                    scanf("%s", title);

                    for (i = 0; i < MAX_BOOKS; i++) {
                        if (strcmp(title, books[i].title) == 0) {
                            if (books[i].num_available > 0 && students[i].num_borrowed < 10) {
                                bookFoundAvailable = 1;
                                (*num_borrow)++;
                                printf("Book is found and available to be borrowed\n\n");
                            } else if (books[i].num_available > 0) {
                                printf("No available copies left for this book.\n\n");
                            } else {
                                printf("You have reached the maximum limit of borrowing!\n");
                                printf("Please return at least one book to borrow.\n\n");
                            }
                            break; // Book found, no need to continue the loop
                        }
                    }

                    if (!bookFoundAvailable) {
                        printf("Book is not found in the system.\n\n");
                    }

                } while (!bookFoundAvailable);//stop asking until user entered a valid book title
                break;

            case 'r':
                puts("------Return platform------");
                do{
                    printf("Enter the exact title of the book to return: ");
                    scanf("%s", title);
                    for (i=0;i<MAX_STUDENTS;i++){//check the book is available in student_borrow_list.txt
                        if (strcmp(ID, students[i].ID) == 0) {
                            for(j=0;j<students[i].num_borrowed;j++){
                                if (strcmp(title, students[i].book_borrowed[j]) == 0){
                                    bookFoundAvailable=1;//update book is found
                                    (*num_return)++;
                                    printf("Book is found and can be returned\n\n");
                                }
                            }
                        }
                    }
                    if(!bookFoundAvailable){
                                printf("Book is not found in the borrowed list of the student\n");
                            }
                    }while(!bookFoundAvailable);
                    break;
            default:
                puts("Error. Enter a valid input");
                break;

        }//ends switch-case loop
}while(action!='b'&&action!='r');
        puts("You are required to update the information in the following 4 text files.");
        puts("You will NEED to refer to the original text file to update the following accordingly.");
        puts("The following are the name of the targeted file to replace.");
        printf("1-book_list.txt\n2-students.txt\n3-student_borrow_list.txt\n4-date_borrowed.txt\n");

        for (i = 0; i < NUM_FILES; ++i) {//4 indicates number of files needed to update
            struct FileData fileData;
            int replace_line=0;

            printf("\nTargeted file to replace: ");
            scanf("%s", fileData.changeList);

            strcpy(fileData.temp_changeList, "temp___");
            strcat(fileData.temp_changeList, fileData.changeList);

            printf("Enter the line number of the text file to change (number ONLY): ");
            scanf("%d", &replace_line);
            fflush(stdin);

            char replace[MAX_LINE];
            printf("Enter the content that you want to replace with: ");
            fgets(replace, MAX_LINE, stdin);

            FILE *changePtr = fopen(fileData.changeList, "r");
            FILE *temp = fopen(fileData.temp_changeList, "w");

            int keep_reading = 1;
            int current_line = 1;
            char buffer[MAX_LINE];

            do {
                if (fgets(buffer, MAX_LINE, changePtr) == NULL) {
                    keep_reading = 0;
                } else if (current_line == replace_line) {
                    fputs(replace, temp);
                } else {
                    fputs(buffer, temp);
                }
                current_line++;
            } while (keep_reading);

            fclose(changePtr);
            fclose(temp);

            remove(fileData.changeList);
            rename(fileData.temp_changeList, fileData.changeList);
        }
            puts("\nCongratulations!! You have successfully updated all text files!");
            printf("\nRedirecting you to main page...\n\n");
            return 0;
}
int display() {
    FILE *bookDisplayPtr = fopen("book_list.txt", "r");

    int i;
    // Open file for reading book list
    if (bookDisplayPtr == NULL) {
        perror("Error opening book_list.txt");
        return 1; // Return an error code to indicate failure
        //return to main page
    }

    struct Book books[MAX_BOOKS]; // Assuming you have 10 books

    for (i = 0; i < MAX_BOOKS; i++) {//to print out all book list
        if (fscanf(bookDisplayPtr, "%s %99s %99s %d %d %d", books[i].ISBN, books[i].title, books[i].author, &books[i].year, &books[i].num_available, &books[i].total) != 6) {//to check number of successfully scanned items is completed
            fprintf(stderr, "Error reading data from book_list.txt in line %d", i+1);
            return 1;
        }
    }

    // Close the book file
    fclose(bookDisplayPtr);

    // Display the list of books
    for (int i = 0; i < MAX_BOOKS; i++) {
        printf("Book %d:\n", i+1);
        printf("ISBN: %s\n", books[i].ISBN);
        printf("Title: %s\n", books[i].title);
        printf("Author: %s\n", books[i].author);
        printf("Year of Publication: %d\n", books[i].year);
        printf("Available: %d\n", books[i].num_available);
        printf("Total: %d\n\n", books[i].total);
    }
    printf("\nRedirecting you to main page...\n\n");
    return 0;
}
double penalty(double *penaltyValue) {
    int day_diff = 0;
    int day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int d1, m1, y1;
    int d2, m2, y2;
    char paid;

    printf("Enter borrow date (dd mm yy): ");
    scanf("%d %d %d", &d1, &m1, &y1);

    printf("Enter return date (dd mm yy): ");
    scanf("%d %d %d", &d2, &m2, &y2);

    if (y2>y1 || (y2==y1 && m2>m1) || (y2==y1 && m2==m1 && d2>d1)) {
        for (int i = m1; i <= 12; i++) {
            day_diff += day[i-1] - d1;
            d1 = 0; // reset d1 after the first iteration
        }
        for (int i = 1; i < m2; i++) {
            day_diff += day[i-1];
        }

        day_diff += d2;
        day_diff-=365;

        if (day_diff <= 7) {
            (*penaltyValue) += 0;
            printf("No penalty is charged because within grace period\n");
        } else {
            double currentPenalty = day_diff - 7;
            printf("Duration of borrow: %d day(s)\n", day_diff);
            printf("RM1.00/day will be charged for late return\n\nJust for confirmation purpose\n");
            
            printf("Have you paid the penalty? (press y for yes, press any key for n): ");
            scanf(" %c", &paid);

            if (paid == 'y') {
                (*penaltyValue) += currentPenalty;
                printf("Total penalty for late return of this book: RM%.2f\n", currentPenalty);
                printf("Total penalty paid has been successfully updated!!\n");
            } else {
                printf("No worries!\n");
                printf("The total penalty of this book won't be recorded into the system\n");
                printf("Total penalty for late return of this book: RM%.2f\n", currentPenalty);
            }
        }

    } else if (y1 == y2 && m1 == m2 && d1 == d2) {
        printf("Duration of borrow: 0 day(s)\n");
    } else {
        printf("Invalid date entered\nPlease ensure that return date is greater than borrow date\n");
    }

    printf("\nRedirecting you to the main page...\n\n");
    return 0;
}

int monthly_report(int *num_borrow, int *num_return, double *penaltyValue){
    int num_to_be_returned=0;
    struct StudentDetails *students = malloc(MAX_STUDENTS * sizeof(struct StudentDetails));

    if (students == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    FILE *studentsPtr = fopen("students.txt", "r");
    for(int i=0;i<MAX_STUDENTS;i++){//reading file information
        if (fscanf(studentsPtr, "%6s %49s %d %d", students[i].ID, students[i].name, &students[i].num_borrowed, &students[i].num_returned) != 4) {
            printf("Error reading data from line %d in students.txt\n", i+1);
            return 1;
        }
        num_to_be_returned+=students[i].num_borrowed;
    }
    fclose(studentsPtr);

    printf("----------November Month Report---------------\n");

    printf("Number of Books Borrowed\t\t %d\n", *num_borrow);//results read from files
    printf("Number of Books Returned\t\t %d\n", *num_return);//results read from students.txt
    printf("Number of Books to be Returned\t\t %i\n", num_to_be_returned);//show each 10 students then sum up
    printf("Total fines Collected (this month):\t RM%.2f\n", *penaltyValue);

    printf("\nRedirecting you to main page...\n\n");

    return 0;
}

int main(void){
    int choice;
    char password[4];//password is UNM, including NULL character, so maximum size is 4
    struct currentUser user;
    struct StudentDetails students[MAX_STUDENTS];
    int validChoice=0, num_borrow=0, num_return=0, studentFound=0;

    FILE *Ptr = fopen("students.txt", "r");
    if(Ptr==NULL){
        fprintf(stderr,"Error opening 'students.txt'\n");
        puts("Please ensure that you have downloaded all text files successfully");
        return 1;;//add code to redirect to main page
    }
    for(int i=0;i<MAX_STUDENTS;i++){
        if (fscanf(Ptr, "%6s %49s %d %d", students[i].ID, students[i].name, &students[i].num_borrowed, &students[i].num_returned) != 4) {
                printf("Error reading data from line %d in students.txt\n", i+1);
                return 1;
        }
    }
    fclose(Ptr);
    do{
    puts("------Welcome to Library Management System------");
    printf("Enter your ID: ");
    scanf("%s", user.ID);//reads and store it into struct

    if (strlen(user.ID) == 6 && user.ID[0] == 'L'){//checks whether user fills ID completely and checks user's role
        printf("Enter password: ");
        scanf("%s", password);
        if (strcmp(password,"UNM")==0){
            puts("Hello, Librarian! \nWhat can I do for you today?\n");

            puts("1-searching books");
            puts("2-borrow and return");
            puts("3-display list of books");
            puts("4-calculate penalty for late return");
            puts("5-generate monthly report");
            do{
                printf("Enter the following number to continue: ");
                scanf("%i", &choice);

                switch(choice){
                    case 1:
                        searching_books();
                        validChoice=1;
                        break;
                    case 2:
                        borrow_return(&num_borrow, &num_return);
                        validChoice=1;
                        break;
                    case 3:
                        display();
                        validChoice=1;
                        break;
                    case 4:
                        penalty(&penaltyValue);
                        validChoice=1;
                        break;
                    case 5:
                        monthly_report(&num_borrow, &num_return, &penaltyValue);
                        validChoice=1;
                        break;
                    default:
                        puts("\nError, please check your choice is within the range");
                }
            }while(!validChoice);//loop back to enter choice
            }
        else{
            puts("Error! Invalid password");
        }
    }//end if statement for actions available for librarian
    else {//else statement to check user ID is student registered or unrecognised user
        for (int i = 0; i < MAX_STUDENTS; i++) {
            if (strcmp(user.ID, students[i].ID) == 0) {
                studentFound = 1;
                printf("Hello, %s! \nWhat can I do for you today?\n", students[i].name);

                puts("1-searching books");
                puts("2-display list of books");
                puts("3-calculate penalty for late return");

                do {
                    printf("Enter the following number to continue: ");
                    scanf("%i", &choice);

                    switch (choice) {
                        case 1:
                            searching_books();
                            validChoice = 1;
                            break;
                        case 2:
                            display();
                            validChoice = 1;
                            break;
                        case 3:
                            penalty(&penaltyValue);
                            validChoice = 1;
                            break;
                        default:
                            puts("\nError, please check your choice is within the range");
                    }
                } while (!validChoice);
            }
        }
        if (!studentFound) {//if student is still not found, proceed to the next else condition
            puts("Error! Invalid input or unrecognized user.");// loops back to enter ID
            puts("\nRedirecting you to main page...\n\n");
        }
    }
} while (1); // creating an infinite loop
}//ends main function
