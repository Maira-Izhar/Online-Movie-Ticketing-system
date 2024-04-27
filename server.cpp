#include <iostream>
#include <sstream>
#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>
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
	int sem1id = shmget(5025, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	int sem2id = shmget(5026, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	int sem3id = shmget(5027, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	int idcs = shmget(5028, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	if (sem1id == -1 || sem2id == -1 || idcs == -1) {
		cout << "Error creating shared memory..." << endl;
		return 1;
	}
	sem_t* semaphore1;
	sem_t* semaphore2;
	sem_t* semaphore3;

	semaphore1 = (sem_t*)shmat(sem1id, NULL, 0);
	semaphore2 = (sem_t*)shmat(sem2id, NULL, 0);
	semaphore3 = (sem_t*)shmat(sem3id, NULL, 0);
	buff* buffer;
	buffer = (buff*)shmat(idcs, NULL, 0);
	sem_init(semaphore1, 1, 0);
	sem_init(semaphore2, 1, 0);
	sem_init(semaphore3, 1, 0);

	while (true) {

		buffer->check = true;
		int fd = open("movieTicketInfo.txt", O_RDWR);
		if (fd == -1) {
			cout << "Error opening file..." << endl;
			return 1;
		}
		ssize_t bytesRead = read(fd, &buffer->arr, sizeof(buffer->arr));
		if (bytesRead < 0) {
			cout << "Error reading from the file..." << endl;

		}

		buffer->arr[bytesRead] = '$';

		close(fd);

		sem_post(semaphore3);
		sem_wait(semaphore1);
		char temp1 = buffer->arr[0];
		int total = temp1;
		total -= 48;
		if (buffer->movie <= total) {
			int count = 0;
			int i = 0;
			while (true) {

				if (buffer->arr[i] == '\n') {
					count++;
				}
				if (buffer->arr[i] == '\n' && count == buffer->movie) {
					break;
				}

				i++;
			}
			int j = i;
			char temp;
			int k = 0, z = 0;
			count = 0;
			i++;
			int y = 0;
			for (; buffer->arr[i] != '\n'; i++) {
				if (buffer->arr[i] == '/') {
					count++;
					i++;
				}
				if (count == 1) {
					buffer->movieName[z++] = buffer->arr[i];
				}
				if (count == 2) {
					temp = buffer->arr[i];
					y *= 10;
					y += temp;
					y -= 48;
				}

			}
			buffer->movieName[z] = '\0';
			if (buffer->ticket <= y) {
				buffer->price = buffer->ticket * 500;
			}
			else {
				buffer->check = false;
				sem_post(semaphore2);
				sem_wait(semaphore1);
				cout << "check 1 false" << endl;
				continue;
			}
		}
		else {
			buffer->check = false;
			sem_post(semaphore2);
			sem_wait(semaphore1);
			cout << "check 2 false" << endl;
			continue;
		}

		sem_post(semaphore2);
		sem_wait(semaphore1);
		char name[5][50];
		int num[5];

		int count = 0;
		int j = 0, k = 0, z = 0;
		int y = 0, c = 0;


		for (int i = 2; buffer->arr[i] != '$'; i++) {
			if (buffer->arr[i] == '/') {
				i++;
				k = 0;
				while (buffer->arr[i] != '/' && count != total) {
					name[j][k++] = buffer->arr[i++];
					if (buffer->arr[i] == '/') {
						count++;
					}
				}
				name[j][k] = '\0';

				i++;
				char tem;
				y = 0;
				while (buffer->arr[i] != '\n' && c != total) {
					tem = buffer->arr[i++];
					y *= 10;
					y += tem;
					y -= 48;
					if (buffer->arr[i] == '\n') {
						c++;
					}
				}
				z = 0;
				i++;
				num[j] = y;
				j++;
				if (count == total && c == total) {
					break;
				}
			}

		}
		if (buffer->x == 'n') {
			continue;
		}
		else {
			num[buffer->movie - 1] -= buffer->ticket;
		}

		fd = open("movieTicketInfo.txt", O_WRONLY | O_TRUNC);
		if (fd == -1) {
			cout << "Error opening file..." << endl;
			return 1;
		}

		count = 1;
		string temp2;
		int numberofmovies = total;
		ostringstream oss;
		for (int i = 0; i < total; i++) {
			if (num[i] != 0) {

				oss << i + 1 << "/" << name[i] << "/" << num[i] << "\n";

			}
			else {
				numberofmovies--;
			}

		}
		char extra[2];
		extra[0] = numberofmovies + 48;
		extra[1] = '\n';
		string line = oss.str();
		write(fd, &extra, 2);
		write(fd, line.c_str(), line.size());

		int fdx = open("BookingInfo.txt", O_RDONLY);
		if (fdx == -1) {
			cout << "Error opening file..." << endl;
			return 1;
		}
		char hello;
		ssize_t br = read(fdx, &hello, 1);
		if (br <= 0) {
			close(fdx);
			int fd1 = open("BookingInfo.txt", O_WRONLY | O_APPEND);
			if (fd1 == -1) {
				cout << "Error opening file..." << endl;
				return 1;
			}
			ostringstream oss1;
			oss1 << 1 << "\n" << buffer->cnic << "/" << buffer->movie << "/" << buffer->ticket << "\n";
			string line1 = oss1.str();
			write(fd1, line1.c_str(), line1.size());

		}
		else {
			close(fdx);
			int fdy = open("BookingInfo.txt", O_WRONLY);
			if (fdy == -1) {
				cout << "Error opening file..." << endl;
				return 1;
			}
			int hello2 = hello - 47;

			ostringstream oss2;
			oss2 << hello2;
			string line2 = oss2.str();
			write(fdy, line2.c_str(), line2.size());


			close(fdy);
			int fd1 = open("BookingInfo.txt", O_WRONLY | O_APPEND);
			if (fd1 == -1) {
				cout << "Error opening file..." << endl;
				return 1;
			}
			ostringstream oss1;
			oss1 << buffer->cnic << "/" << buffer->movie << "/" << buffer->ticket << "\n";
			string line1 = oss1.str();
			write(fd1, line1.c_str(), line1.size());
		}

		sem_post(semaphore2);

	}

	sem_destroy(semaphore1);
	sem_destroy(semaphore2);
	shmdt(semaphore1);
	shmdt(semaphore2);
	shmdt(buffer);
	shmctl(sem1id, IPC_RMID, NULL);
	shmctl(sem2id, IPC_RMID, NULL);
	shmctl(idcs, IPC_RMID, NULL);

	return 0;
}
