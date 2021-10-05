/**
 *  Name:   binary_packet.cpp
 *  Author: Henry Tsai
 *  Description: 
 *   - Helper functions for converting 32-bit floats to bitsets and vice versa
 *   - Helper function for calculating Fletcher-16 checksums
 *   - Functions for creating and reading binary packets
 */

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;

// int packet_count = 0;
const int NUM_FIELDS = 3; // Temporary variable for testing purposes
const int PACKET_SIZE = 56 + 32 * NUM_FIELDS; // Size of packet in bits
const string FILENAME = "logs.txt";

bitset<32> floatToBinary(float float_value);
float binaryToFloat(bitset<32> bitset_value);
uint16_t Fletcher16(float* data, uint8_t num_fields);
bitset<PACKET_SIZE> createPacket(char id, float timestamp, uint8_t num_fields, float* data);
float* readPacket(bitset<PACKET_SIZE> packet, uint8_t num_fields);
// int getPacketCount(){ return packet_count; }
// void incrementPacketCount(){ packet_count += 1; }

int main(){
    char test_id = 'C';
    float test_float = 1.23456;
    float test_timestamp = 420.690;
    float test_data[NUM_FIELDS] = {3.14159265, 2.7182, 1.99999};
    bitset<32> test_binary = floatToBinary(test_float);
    bitset<PACKET_SIZE> test_packet = createPacket(test_id, test_timestamp, NUM_FIELDS, test_data);
    float* read_data = readPacket(test_packet, NUM_FIELDS);
    string data_dashes(NUM_FIELDS * 32 - 5, '-');

    cout << "\n------------------- Test Conversion Functions -------------------" << endl;
    cout << "Value (original float):          " << test_float << endl;
    cout << "Value (converted to binary):     " << test_binary << endl;
    cout << "Value (converted back to float): " << binaryToFloat(test_binary) << endl;

    cout << "\n-------------------  Test Packet Functions  ---------------------" << endl;
    cout << "ID (char):          " << test_id << endl;
    cout << "Timestamp (float):  " << test_timestamp << endl;
    cout << "ID (binary):        " << bitset<8>(test_id) << endl;
    cout << "Timestamp (binary): " << floatToBinary(test_timestamp) << endl;
    cout << "Packet in binary: " << endl;
    cout << "ID-----|" << "Timestamp----------------------|Data" << data_dashes
         << "|Checksum-------|\n" << test_packet << endl;
    cout << "\nDecoded packet data: " << endl;
    for (int i = 0; i < NUM_FIELDS; i++){
        cout << "Field #" << i + 1 << ": " << read_data[i] << endl;
    }
    cout << "Successfully wrote Packet " << test_id << " to " << FILENAME << endl;
}

///****************************************************************
// Parameters: float_value (32-bit float)
// Behavior:   Converts float_value into a 32-bit bitset using
//             IEEE 754 convention
// Returns:    32-bit bitset representation of float_value
///****************************************************************
bitset<32> floatToBinary(float float_value){
    union{
        float input;
        int output;
    } data;
    data.input = float_value;
    return bitset<32>(data.output);
}

///***************************************************************
// Parameters: bitset_value (32-bit bitset)
// Behavior:   Converts bitset_value into a 32-bit float using
//             IEEE 754 convention
// Returns:    32-bit float represented by bitset_value
///***************************************************************
float binaryToFloat(bitset<32> bitset_value){
    // TODO: remove temporary string casting
    string bitset_string = bitset_value.to_string();
    bitset<32> set(bitset_string);
    int hex_number = set.to_ulong();
    int exponent = ((hex_number & 0x7f800000) >> 23) - 127;
    int sign = !!(hex_number & 0x80000000) ? -1 : 1;
    float total = 1.0;

    for (int i = 0; i < 23; i++){
        int c = bitset_string[i + 9] - '0';
        total += (float) c * (float) pow(2.0, (i + 1) * -1);
    }

    return sign * (float) pow(2.0, exponent) * total;
}

