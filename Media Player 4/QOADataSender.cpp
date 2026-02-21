#include "QOADataSender.h"

QOADataSender::QOADataSender() {
    QOADataSender::samples_read = 0;
}




bool QOADataSender::openFile(QString fileName) {
    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file.is_open()) {
        emit QOADataSender::sendError("File failed to open, likely does not exist.", QOA_DATA);
        return false;
    }

    file.seekg(0, std::ios::end);
    size_t file_length = file.tellg();
    file.seekg(0, std::ios::beg);

    QOADataSender::data_read_offset = 0;

    //std::vector<char> data((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    QOADataSender::data.reserve(file_length);


    std::copy(std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(),
        std::back_inserter(QOADataSender::data));


    file.close();
    //std::string test(data,100);

    if (std::string(QOADataSender::data.begin(), QOADataSender::data.begin() + 4) != "qoaf") {
        emit QOADataSender::sendError("File is not actually QOA.", QOA_DATA);
        return false;
    }

    std::string test(QOADataSender::data.begin() + 4, QOADataSender::data.begin() + 4 + sizeof(uint32_t));


    //int sample_count = (uint32_t)data[4] << 24 | (uint32_t)data[5] << 16 | (uint32_t)data[6] << 8 | (uint32_t)data[7];
    QOADataSender::sample_count = convert_to_uint32(QOADataSender::data.begin() + 4);
    QOADataSender::frame_count = ceil(sample_count / (256 * 20));

    QOADataSender::data_read_offset = 8;
    
    return true;
}

