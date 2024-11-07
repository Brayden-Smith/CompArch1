//On my honor, I have neither given nor received any unauthorized aid on this assignment.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <bitset>
#include <tuple>
#include <queue>
#include <regex>

using namespace std;


//convert using 2's complement
int binaryToDecimal(string binary) {
    string result = binary;
    if(result[0] == '0') {
        return stoll(result, nullptr, 2);
    }
    else {
        for (int i = 0; i < binary.length(); i++) {
            //flip bits
            if (binary[i] == '0') {
                result[i] = '1';
            } else {
                result[i] = '0';
            }
        }
        return -1 * (stoll(result, nullptr, 2) + 1);
    }
}

//convert
string decimalToBinary(int decimal) {
    bitset<32> bits(decimal);
    return bits.to_string();
}

//turn binary instruction into english
string disassemble(string instruction) {
    //get the least significant two bits
    string LSBs = instruction.substr(instruction.length() - 2,2);

    string result;

    switch (stoi(LSBs, nullptr, 2)) {

        case 0: { //00 Category-4
            string sub = instruction.substr(instruction.length() - 7, 5);
            if(sub == "11111") { //break instruction
                result.append("break");
            }
            else {
                result.append("jal ");

                string rd = instruction.substr(instruction.length() - 12, 5);
                result.append("x" + to_string(stoi(rd, nullptr, 2)) + ", ");

                string imm = instruction.substr(instruction.length() - 32, 20);
                int immD = binaryToDecimal(imm);
                result.append("#" + to_string(immD));
            }
            break;
        }

        case 1: { //01 Category-2
            string opcode;
            string sub = instruction.substr(instruction.length() - 7, 5);
            switch (stoi(sub, nullptr, 2)) {
                case 0:
                    opcode = "add ";
                    break;
                case 1:
                    opcode = "sub ";
                    break;
                case 2:
                    opcode = "and ";
                    break;
                case 3:
                    opcode = "or ";
                    break;
            }
            string rd = instruction.substr(instruction.length() - 12, 5);
            result.append(opcode);
            result.append("x" + to_string(stoi(rd, nullptr, 2)) + ", ");

            string rs1 = instruction.substr(instruction.length() - 20, 5);
            string rs2 = instruction.substr(instruction.length() - 25, 5);

            //have to convert binary to hexadecimal then back into string
            result.append("x" + to_string(stoi(rs1, nullptr, 2)) + ", ");
            result.append("x" + to_string(stoi(rs2, nullptr, 2)));
            break;
        }

        case 2: { //10 Category-3
            string opcode;
            string sub = instruction.substr(instruction.length() - 7, 5);
            switch (stoi(sub, nullptr, 2)) {
                case 0:
                    opcode = "addi ";
                    break;
                case 1:
                    opcode = "andi ";
                    break;
                case 2:
                    opcode = "ori ";
                    break;
                case 3:
                    opcode = "sll ";
                    break;
                case 4:
                    opcode = "sra ";
                    break;
                case 5:
                    opcode = "lw ";
                    break;
            }
            result.append(opcode);
            if (opcode == "lw ") {
                string rd = instruction.substr(instruction.length() - 12, 5);
                string rs1 = instruction.substr(instruction.length() - 20, 5);

                result.append("x" + to_string(stoi(rd, nullptr, 2)) + ", ");

                string imm = instruction.substr(instruction.length() - 32, 12);
                int immD = binaryToDecimal(imm);
                result.append(to_string(immD));
                result.append("(x" + to_string(stoi(rs1, nullptr, 2)) + ')');
            }
            else {
                string rd = instruction.substr(instruction.length() - 12, 5);
                string rs1 = instruction.substr(instruction.length() - 20, 5);

                result.append("x" + to_string(stoi(rd, nullptr, 2)) + ", ");
                result.append("x" + to_string(stoi(rs1, nullptr, 2)) + ", ");

                string imm = instruction.substr(instruction.length() - 32, 12);
                int immD = binaryToDecimal(imm);
                result.append("#" + to_string(immD));
            }
            break;
        }

        case 3: { //11 Category-1
            string opcode;
            string sub = instruction.substr(instruction.length() - 7, 5);
            switch (stoi(sub, nullptr, 2)) {
                case 0:
                    opcode = "beq ";
                    break;
                case 1:
                    opcode = "bne ";
                    break;
                case 2:
                    opcode = "blt ";
                    break;
                case 3:
                    opcode = "sw ";
                    break;
            }
            result.append(opcode);
            if (opcode == "sw ") { //store word has different format
                string rs2 = instruction.substr(instruction.length() - 25, 5);
                string rs1 = instruction.substr(instruction.length() - 20, 5);

                result.append("x" + to_string(stoi(rs1, nullptr, 2)) + ", ");

                string imm = instruction.substr(instruction.length() - 32, 7) + instruction.substr(instruction.length() - 12, 5);
                int immD = binaryToDecimal(imm);
                result.append(to_string(immD));
                result.append("(x" + to_string(stoi(rs2, nullptr, 2)) + ')');
            }
            else {
                string rs2 = instruction.substr(instruction.length() - 25, 5);
                string rs1 = instruction.substr(instruction.length() - 20, 5);

                result.append("x" + to_string(stoi(rs1, nullptr, 2)) + ", ");
                result.append("x" + to_string(stoi(rs2, nullptr, 2)) + ", ");

                string imm = instruction.substr(instruction.length() - 32, 7) + instruction.substr(instruction.length() - 12, 5);
                int immD = binaryToDecimal(imm);
                result.append("#" + to_string(immD));
            }




            break;
        }


    }
    return result;
}