///***************************************************************
// Parameters: data (pointer to a float array of data fields), 
//             num_fields (8-bit int)
// Behavior:   Calculates the Fletcher-16 checksum of the data array
// Returns:    checksum (16-bit unsigned int)
///***************************************************************
uint16_t Fletcher16(float* data, uint8_t num_fields){
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    for (int i = 0; i < num_fields; i++){
        if (data[i] > 0){
            // TODO: remove casting of float to uint8 to avoid data loss
            sum1 = (sum1 + (uint8_t) data[i]) % 255;
            sum2 = (sum2 + sum1) % 255;
        }
    }
    return (sum2 << 8) | sum1;
}

///***************************************************************
// Parameters: id (8-bit char), timestamp (32-bit float),
//             num_fields (8-bit int), data (array of 32-bit floats)
// Behavior:   Converts id, timestamp, and data into bitsets
//             using IEEE 754 convention and then calculates a
//             Fletcher-16 checksum. Concatenates all of them into
//             a single PACKET_SIZE-bit bitset packet.
// Returns:    PACKET_SIZE-bit bitset packet
///***************************************************************
bitset<PACKET_SIZE> createPacket(char id, float timestamp, uint8_t num_fields, float* data){
    bitset<PACKET_SIZE> packet;
    bitset<8> binary_id = bitset<8>(id);
    bitset<32> binary_timestamp = floatToBinary(timestamp);
    bitset<16> binary_checksum = bitset<16>(Fletcher16(data, num_fields));

    for (int c = 0; c < 16; c++){
        packet[c] = binary_checksum[c];
    }
    for (int f = 0; f < num_fields; f++){
        bitset<32> field = floatToBinary(data[f]);
        for (int d = 0; d < 32; d++){
            packet[d + PACKET_SIZE - 72 - f * 32] = field[d];
        }
    }
    for (int t = 0; t < 32; t++){
        packet[t + PACKET_SIZE - 40] = binary_timestamp[t];
    }
    for (int i = 0; i < 8; i++){
        packet[i + PACKET_SIZE - 8] = binary_id[i];
    }

    return packet;
}

///***************************************************************
// Parameters: packet (PACKET_SIZE-bit bitset), num_fields (8-bit int)
// Behavior:   Decodes binary packet into char id, float
//             timestamp, float array of data fields and
//             int checksum. Saves data into a text file.
// Returns:    pointer to a float array of data fields
///***************************************************************
float* readPacket(bitset<PACKET_SIZE> packet, uint8_t num_fields){
    ofstream outputFile;
    // TODO: replace constant NUM_FIELDS with parameter num_fields (static may also be an issue)
    static float data[NUM_FIELDS];
    bitset<8> binary_id;
    bitset<16> binary_checksum;
    bitset<32> binary_timestamp;
    bitset<32> binary_data[num_fields];

    for (int c = 0; c < 16; c++){
        binary_checksum[c] = packet[c];
    }
    for (int f = 0; f < num_fields; f++){
        for (int d = 0; d < 32; d++){
            binary_data[f][d] = packet[d + PACKET_SIZE - 72 - f * 32];
        }
    }
    for (int t = 0; t < 32; t++){
        binary_timestamp[t] = packet[t + PACKET_SIZE - 40];
    }
    for (int i = 0; i < 8; i++){
        binary_id[i] = packet[i + PACKET_SIZE - 8];
    }

    char packet_id = (char) binary_id.to_ulong();
    float timestamp = binaryToFloat(binary_timestamp);
    int checksum = (int) binary_checksum.to_ulong();
    outputFile.open(FILENAME, ios::app);

    outputFile << "---------------------------" << endl;
    outputFile << "Packet ID: " << packet_id << endl;
    outputFile << "Timestamp: " << timestamp << endl;
    outputFile << "Checksum:  " << checksum << endl;
    outputFile << "Data: " << endl;

    for (int i = 0; i < num_fields; i++){
        float field = binaryToFloat(binary_data[i]);
        data[i] = field;
        outputFile << "Field #" << i + 1 << ": " << field << endl;
    }

    outputFile.close();
    return data;
}
