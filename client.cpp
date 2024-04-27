#include <iostream>
#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

struct buff {
	char arr[100];
	int movie;
	int ticket;
	int price;
	char movieName[30];
	char x;
	int cnic;
	bool check;
};

int main() {
	int sem1id = shmget(5025, 1024, 0);
	int sem2id = shmget(5026, 1024, 0);
	int sem3id = shmget(5027, 1024, 0);
	int idcs = shmget(5028, 1024, 0);
	if (sem1id == -1 || sem2id == -1 || idcs == -1) {
		cout << "Error creating shared memory..." << endl;
		return 1;
	}
	sem_t* semaphore1;
	sem_t* semaphore2;
	sem_t* semaphore3;
	buff* buffer;
	semaphore1 = (sem_t*)shmat(sem1id, NULL, 1);
	semaphore2 = (sem_t*)shmat(sem2id, NULL, 0);
	semaphore3 = (sem_t*)shmat(sem3id, NULL, 0);
	buffer = (buff*)shmat(idcs, NULL, 0);
	int movie, tickets;
	sem_wait(semaphore3);
	cout << "----------Welcome to Movie World----------" << endl;
	cout << "In theaters: " << endl;
	int count = 1;
	for (int i = 0; i < strlen(buffer->arr); i++) {
		if (buffer->arr[i] == '$') {
			break;
		}
		if (buffer->arr[i] == '\n' && buffer->arr[i + 1] != '$') {
			cout << endl << "[" << count << "] ";
			count++;
		}
		if (isalpha(buffer->arr[i]) || buffer->arr[i] == ' ') {
			cout << buffer->arr[i];
		}
	}
	cout << endl << endl;
	cout << "Select a movie: ";
	cin >> buffer->movie;
	cout << "Enter number of tickets: ";
	cin >> buffer->ticket;
	sem_post(semaphore1);
	sem_wait(semaphore2);
	if (buffer->check == false) {
		cout << "Movie or ticket entered is not available..." << endl;
		sem_post(semaphore1);
		shmdt(semaphore1);
		shmdt(semaphore2);
		shmdt(buffer);
		return 0;
	}
	else {
		cout << "Your total bill for the show " << buffer->movieName << " is " << buffer->price << endl;
		cout << "Press y to proceed or press n to cancel: ";
		cin >> buffer->x;
		if (buffer->x == 'n') {
			sem_post(semaphore1);
			shmdt(semaphore1);
			shmdt(semaphore2);
			shmdt(buffer);
			return 0;
		}

		else {
			cout << "Enter your 4-digit CNIC: ";
			cin >> buffer->cnic;
			sem_post(semaphore1);
			sem_wait(semaphore2);
			cout << "Reservation done!" << endl;

		}
	}

	shmdt(semaphore1);
	shmdt(semaphore2);
	shmdt(buffer);
	return 0;
}