string printoutfix(vector<string> queue, int index) {
    if (queue[index] == "") {
        return "";
    }
    else {
        return " [" + queue[index] + "]";
    }
}

//make the printout of current register and memory state
void simulation_printOut(ofstream &file,int cycle, int address, vector<int> registers, vector<tuple<int,int>> &Data,
                         vector<vector<string>>& WBqueues, vector<string>& preMEM, vector<string> ALU1, vector<string> ALU2,
                         vector<string> issue, vector<string> IFUnit) {

    vector<string> temp;
    for(int i = 0; i < 20; i++) {
        file << "-";
    }
    file << endl;

    file << "Cycle " <<  cycle << ":" << endl << endl;

    file << "IF Unit:" << endl;
    file << "\t" << "Waiting:" << printoutfix(IFUnit, 0) << endl;
    file << "\t" << "Executed:" << printoutfix(IFUnit, 1) << endl;

    file << "Pre-Issue Queue:" << endl;
    file << "\t" << "Entry 0:" << printoutfix(issue, 0) << endl;
    file << "\t" << "Entry 1:" << printoutfix(issue, 1) << endl;
    file << "\t" << "Entry 2:" << printoutfix(issue, 2) << endl;
    file << "\t" << "Entry 3:" << printoutfix(issue, 3) << endl;

    file << "Pre-ALU1 Queue:" << endl;
    file << "\t" << "Entry 0:" << printoutfix(ALU1, 0) << endl;
    file << "\t" << "Entry 1:" << printoutfix(ALU1, 1) << endl;

    file << "Pre-MEM Queue:" << printoutfix(preMEM, 0) << endl;
    file << "Post-MEM Queue:" << printoutfix(WBqueues[1], 0) << endl;

    file << "Pre-ALU2 Queue:" << endl;
    file << "\t" << "Entry 0:" << printoutfix(ALU2, 0)<< endl;
    file << "\t" << "Entry 1:" << printoutfix(ALU2, 1) << endl;

    file << "Post-ALU2 Queue:" << printoutfix(WBqueues[0], 0) << endl << endl;


    file << "Registers" << endl;
    file << "x00:" <<  "\t" << registers[0] <<  "\t" << registers[1] <<  "\t" << registers[2] <<  "\t" << registers[3] <<  "\t" << registers[4] <<  "\t" << registers[5] <<  "\t" << registers[6] <<  "\t" << registers[7] << endl;
    file << "x08:" <<  "\t" << registers[8] <<  "\t" << registers[9] <<  "\t" << registers[10] <<  "\t" << registers[11] <<  "\t" << registers[12] <<  "\t" << registers[13] <<  "\t" << registers[14] <<  "\t" << registers[15] << endl;
    file << "x16:" <<  "\t" << registers[16] <<  "\t" << registers[17] <<  "\t" << registers[18] <<  "\t" << registers[19] <<  "\t" << registers[20] <<  "\t" << registers[21] <<  "\t" << registers[22] <<  "\t" << registers[23] << endl;
    file << "x24:" <<  "\t" << registers[24] <<  "\t" << registers[25] <<  "\t" << registers[26] <<  "\t" << registers[27] <<  "\t" << registers[28] <<  "\t" << registers[29] <<  "\t" << registers[30] <<  "\t" << registers[31] << endl;
    file << endl << "Data" << endl;

    for(int i = 0; i < Data.size(); i++) {
        if(i % 8 == 0) {
            file << get<0>(Data[i]) << ":";
        }
        file << "\t" << get<1>(Data[i]);
        if(i % 8 == 7) {
            file << endl;
        }
    }
}

