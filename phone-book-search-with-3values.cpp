#include<bits/stdc++.h>
#include<mpi.h>


using namespace std;


string check(string id, string name, string phone_number, string search_name){
    if(name.find(search_name) != string::npos){
        return id + " " + name + " " + phone_number + "\n";
    }
    return "";
}
vector<string> string_to_vector(string &str){
    vector<string> names;
    istringstream iss(str);
    string line;
    while(getline(iss, line)){
        if(!line.empty()){
            names.push_back(line);
        }
    }
    return names;
}
string receive_string(int sender){
    int length;
    MPI_Recv(&length, 1, MPI_INT, sender, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    char *text = new char[length];
    MPI_Recv(text, length, MPI_CHAR, sender, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    string result(text);
    delete[] text;
    return result;
}
void send_string(const string &text, int receciver){
    int length = text.size() + 1;
    MPI_Send(&length, 1, MPI_INT, receciver, 1, MPI_COMM_WORLD);
    MPI_Send(text.c_str(), length, MPI_CHAR, receciver, 1, MPI_COMM_WORLD);
}
string vector_to_string(vector<string> &names, int start, int end){
    string text;
    for(int i = start; i < min((int)names.size(), end); i++){
        text += names[i];
        text += "\n";
    }
    return text;
}
void read_phonebook(vector<string> &fileNames, vector<string> &ids, vector<string> &names, vector<string> &phoneNumbers){
    for(auto file_name : fileNames){
        ifstream file(file_name);
        if(!file){
            cerr << "Error opening file : " << file_name << '\n';
            continue;
        }

        string line;
        while(getline(file, line)){
            line += ",";
            string str = "";
            vector<string> cleanString;
            for(int i = 0; i < line.size(); i++){
                if(line[i] == '\"') continue;
                else if(line[i] != '\"' and line[i] != ','){
                    str += line[i];
                }else{
                    cleanString.push_back(str);
                    str = "";
                }
            }
            ids.push_back(cleanString[0]);
            names.push_back(cleanString[1]);
            phoneNumbers.push_back(cleanString[2]);
        }
        file.close();
    }
}
int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if(argc < 3){
        if(world_rank == 0){
            cout << "Usage: mpirun -n <num_process> " << argv[0] << "<phonebook_file> ... <search_term>\n";
        }
        MPI_Finalize();
        return 1;
    }

    string search = "";
    for(int i = 2; i < argc; i++){
        int len = strlen(argv[i]);
        if(len >= 4 and argv[i][len - 1] == 't' and argv[i][len - 2] == 'x', argv[i][len - 3] == 't' and argv[i][len - 4] == '.'){
            continue;
        }else{
            if(search.size() > 0){
                search += " ";
            }
            search += argv[i];
        }
    }
    double startTime, endTime;

    if(world_rank == 0){
        vector<string> ids, names, phoneNumbers;
        vector<string> fileNames;
        cout << "I am a specialist\n";
        for(int i = 1; i < argc; i++){
            int len = strlen(argv[i]);
            if(len >= 4 and argv[i][len - 1] == 't' and argv[i][len - 2] == 'x', argv[i][len - 3] == 't' and argv[i][len - 4] == '.'){
                fileNames.push_back(argv[i]);
            }
        }

        
        read_phonebook(fileNames, ids, names, phoneNumbers);
        // for(int i = 0; i < (int)id.size(); i++){
        //     cout << id[i] << ' ' << names[i] << ' ' << phoneNumbers[i] << '\n';
        // }
        
        int totalContacts = names.size();
        int chunk = (totalContacts + world_size - 1) / world_size;

        for(int proc = 1; proc < world_size; proc++){
            int start = proc * chunk;
            int end = start + chunk;
            string ids_to_send = vector_to_string(ids, start, end);
            send_string(ids_to_send, proc);

            string names_to_send = vector_to_string(names, start, end);
            send_string(names_to_send, proc);

            string phones_to_send = vector_to_string(phoneNumbers, start, end);
            send_string(phones_to_send, proc);

            send_string(search, proc);
        }

        string local_results;
        startTime = MPI_Wtime();
        for(int i = 0; i < min(chunk, totalContacts); i++){
            string res = check(ids[i], names[i], phoneNumbers[i], search);
            if(!res.empty()){
                local_results += res;
            }
        }

        endTime = MPI_Wtime();

        for(int proc = 1; proc < world_size; proc++){
            string received = receive_string(proc);
            local_results += received;
        }

        cout << "Time taken to rank : " << world_rank << " is " << endTime - startTime << '\n';

        
        vector<string> finalResult = string_to_vector(local_results);

        ofstream file("result.txt");

        for(auto it : finalResult){
            file << it << '\n';
            //cout << it << '\n';
        }
        
    }
    else{
        string received_ids = receive_string(0);
        vector<string> ids = string_to_vector(received_ids);

        string received_names = receive_string(0);
        vector<string> names = string_to_vector(received_names);

        string received_phones = receive_string(0);
        vector<string> phone_numbers = string_to_vector(received_phones);
        
        string search = receive_string(0);

        string local_result;
        startTime = MPI_Wtime();
        for(size_t i = 0; i < names.size(); i++){
            string res = check(ids[i], names[i], phone_numbers[i], search);
            if(res.size() != 0){
                local_result += res;
            }
        }
        endTime = MPI_Wtime();
        cout << "Time taken to rank : " << world_rank << " is " << endTime - startTime << '\n';

        send_string(local_result, 0);
    }
    MPI_Finalize();
    return 0;
}