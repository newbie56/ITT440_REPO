#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h> // Add the MySQL library

#define PORT 11235

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in socket");
        exit(1);
    }
    printf("Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
        perror("Error setting socket option");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(1);
    }
    printf("Binding success.\n");

    if (listen(sockfd, 10) == 0) {
        printf("Listening....\n");
    } else {
        perror("Listening failed");
        exit(1);
    }

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    conn = mysql_init(NULL);
    if (conn == NULL) {
        perror("MySQL initialization failed");
        exit(1);
    }
    const char *mysql_server_ip = getenv("MYSQL_SERVER_IP");
    if (mysql_server_ip == NULL) {
        perror("MYSQL_SERVER_IP environment variable not set");
        exit(1);
    }
    if (mysql_real_connect(conn, server01-mysql, "user", "123", "players", 30000, NULL, 0) == NULL) {
        perror("Connection to MySQL database failed");
        exit(1);
    }

    while (1) {
        addr_size = sizeof(new_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);

        // Receive the user name from the client
        recv(newsockfd, buffer, sizeof(buffer), 0);
        char user[50];
        sscanf(buffer, "%s", user);

        // Query to get the current points for the specific user
        char query[200];
        snprintf(query, sizeof(query), "SELECT points FROM players WHERE name = '%s'", user);

        if (mysql_query(conn, query) != 0) {
            perror("Query execution failed");
            exit(1);
        }

        res = mysql_store_result(conn);
        if (res == NULL) {
            perror("Result set retrieval failed");
            exit(1);
        }

        row = mysql_fetch_row(res);
        int current_points = atoi(row[0]);
        current_points++;

        // Query to update the points for the specific user
        snprintf(query, sizeof(query), "UPDATE players SET points = %d WHERE name = '%s'", current_points, user);
        if (mysql_query(conn, query) != 0) {
            perror("Query execution failed");
            exit(1);
        }

        // Send the updated points back to the client
        send(newsockfd, &current_points, sizeof(current_points), 0);

        mysql_free_result(res);
        close(newsockfd);

        sleep(30); // Wait for 30 seconds before processing the next client request
    }

    mysql_close(conn);
    close(sockfd);

    return 0;
}
