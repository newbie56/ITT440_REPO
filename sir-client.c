#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 11235

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error in socket");
        exit(1);
    }
    printf("Client socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr("server02-c");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in connection");
        exit(1);
    }
    printf("Connected to Server.\n");

    while (1) {
        // Get the user name input from the user
        printf("Enter user name to get the latest points: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send the user name to the server
        send(client_socket, buffer, sizeof(buffer), 0);

        // Receive the updated points from the server
        int updated_points;
        recv(client_socket, &updated_points, sizeof(updated_points), 0);
        printf("Updated points for the user: %d\n", updated_points);

        sleep(30); // Wait for 30 seconds before sending the next request
    }

    close(client_socket);

    return 0;
}
