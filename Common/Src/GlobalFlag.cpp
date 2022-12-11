/*
 * GlobalFlag.cpp
 *
 * Global Flag
 *
 * Created on: Nov 29, 2022
 * Author(s): Gordon Fountain
 */

#include "../Inc/GlobalFlag.hpp"

void GlobalFlag::writeFlag(bool new_value) {
    // Take spot in line for service
    xSemaphoreTake(service_queue, TIMEOUT);
    // Once at front take the resource mutex
    xSemaphoreTake(flag_resource_mux, TIMEOUT);
    // Let next in line get served now that resource is held
    xSemaphoreGive(service_queue);
    // Write to the flag
    flag = new_value;
    // Release resource mux
    xSemaphoreGive(flag_resource_mux);
}

bool GlobalFlag::readFlag() {
    // Take spot in line for service
    xSemaphoreTake(service_queue, TIMEOUT);
    // Once at front of line take the reader count mutex
    xSemaphoreTake(reader_count_mux, TIMEOUT);
    // Increment the number of readers accessing resource
    reader_count++;
    // If this is the first reader then take the resource mutex
    if (reader_count == 1) {
        xSemaphoreTake(flag_resource_mux, TIMEOUT);
    }
    // Let the next call get service
    xSemaphoreGive(service_queue);
    // Release the reader count mutex to the next reader
    xSemaphoreGive(reader_count_mux);
    // Store the flag value
    bool return_value = flag;
    // Get the reader count mutex
    xSemaphoreTake(reader_count_mux, TIMEOUT);
    // Decrement the number of readers
    reader_count--;
    // Check if all readers are finished to release resource
    if (reader_count == 0) {
        xSemaphoreGive(flag_resource_mux);
    }
    // Release reader count mutex
    xSemaphoreGive(reader_count_mux);
    // Return the stored value
    return return_value;
}