//return result of load
vector<int> execute_MEMUnit(string cur_Instruction, vector<int> registers, vector<tuple<int,int>> &Data) {
    string cur_Opcode;
    istringstream stream(cur_Instruction);
    stream >> cur_Opcode;

    //sw
    if (cur_Opcode == "sw") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_1 + 1);
        pos_2 = cur_Instruction.find(')');
        string rs2 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find(',');
        pos_2 = cur_Instruction.find('(');
        string imm = cur_Instruction.substr(pos_1 + 2, pos_2 - pos_1 - 2);


        int offset = registers[stoi(rs2)] + stoi(imm);


        bool exists = false;
        for (int i = 0; i < Data.size(); i++) {
            if (get<0>(Data[i]) == offset) {
                exists = true;
                Data[i] = make_tuple(offset, registers[stoi(rs1)]);
                break;
            }
        }

        if (!exists) {
            while (get<0>(Data[Data.size() - 1]) < offset) {
                Data.emplace_back(make_tuple(4 + get<0>(Data[Data.size() - 1]), 0));
            }
            Data[Data.size() - 1] = make_tuple(offset, registers[stoi(rs1)]);
        }
        return {-1};
    }

    //lw
    if (cur_Opcode == "lw") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_1 + 1);
        pos_2 = cur_Instruction.find(')');
        string rs2 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find(',');
        pos_2 = cur_Instruction.find('(');
        string imm = cur_Instruction.substr(pos_1 + 2, pos_2 - pos_1 - 2);


        int offset = registers[stoi(rs2)] + stoi(imm);

        int result;
        bool exists = false;
        for (int i = 0; i < Data.size(); i++) {
            if (get<0>(Data[i]) == offset) {
                exists = true;
                result = get<1>(Data[i]);
                break;
            }
        }

        if (!exists) {
            while (get<0>(Data[Data.size() - 1]) < offset) {
                Data.emplace_back(make_tuple(4 + get<0>(Data[Data.size() - 1]), 0));
            }
            result = 0;
        }
        return {stoi(rs1), result};
    }
    return {};
}