void QOADataSender::splitAndSend(QString fileName, int readWriteCOM, int readOnlyCOM) {
    //open com ports
    std::wstringstream wide_string_stream; //to convert int to lpcwstr
    wide_string_stream << L"COM" << readWriteCOM;
    QOADataSender::read_write_COM_handle = CreateFile(wide_string_stream.str().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    wide_string_stream.str(L"");
    wide_string_stream.clear();
    wide_string_stream << L"COM" << readOnlyCOM;

    QOADataSender::dedicated_read_COM_handle = CreateFile(wide_string_stream.str().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (QOADataSender::read_write_COM_handle == INVALID_HANDLE_VALUE || QOADataSender::dedicated_read_COM_handle == INVALID_HANDLE_VALUE) {
        emit QOADataSender::sendError("COM Handles failed to open, one or both COM handles are invalid.", QOA_DATA);
        emit QOADataSender::badCOMPorts();
        emit QOADataSender::finished();
        return;
    }

    //setup communication params
    DCB serialParams = { 0 };
    if (!GetCommState(QOADataSender::read_write_COM_handle, &serialParams)) { //default comm state, setting both to this same params though
        emit QOADataSender::sendError("Failed to grab COM state of thr read/write COM port.", QOA_DATA);
        emit QOADataSender::badCOMPorts();
        emit QOADataSender::finished();
        return;
    }

    serialParams.BaudRate = BAUDRATE;
    serialParams.ByteSize = 8;
    serialParams.StopBits = 1;
    serialParams.Parity = NOPARITY;

    //apply params
    if (!SetCommState(QOADataSender::read_write_COM_handle, &serialParams) || !SetCommState(QOADataSender::dedicated_read_COM_handle, &serialParams)) {
        emit QOADataSender::sendError("Failed to set COM state of one or both COM ports.", QOA_DATA);
        emit QOADataSender::badCOMPorts();
        emit QOADataSender::finished();
        return;
    }

    //set up timeouts
    COMMTIMEOUTS timeouts = { 0 };
    //literally arbitrary values
    timeouts.ReadIntervalTimeout = 5;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(QOADataSender::read_write_COM_handle, &timeouts) || !SetCommTimeouts(QOADataSender::dedicated_read_COM_handle, &timeouts)) {
        emit QOADataSender::sendError("Failed to set timeouts of one or both COM ports.", QOA_DATA);
        emit QOADataSender::badCOMPorts();
        emit QOADataSender::finished();
        return;
    }

    unsigned long transferred_bytes = 0;
    
    if (!QOADataSender::openFile(fileName)) {
        emit QOADataSender::sendError("Failed to open file.", QOA_DATA);
        emit QOADataSender::finished();
        return;
    }

    //some setup for slices
    uint8_t lastSfQuant = 0;
    uint8_t history_weight_data_send_array[3] = {};
    uint8_t sf_quant[1] = {};


    uint64_t qr_send_full;
    uint8_t quantized_residual_data_send[8] = {};
    std::array<uint8_t, 40> received_bytes = {};

    bool data_received = true;
    bool round_robin_recieve = true;

    for (int i = 0; i < QOADataSender::frame_count; i++) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit QOADataSender::sendError("Thread terminated.", QOA_DATA);
            emit QOADataSender::finished();
            return;
        }

        uint8_t channel_num = QOADataSender::data[QOADataSender::data_read_offset];

        if (channel_num > 1) {
            emit QOADataSender::sendError("Channel count exceeds 1. Please reconsider.", QOA_DATA);
            emit QOADataSender::finished();

            return;
        }

        
        uint32_t samplerate = convert_to_uint24(QOADataSender::data.begin() + QOADataSender::data_read_offset + 1);
        
        if (i == 0) {
            emit QOADataSender::send_sample_count(samplerate, QOADataSender::sample_count);
        }
        uint16_t samples_per_channel = convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 4);
        

        uint16_t frame_size = convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 6);
        QOADataSender::data_read_offset = QOADataSender::data_read_offset + 8;

        //history and weights
        //history: 
        uint16_t history[4] = { convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset),
                               convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 2),
                                convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 4),
                            convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 6) };
        QOADataSender::data_read_offset = QOADataSender::data_read_offset + 8;
        //weights
        uint16_t weights[4] = { convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset),
                            convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 2),
                            convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 4),
                            convert_to_uint16(QOADataSender::data.begin() + QOADataSender::data_read_offset + 6) };
        QOADataSender::data_read_offset = QOADataSender::data_read_offset + 8;

        //send history
        int counter = 0;
        for (uint16_t history_value : history) {
            uint8_t byte1 = history_value & 0xFF;
            uint8_t byte2 = (history_value >> 8) & 0xFF;
            uint32_t whole = HISTORY | ((counter & 0b11) << 4) | byte1 << 6 | byte2 << 14;
            history_weight_data_send_array[0] = whole & 0xFF;
            history_weight_data_send_array[1] = (whole >> 8) & 0xFF;
            history_weight_data_send_array[2] = (whole >> 16) & 0xFF;

            //send the data
            if (!WriteFile(QOADataSender::read_write_COM_handle, history_weight_data_send_array, sizeof(history_weight_data_send_array), &transferred_bytes, NULL)) {
                emit QOADataSender::sendError("Failed to send history data.", QOA_DATA);
                emit QOADataSender::finished();
                return;
            }

            if (transferred_bytes < 3) {
                emit QOADataSender::sendError("Less than 3 bytes were transferred when sending history data.", QOA_DATA);
                emit QOADataSender::finished();
                return;
            }

            counter++;
        }
        //send weights
        counter = 0;
        for (uint16_t weight_value : weights) {
            uint8_t byte1 = weight_value & 0xFF;
            uint8_t byte2 = (weight_value >> 8) & 0xFF;
            uint32_t whole = WEIGHT | ((counter & 0b11) << 4) | byte1 << 6 | byte2 << 14;
            history_weight_data_send_array[0] = whole & 0xFF;
            history_weight_data_send_array[1] = (whole >> 8) & 0xFF;
            history_weight_data_send_array[2] = (whole >> 16) & 0xFF;

            //send the data
            if (!WriteFile(QOADataSender::read_write_COM_handle, history_weight_data_send_array, sizeof(history_weight_data_send_array), &transferred_bytes, NULL)) {
                emit QOADataSender::sendError("Failed to send weight data.", QOA_DATA);
                emit QOADataSender::finished();
                return;
            }

            if (transferred_bytes < 3) {
                emit QOADataSender::sendError("Less than 3 bytes were transferred when sending weight data.", QOA_DATA);
                emit QOADataSender::finished();
                return;
            }

            counter++;
        }

        //slices... oh god
        for (int slice_iter = 0; slice_iter < SLICES_PER_FRAME; slice_iter++) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                emit QOADataSender::sendError("Thread terminated", QOA_DATA);
                emit QOADataSender::finished();
                return;
            }

            //int binary[64];
            uint64_t slice = convert_to_uint64(QOADataSender::data.begin() + QOADataSender::data_read_offset);
            QOADataSender::data_read_offset = QOADataSender::data_read_offset + 8;
            sf_quant[0] = (SF_QUANT_INDICATOR) | ((slice >> 60) & 0x0F) << 4;
            

            if ((i == 0 && slice_iter == 0) || sf_quant[0] != lastSfQuant) {
                //send sf_quant
                if (!WriteFile(QOADataSender::read_write_COM_handle, sf_quant, sizeof(sf_quant), &transferred_bytes, NULL)) {
                    emit QOADataSender::sendError("Failed to send sf_quant data.", QOA_DATA);
                    emit QOADataSender::finished();
                    return;
                }
                if (transferred_bytes < 1) {
                    emit QOADataSender::sendError("The singular byte that is the sf_quant data failed to be transferred.", QOA_DATA);
                    emit QOADataSender::finished();
                    return;
                }


            }

            lastSfQuant = sf_quant[0];
            qr_send_full = QR_INDICATOR;
            for (int qr = 0; qr < 20; qr++) {
                if (qr == 0) {
                    qr_send_full = (qr_send_full) | (((slice >> 57) & 0x07) << 4);
                }
                else {
                    qr_send_full = (qr_send_full << (0) | (((slice >> 57) & 0x07) << (3 * (qr + 1) + 1)));
                }

                
                slice = slice << 3;
            }

            

            for (int byte = 0; byte < 8; byte++) {
                quantized_residual_data_send[byte] = (qr_send_full) >> ((byte * 8));

            }
            if (!WriteFile(QOADataSender::read_write_COM_handle, quantized_residual_data_send, sizeof(quantized_residual_data_send), &transferred_bytes, NULL)) {
                emit QOADataSender::sendError("Failed to send quantized residuals.", QOA_DATA);
                emit QOADataSender::finished();
                return;
            }

            if (transferred_bytes < 8) {
                emit QOADataSender::sendError("Failed to transfer all bytes of quantized residual data.", QOA_DATA);
                emit QOADataSender::finished();
                return;
            }

 
            //recieve samples
            data_received = false;
            for (int receive_attempts = 0; receive_attempts < 6; receive_attempts++) {
                if (receive_attempts >= 3) {
                    round_robin_recieve = !round_robin_recieve;
                }

                
                //transferred_bytes = 0;

                if (round_robin_recieve) {

                    if (!ReadFile(QOADataSender::read_write_COM_handle, received_bytes.data(), sizeof(received_bytes), &transferred_bytes, NULL)) {
                        emit QOADataSender::sendError("Failed to receive data through the read/write COM port.", QOA_DATA);
                        continue;
                    }
                    
                    round_robin_recieve = !round_robin_recieve;
                    data_received = true;
                }
                else {
                    if (!ReadFile(QOADataSender::dedicated_read_COM_handle, received_bytes.data(), sizeof(received_bytes), &transferred_bytes, NULL)) {
                        emit QOADataSender::sendError("Failed to receive data through the dedicated read port.", QOA_DATA);
                        continue;
                    }
                    
                    round_robin_recieve = !round_robin_recieve;
                    data_received = true;
                }
                if (transferred_bytes < 40) {
                    emit QOADataSender::sendError("Failed to receive all bytes of data from the decoder.", QOA_DATA);
                    data_received = false;
                    continue;
                }
                else {
                    break;
                }
                
            }
           
            
            if (!data_received) {
                emit QOADataSender::finished();
                return;
            }
            
            emit QOADataSender::send_decoded_samples(received_bytes);
            

            QOADataSender::samples_read = QOADataSender::samples_read + 20;

            if (QOADataSender::samples_read >= QOADataSender::sample_count) {
                emit QOADataSender::finished();
                return;
            }
 
        }

        emit QOADataSender::finished_frame(i);


    }
    emit QOADataSender::finished();
    return;

}


QOADataSender::~QOADataSender() {
    CloseHandle(QOADataSender::read_write_COM_handle);
    CloseHandle(QOADataSender::dedicated_read_COM_handle);
}
