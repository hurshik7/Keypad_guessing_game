//
// Created by Shik Hur on 2022-10-28.
//

#include "rudp_v2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uint16_t generate_crc16(const char *data_p, size_t length) {
    unsigned char x;
    uint16_t crc = 0xFFFF;          // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    while (length--) {
        x = crc >> 8 ^
            *data_p++;   // NOLINT(hicpp-signed-bitwise, readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
        x ^= x
                >> 4;                // NOLINT(hicpp-signed-bitwise, readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
        crc = (crc << 8) ^ ((uint16_t) (x << 12)) ^ ((uint16_t) (x << 5)) ^
              ((uint16_t) x);   // NOLINT(hicpp-signed-bitwise, readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
    }
    return crc;
}

rudp_packet_t *create_rudp_packet_malloc(const rudp_header_t *header, const size_t data_length, const char *data) {
    rudp_packet_t *packet = (rudp_packet_t *) malloc(sizeof(rudp_packet_t));
    memcpy(&packet->header, header, sizeof(rudp_header_t));
    if (header->packet_type == RUDP_SYN) {
        packet->data_length = data_length;
        strncpy(packet->data, data, data_length);
        uint16_t check_sum = generate_crc16(data, packet->data_length);
        packet->check_sum = check_sum;
    } else {
        // RUDP_ACK or RDUP_FIN
        packet->data_length = 0;
        packet->check_sum = 0;
        memset(packet->data, 0, MAX_DATA_LENGTH);
    }
    // change values into network order
    packet->header.packet_type = htons(packet->header.packet_type);
    packet->header.seq_no = htonl(packet->header.seq_no);
    packet->data_length = htons(packet->data_length);
    packet->check_sum = htons(packet->check_sum);
    return packet;
}

void init_rudp_header(uint16_t type, uint32_t seq_no, rudp_header_t *header_out) {
    header_out->packet_type = type;
    header_out->seq_no = seq_no;
}

void deserialize_packet(rudp_packet_t *packet) {
    packet->header.packet_type = ntohs(packet->header.packet_type);
    packet->header.seq_no = ntohl(packet->header.seq_no);
    packet->data_length = ntohs(packet->data_length);
    packet->check_sum = ntohs(packet->check_sum);
}

// send rudp packet and recv ACK to the packet
int rudp_send(int sock_fd, struct sockaddr_in *to_addr, const char *data, size_t data_size, uint16_t packet_type) {
    static uint32_t current_seq = 0;
    ssize_t nwrote;
    ssize_t nread;
    rudp_packet_t response_packet;
    struct sockaddr_in from_addr;
    socklen_t from_addr_len = sizeof(struct sockaddr_in);

    if (packet_type == RUDP_INIT) {
        current_seq = 0;
        return RUDP_SEND_SUCCESS;
    }

    struct timeval tv;
    tv.tv_sec = TIMEOUT_IN_SECOND;
    tv.tv_usec = 0;
    // set timeout
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));

    // create rudp packet header
    rudp_header_t header;
    init_rudp_header(packet_type, current_seq, &header);

    // create rudp packet
    rudp_packet_t *packet = create_rudp_packet_malloc(&header, data_size, data);

    // sendto server
    nwrote = sendto(sock_fd, packet, sizeof(rudp_packet_t), 0, (const struct sockaddr *) to_addr,
                    sizeof(struct sockaddr_in));
    if (nwrote == -1) {
        free(packet);
        return RUDP_SEND_FAILURE;
    }

wait_response_packet:
    nread = recvfrom(sock_fd, &response_packet, sizeof(rudp_packet_t), 0, (struct sockaddr *) &from_addr,
                     &from_addr_len);
    if (nread == -1) {
        // timeout reached (3000 ms)
        fprintf(stdout,
                "\t[TIMEOUT]: The packet has been sent again\n");                                                   // NOLINT(cert-err33-c)
        nwrote = sendto(sock_fd, packet, sizeof(rudp_packet_t), 0, (const struct sockaddr *) to_addr,
                        sizeof(struct sockaddr_in));
        if (nwrote == -1) {
            free(packet);
            return RUDP_SEND_FAILURE;
        }
        goto wait_response_packet;
    }

    deserialize_packet(&response_packet);
    // if it receives NAK, or it receives ACK but the seq_no is not equal to the fin packet it sent, resend the fin packet.
    if (response_packet.header.packet_type == RUDP_NAK ||
        (response_packet.header.seq_no != current_seq || response_packet.header.packet_type != RUDP_ACK) ||
        from_addr.sin_addr.s_addr != to_addr->sin_addr.s_addr) {
        nwrote = sendto(sock_fd, packet, sizeof(rudp_packet_t), 0, (const struct sockaddr *) to_addr,
                        sizeof(struct sockaddr_in));
        if (nwrote == -1) {
            free(packet);
            return RUDP_SEND_FAILURE;
        }
        goto wait_response_packet;
    }

    fprintf(stdout, "\t[received ACK]\n");           // NOLINT(cert-err33-c)
    // increase sequence number and free the allocated packet.
    free(packet);
    current_seq++;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    // set timeout
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));
    return RUDP_SEND_SUCCESS;
}