vector<int> execute_ALU2Unit(string cur_Instruction, vector<int> registers, vector<tuple<int,int>> &Data) {
    string cur_Opcode;
    istringstream stream(cur_Instruction);
    stream >> cur_Opcode;
    int result;

    //add
    if(cur_Opcode == "add") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        string rs2 = cur_Instruction.substr(pos_1 + 1);

        result = registers[stoi(rs1)] + registers[stoi(rs2)];
        return {stoi(rd), result};
    }

    //sub
    if(cur_Opcode == "sub") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        string rs2 = cur_Instruction.substr(pos_1 + 1);

        result = registers[stoi(rs1)] - registers[stoi(rs2)];
        return {stoi(rd), result};
    }

    //and
    if(cur_Opcode == "and") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        string rs2 = cur_Instruction.substr(pos_1 + 1);

        result = registers[stoi(rs1)] & registers[stoi(rs2)];
        return {stoi(rd), result};
    }

    //or
    if(cur_Opcode == "or") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        string rs2 = cur_Instruction.substr(pos_1 + 1);

        result = registers[stoi(rs1)] | registers[stoi(rs2)];
        return {stoi(rd), result};
    }

    //addi
    if(cur_Opcode == "addi") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('#', pos_2);
        string imm = cur_Instruction.substr(pos_1 + 1);

        result = registers[stoi(rs1)] + stoi(imm);
        return {stoi(rd), result};
    }

    //andi
    if(cur_Opcode == "andi") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('#', pos_2);
        string imm = cur_Instruction.substr(pos_1 + 1);

        result = registers[stoi(rs1)] & stoi(imm);
        return {stoi(rd), result};
    }

    //ori
    if(cur_Opcode == "ori") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('#', pos_2);
        string imm = cur_Instruction.substr(pos_1 + 1);

        result = registers[stoi(rs1)] | stoi(imm);
        return {stoi(rd), result};
    }

    //sll
    if(cur_Opcode == "sll") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('#', pos_2);
        string imm = cur_Instruction.substr(pos_1 + 1);

        rs1 = decimalToBinary(registers[stoi(rs1)]);
        for(int i = 0; i < stoi(imm); i++) {
            rs1.append("0");
        }
        rs1.erase(0,stoi(imm));

        result = binaryToDecimal(rs1);
        return {stoi(rd), result};
    }

    //sra
    if(cur_Opcode == "sra") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('#', pos_2);
        string imm = cur_Instruction.substr(pos_1 + 1);

        int rs = registers[stoi(rs1)];
        for(int i = 0; i < stoi(imm); i++) {
            rs = rs >> 1;
        }

        result = rs;
        return {stoi(rd), result};
    }
    return {};
}

void execute_instruction(int &address, vector<int> &registers,vector<tuple<int,int>> &Data, string cur_Instruction) {
    string cur_Opcode;
    istringstream stream(cur_Instruction);
    stream >> cur_Opcode;



    if(cur_Opcode == "jal") {
        //we don't know if one or two digits so just extract between these two
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rd = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);
        //save the skipped address at rd
        registers[stoi(rd)] = address + 4;

        //calculate offset
        pos_1 = cur_Instruction.find('#');
        int imm = stoi(cur_Instruction.substr(pos_1 + 1));
        string offset = decimalToBinary(imm);
        offset.push_back('0');
        address += binaryToDecimal(offset) - 4;
        return;
    }

    //beq
    if(cur_Opcode == "beq") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs2 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        //if equal take branch
        if (registers[stoi(rs1)] == registers[stoi(rs2)]) {
            pos_1 = cur_Instruction.find('#');
            int imm = stoi(cur_Instruction.substr(pos_1 + 1));
            string offset = decimalToBinary(imm);
            offset.push_back('0');
            address += binaryToDecimal(offset) - 4;
        }
        else {
            return;
        }
    }

    //bne
    if(cur_Opcode == "bne") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs2 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        if (registers[stoi(rs1)] != registers[stoi(rs2)]) {
            pos_1 = cur_Instruction.find('#');
            int imm = stoi(cur_Instruction.substr(pos_1 + 1));
            string offset = decimalToBinary(imm);
            offset.push_back('0');
            address += binaryToDecimal(offset) - 4;
        }
        else {
            return;
        }
    }

    //blt
    if(cur_Opcode == "blt") {
        int pos_1 = cur_Instruction.find('x');
        int pos_2 = cur_Instruction.find(',');
        string rs1 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);

        pos_1 = cur_Instruction.find('x', pos_2);
        pos_2 = cur_Instruction.find(',', pos_1);
        string rs2 = cur_Instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);


        if (binaryToDecimal(decimalToBinary(registers[stoi(rs1)])) < binaryToDecimal(decimalToBinary(registers[stoi(rs2)]))) {
            pos_1 = cur_Instruction.find('#');
            int imm = stoi(cur_Instruction.substr(pos_1 + 1));
            string offset = decimalToBinary(imm);
            offset.push_back('0');
            address += binaryToDecimal(offset)-4;
        }
        else {
            return;
        }
    }
}

bool empty_queueSlot(vector<string> queue) {
    for(int i = 0; i < queue.size(); i++) {
        if (queue[i] == "") {
            return true;
        }
    }
    return false;
}

//return the destination and read registers from an instruction
vector<vector<int>> registers_pull(string instruction) {

    vector<int> read;

    string cur_Opcode;
    istringstream stream(instruction);
    stream >> cur_Opcode;

    //these dont have destination registers
    if(cur_Opcode == "beq" || cur_Opcode == "blt" || cur_Opcode == "bne" || cur_Opcode == "sw") {
        regex pattern("x(\\d+)");
        sregex_iterator iter(instruction.begin(), instruction.end(), pattern);
        sregex_iterator end;

        while (iter != end) {
            read.push_back(std::stoi((*iter)[1].str()));
            ++iter;
        }
        return {{}, read};
    }
    //the instruction does have a destination
    else {
        int pos_1 = instruction.find('x');
        int pos_2 = instruction.find(',');
        string rd = instruction.substr(pos_1 + 1, pos_2 - pos_1 - 1);
        int destination = stoi(rd);


        regex pattern("x(\\d+)");
        sregex_iterator iter(instruction.begin() + pos_2, instruction.end(), pattern);
        sregex_iterator end;

        while (iter != end) {
            read.push_back(std::stoi((*iter)[1].str()));
            ++iter;
        }
        return {{destination}, read};
    }
}

void lowerScoreboard(string instruction, vector<vector<int>> &scoreboards) {
    //figure out what destinations or sources we are removing
    vector<vector<int>> entree = registers_pull(instruction);

    //if there is a destination register
    if(entree[0].empty() != true) {
        //remove one from that register count in the write scoreboard
        scoreboards[1][entree[0][0]] -= 1;
    }
    for(int i = 0; i < entree[1].size(); i++) {
        //remove one from the source registers
        scoreboards[0][entree[1][i]] -= 1;
    }
}

void raiseScoreboard(string instruction, vector<vector<int>> &scoreboards) {
//figure out what destinations or sources we are adding
    vector<vector<int>> entree = registers_pull(instruction);

    //if there is a destination register
    if(entree[0].empty() != true) {
        //add one from that register count in the write scoreboard
        scoreboards[1][entree[0][0]] += 1;
    }
    for(int i = 0; i < entree[1].size(); i++) {
        //add one from the source registers
        scoreboards[0][entree[1][i]] += 1;
    }
}

bool checkScoreboard(string instruction, vector<vector<int>> &scoreboards) {
    vector<vector<int>> check = registers_pull(instruction);

    //if there is a destination register
    if(check[0].empty() != true) {
        //check if destination is used in read score board
        if(scoreboards[0][check[0][0]] > 0) {
            //WAR hazard
            return true;
        }
        //check if destination is used in write score board
        if(scoreboards[1][check[0][0]] > 0) {
            //WAW hazard
            return true;
        }
    }
    for(int i = 0; i < check[1].size(); i++) {
        //add one from the source registers
        if(scoreboards[1][check[1][i]] > 0) {
            //RAW hazard
            return true;
        }
    }

    //no hazard
    return false;
}

void pop_element(vector<string>& queue) {
    queue.erase(queue.begin());
    queue.push_back("");
}

void push_element(vector<string>& queue, string element) {
    for(int i = 0; i < queue.size(); i++) {
        if (queue[i] == "") {
            queue[i] = element;
            break;
        }
    }
}

void pseudopop(vector<string>& queue, int index) {
    queue.erase(queue.begin() + index);
    queue.push_back("");
}