// recv rudp packet and send response packet
int rudp_recv(int sock_fd, char *recv_data, struct sockaddr_in *from_addr) {
    rudp_packet_t packet_recv;
    static long current_seq_no = -1;
    socklen_t from_addr_len = sizeof(struct sockaddr_in);
    ssize_t nread;

    char buffer[MAX_DATA_LENGTH];

    nread = recvfrom(sock_fd, &packet_recv, sizeof(rudp_packet_t), 0, (struct sockaddr *) from_addr, &from_addr_len);
    if (nread == -1) {
        return RDUP_RECV_FAILURE;
    }

    deserialize_packet(&packet_recv);
    rudp_header_t response_packet_header;
    rudp_packet_t *response_packet;
    if (packet_recv.header.packet_type == RUDP_SYN) {
        if (packet_recv.header.seq_no == 0 && current_seq_no == -1) {
            fprintf(stdout, "[Start receiving a data from client]\n"); // NOLINT(cert-err33-c, concurrency-mt-unsafe)
            current_seq_no = 0;
        }

        // check if the data is corrupted or not.
        uint16_t check_sum_of_data_in_pack;
        check_sum_of_data_in_pack = generate_crc16(packet_recv.data, packet_recv.data_length);
        if (check_sum_of_data_in_pack != packet_recv.check_sum) {
            init_rudp_header(RUDP_NAK, packet_recv.header.seq_no, &response_packet_header);
            response_packet = create_rudp_packet_malloc(&response_packet_header, 0, NULL);
            sendto(sock_fd, response_packet, sizeof(rudp_packet_t), 0, (const struct sockaddr *) from_addr,
                   sizeof(struct sockaddr_in));
            free(response_packet);
            return RDUP_RECV_FAILURE;
        }

        // if the seq_no is different from the current seq_no, just ignore the packet
        if ((long) packet_recv.header.seq_no != current_seq_no) {
            return RDUP_RECV_FAILURE;
        }

        // if the sender sent the same packet with previous one, send ACK to client again for the same packet.
        // this situation only happens when the client can not receive the ACK of the packet from the server
        if (packet_recv.header.seq_no != current_seq_no) {
            init_rudp_header(RUDP_ACK, packet_recv.header.seq_no, &response_packet_header);
            response_packet = create_rudp_packet_malloc(&response_packet_header, 0, NULL);
            sendto(sock_fd, response_packet, sizeof(rudp_packet_t), 0, (const struct sockaddr *) from_addr,
                   sizeof(struct sockaddr_in));
            free(response_packet);
            return RDUP_RECV_FAILURE;
        }

        // copy data from the client, print it on stdout
        strncpy(buffer, packet_recv.data, packet_recv.data_length);
        buffer[packet_recv.data_length] = '\0';
        if (strlen(buffer) < MAX_DATA_LENGTH) {
            strncpy(recv_data, buffer, strlen(buffer));
        }

        // send ACK
        init_rudp_header(RUDP_ACK, packet_recv.header.seq_no, &response_packet_header);
        response_packet = create_rudp_packet_malloc(&response_packet_header, 0, NULL);
        sendto(sock_fd, response_packet, sizeof(rudp_packet_t), 0, (const struct sockaddr *) from_addr,
               sizeof(struct sockaddr_in));

        // free response packet_recv and increase current_seq_no
        current_seq_no++;
    } else if (packet_recv.header.packet_type == RUDP_FIN) {
        fprintf(stdout, "[Finish the data transmission]\n");    // NOLINT(cert-err33-c)
        // send ACT to FIN
        init_rudp_header(RUDP_ACK, packet_recv.header.seq_no, &response_packet_header);
        response_packet = create_rudp_packet_malloc(&response_packet_header, 0, NULL);
        sendto(sock_fd, response_packet, sizeof(rudp_packet_t), 0, (const struct sockaddr *) from_addr,
               sizeof(struct sockaddr_in));
        current_seq_no = -1;
    } else {
        return RDUP_RECV_FAILURE;
    }

    // free allocated memory (response_packet)
    free(response_packet);

    return RDUP_RECV_SUCCESS;
}