bool simulate_cycle(int &address, vector<string> &instructions, vector<int> &registers,vector<tuple<int,int>> &Data
                    , vector<vector<string>>& WBqueues, vector<string>& preMEM, vector<string>& ALU1, vector<string>& ALU2,
                    vector<string>& issue, vector<string>& IFUnit, vector<vector<int>>& scoreboards) {
    //WRITE BACK UNIT
    //for post mem and post alu2 queues set the destination register to the result of the prev units
    vector<string> clearInstructions;
    for(int i = 0; i < WBqueues.size(); i++) {
        //if there is an instruction in this queue
        if (WBqueues[i][0] != "") {
            int result = stoi(WBqueues[i][2]);
            registers[stoi(WBqueues[i][1])] = result;

            //store instructions so we can remove from score board later
            clearInstructions.push_back(WBqueues[i][0]);

            //set each queue back to default now that we have cleared it
            WBqueues[i] = {"","",""};
        }
    }

    //MEM UNIT
    if(preMEM[0] != "") {
        vector<int> MEMres = execute_MEMUnit(preMEM[0], registers, Data);
        //-1 implies instruction was sw and theres is nothin to move on not implies lw
        if (MEMres[0] != -1) {
            //move instruction, destination, and loaded data into post mem
            WBqueues[1][0] = preMEM[0];
            WBqueues[1][1] = to_string(MEMres[0]);
            WBqueues[1][2] = to_string(MEMres[1]);
        }
        //store instruction need to update scoreboard
        else {
            lowerScoreboard(preMEM[0],scoreboards);
        }
        preMEM[0] = "";
    }

    //ALU1 UNIT
    //just moves the oldest instruction in
    preMEM[0] = ALU1[0];
    pop_element(ALU1);

    //ALU2 UNIT
    if(ALU2[0] != "") {
        vector<int> ALUres = execute_ALU2Unit(ALU2[0], registers, Data);

        //add instruction and results to post alu queue
        WBqueues[0][0] = ALU2[0];
        WBqueues[0][1] = to_string(ALUres[0]);
        WBqueues[0][2] = to_string(ALUres[1]);
        pop_element(ALU2);
    }

    //ISSUE UNIT

        //remove all of issue queue from scoreboard so that they dont self hazard
        for(int i = 0; i < issue.size(); i++) {
            //since this is a pseudoqueue if this spot is empty theres nothing else
            if(issue[i] == "") {
                break;
            }
            //since it is still here it has not been issued which means it must be removed from scoreboard
            lowerScoreboard(issue[i], scoreboards);
        }
        //starts out being able to issue one of each instruction
        bool issueMem = true;
        bool issuearith = true;
        if(!empty_queueSlot(ALU1)) {
            issueMem = false;
        }
        if(!empty_queueSlot(ALU2)) {
            issuearith = false;
        }

        for(int i = 0; i < issue.size(); i++) {
            //since this is a pseudoqueue if this spot is empty theres nothing else
            if(issue[i] == "") {
                break;
            }
            else {
                string cur_Opcode;
                istringstream stream(issue[i]);
                stream >> cur_Opcode;

                if(cur_Opcode == "lw" || cur_Opcode == "sw") {
                    if (issueMem && !checkScoreboard(issue[i], scoreboards)) {
                        push_element(ALU1, issue[i]);
                        raiseScoreboard(issue[i], scoreboards);
                        pseudopop(issue, i);
                        i--;
                        issueMem = false;
                    } else {
                        //no other mem instructions can go if older stores still havent happened
                        if (cur_Opcode == "sw") {
                            issueMem = false;
                        }
                        raiseScoreboard(issue[i], scoreboards);
                    }
                }
                else{
                    if (issuearith && !checkScoreboard(issue[i], scoreboards)) {
                        push_element(ALU2, issue[i]);
                        raiseScoreboard(issue[i], scoreboards);
                        pseudopop(issue, i);
                        i--;
                        issuearith = false;
                    } else {
                        raiseScoreboard(issue[i], scoreboards);
                    }
                }
            }
        }



    //FETCH UNIT
    IFUnit[1] = "";
    string cur_Opcode;
    //check conditions for each instruction fetched
    for(int i = 0; i < 2; i++) {
    //if theres no stalled branch instruction
    if (IFUnit[0] == "") {
        //if theres an empty issue slot we can fetch
        if (empty_queueSlot(issue)) {
            string cur_Instruction = instructions[(address - 256) / 4];
            istringstream stream(cur_Instruction);
            stream >> cur_Opcode;

            if (cur_Opcode == "break") {
                //put break in exectued
                IFUnit[1] = "break";
                return true;
            }
            if (cur_Opcode == "jal" || cur_Opcode == "beq" || cur_Opcode == "bne" || cur_Opcode == "blt") {
                if(checkScoreboard(cur_Instruction, scoreboards)){
                    IFUnit[0] = cur_Instruction;
                }
                else {
                    execute_instruction(address, registers, Data, cur_Instruction);
                    IFUnit[1] = cur_Instruction;
                    address += 4;
                }
                //dont accept next instruction
                break;
            }
            else {
                //add to issue storage
                push_element(issue, cur_Instruction);
                raiseScoreboard(cur_Instruction, scoreboards);
                address += 4;
            }
        }
    }
    }
    if(IFUnit[0] != "") {
        if (!checkScoreboard(IFUnit[0], scoreboards)) {
            IFUnit[1] = IFUnit[0];
            execute_instruction(address, registers, Data, IFUnit[0]);
            IFUnit[0] = "";
            address += 4;
        }
    }

    for(int i = 0; i < clearInstructions.size(); i++) {
        lowerScoreboard(clearInstructions[i], scoreboards);
    }


    return false;
}

int main(int argc, char* argv[]) {

    ifstream input(argv[1]);

    if (!input.is_open()) {
        std::cout << "Invalid File Name";
        return 1;
    }

    ofstream output("disassembly.txt");


    string line;
    int address = 256;
    vector<string> instructions;

    // Read and disassemble all code up to break
    while (getline(input, line)) {
        output << line << "\t" << address << "\t";

        string disassembled_line = disassemble(line);
        instructions.emplace_back(disassembled_line);
        output << disassembled_line << endl;

        address += 4;
        if(disassembled_line == "break") {
            break;
        }
    }

    vector<tuple<int,int>> Data;
    //print out the two's complements
    while (getline(input, line)) {
        output << line << "\t" << address << "\t";
        int data = binaryToDecimal(line);
        output << data << endl;
        Data.emplace_back(address,data);
        address += 4;
    }

    input.close();
    output.close();

    //make 32 starting out as zero
    vector<int> registers;
    for(int i = 0; i < 32; i++) {
        registers.emplace_back(0);
    }

    bool broke = false;
    address = 256;
    ofstream outfile("simulation.txt");
    int cycle = 1;

    vector<string> IFUnit(2);
    vector<string> preIssue(4);
    vector<string> ALU1(2);
    vector<string> ALU2(2);
    vector<string> preMem(1);
    vector<string> postALU2(3);
    vector<string> postMEM(3);

    vector<vector<string>> WBqueues;
    WBqueues.push_back(postALU2);
    WBqueues.push_back(postMEM);

    vector<int> readScoreboard(32);
    vector<int> writeScoreboard(32);
    vector<vector<int>> scoreboards;
    scoreboards.push_back(readScoreboard);
    scoreboards.push_back(writeScoreboard);


    while (!broke) {
        //need old address cause simulate cycle could change the address which isn't good for the printout
        int oldAddress = address;
        broke = simulate_cycle(address, instructions, registers, Data, WBqueues, preMem, ALU1, ALU2, preIssue, IFUnit,scoreboards);
        if (cycle != 1) {
            outfile << endl;
        }
        simulation_printOut(outfile,cycle,oldAddress, registers, Data, WBqueues, preMem, ALU1, ALU2, preIssue, IFUnit);
        cycle++;
    }
    outfile.close();

    return 0;
}