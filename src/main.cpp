////////////////////////////////////////
//     Copyright (c) 2021 fikret0     //
////////////////////////////////////////

#include <bits/stdc++.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <cpuid.h>
#include <experimental/filesystem>
#include <curl/curl.h>
#include <curl/easy.h>
using namespace std;

struct sysinfo sysInfo;

string clgreen = "\e[32m";
string clred = "\e[31m";
string clcyan = "\e[36m";
string clwhite = "\e[37m";
string clyellow = "\e[33m";
string clblue = "\e[34m";

string bold = "\e[1m";
string reset = "\e[0m";

string colorize(string text, string col, bool ifbold)
{
    stringstream stream;

    string currentColor;

    if(col == "green")
    {
        currentColor = clgreen;
    }
    else if(col == "red")
    {
        currentColor = clred;
    }
    else if(col == "cyan")
    {
        currentColor = clcyan;
    }
    else if(col == "white")
    {
        currentColor = clwhite;
    }
    else if(col == "yellow")
    {
        currentColor = clyellow;
    }
    else if(col == "blue")
    {
        currentColor = clblue;
    }

    if(ifbold)
    {
        stream << bold << currentColor << text << reset;
    }
    else
    {
        stream << currentColor << text << reset;
    }

    return stream.str();
}

string getUserName()
{
    uid_t uid = geteuid ();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        return string(pw->pw_name);
    }
    return {};
}

vector<string> splitstr(string origin, char delimiter)
{
    string text = origin + delimiter;

    stringstream stream;
    vector<string> strVector;

    for (auto c : text)
    {
        if (c == delimiter)
        {
            strVector.push_back(stream.str());
            stream.str("");
        }
        else
        {
            stream << c;
        }
    }

    return strVector;
}

string color = "";

string makefield(string tag, string text, string delimiter){
    string field;

    field = colorize(tag, color, true) + delimiter + text;

    return field;
}

vector<string> readlines(string path)
{
    fstream fileStream;
    fileStream.open(path, ios::in);

    vector<string> lines;

    if (fileStream.is_open())
    {
        string lc;

        while (getline(fileStream, lc))
        {
            lines.push_back(lc);
        }

        fileStream.close();
    }

    return lines;
}

string ltrim(string s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(),
            not1(ptr_fun<int, int>(isspace))));
    return s;
}

string rtrim(string s) {
    s.erase(find_if(s.rbegin(), s.rend(),
            not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

string trim(string s) {
    return ltrim(rtrim(s));
}

string getcpuinfo(){
    for(string mwe : readlines("/proc/cpuinfo")){
        vector<string> spp = splitstr(mwe, ':');
        if(trim(spp[0]) == "model name"){
            return trim(spp[1]);
        }
    }

    return "Unknown";
}

string getappdir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    string appPath = string(result, (count > 0) ? count : 0);

    size_t found = appPath.find_last_of("/\\");
    return appPath.substr(0, found);
}

string getraminfo(){
    int mb = 1024;

    string lstr = "";

    int total;
    int used;
    int free;

    for(string l : readlines("/proc/meminfo")){
        vector<string> jgx = splitstr(l, ':');
        if(trim(jgx[0]) == "MemTotal"){
            total = stoi(splitstr(trim(jgx[1]), ' ')[0]) / mb;
        }

        if(trim(jgx[0]) == "MemAvailable"){
            free = stoi(splitstr(trim(jgx[1]), ' ')[0]) / mb;
        }
    }

    used = total - free;

    lstr = to_string(used) + "MiB / " + to_string(total) + "MiB";

    return lstr;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    string data((const char*) ptr, (size_t) size * nmemb);
    *((stringstream*) stream) << data << endl;
    return size * nmemb;
}

string downloadversion(){
    void* curl;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/fikret0/sjfetch/main/ver/version");

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    std::stringstream out;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cout << ":: Error while checking version: " << curl_easy_strerror(res) << endl;
    }

    return out.str();
}

string ver = "3.3-edge";

int main(int argc, char** argv){
    if(argc > 1){
        string option = argv[1];
        if(option == "--update" || option == "-u"){
            if(trim(downloadversion()) == ver){
                cout << colorize("::", "cyan", true) << colorize(" sjfetch is up-to-date.", "white", true) << endl;
            }
            else{
                cout << colorize("::", "cyan", true) << colorize(" Updating...", "white", true) << endl;
                system("sudo sh -c \"$(curl -fsSL https://raw.githubusercontent.com/fikret0/sjfetch/main/netinstall.sh)\"");
            }

            return 0;
        }
        else if(option == "-v" || option == "--version"){
            cout << ver << endl;
            return 0;
        }
    }

    if(experimental::filesystem::exists("/etc/os-release")){
        string apppath = getappdir() + "/";

        string name = "";
        string logo = "";
        string buildid = "Unknown";

        vector<string> idf = readlines("/etc/os-release");

        for(string l : idf){
            vector<string> ii = splitstr(l, '=');

            if(ii[0] == "NAME"){
                name = ii[1];
                name.erase(name.end() - 1);
                name.erase(0, 1);
            }

            if(ii[0] == "ID"){
                logo = ii[1];
            }

            if(ii[0] == "BUILD_ID"){
                buildid = ii[1];
            }
        }

        string logopath = "";

        if(argc == 1){
            if(experimental::filesystem::exists(apppath + "ascii/" + logo + ".ascii")){
                logopath = apppath + "ascii/" + logo + ".ascii";
            }
            else{
                logopath = apppath + "ascii/default.ascii";
            }
        }
        else{
            if(experimental::filesystem::exists(apppath + "ascii/" + argv[1] + ".ascii")){
                logopath = apppath + "ascii/" + argv[1] + ".ascii";
            }
            else{
                cout << "Distro " << colorize(argv[1], "red", true) << " not found." << endl;
                exit(-1);
            }
        }

        vector<string> logolines = readlines(logopath);

        color = logolines[0];
        logolines.erase(logolines.begin());

        vector<string> finallines;

        regex ansiexp("(?:\x1B[@-_]|[\x80-\x9F])[0-?]*[ -/]*[@-~]");

        int size = 0;
        for(string l : logolines){
            size = max(size, (int)(regex_replace(l, ansiexp, "").size()));
        }
        
        size += 2;

        for(string logoline : logolines){
            string fnllg = logoline;
            string nonascii = regex_replace(logoline, ansiexp, "");

            int spacesize = size - nonascii.size();
            for(int i = 0; i < spacesize; i++){
                fnllg += " ";
            }

            finallines.push_back(colorize(fnllg, "", true));
        }

        string username = getUserName();  
        string hostname = readlines("/etc/hostname")[0];
        finallines[0] += colorize(username, color, true) + "@" + colorize(hostname, color, true);

        for(int xff = 0; xff < (username + "@" + hostname).size(); xff++){
            finallines[1] += "-";
        }

        string defshell = "";
        for(string l : readlines("/etc/passwd")){
            vector<string> jgx = splitstr(l, ':');
            if(jgx[0] == username){ // fikret:x:1000:1000::/home/fikret:/usr/bin/zsh
                defshell = splitstr(jgx[6], '/').back(); 
            }
        }

        sysinfo(&sysInfo);

        string cpuinfostr = getcpuinfo();
        string cpbrand = trim(splitstr(cpuinfostr, '@')[0]);
        string cpclocks = trim(splitstr(cpuinfostr, '@')[1]);
        
        finallines[2] += makefield("OS", name, ": ");
        finallines[3] += makefield("Build ID", buildid, ": ");

        finallines[5] += makefield("Uptime", to_string(sysInfo.uptime / (60 * 60)) + " h", ": ");
        finallines[6] += makefield("Memory", getraminfo(), ": ");
        finallines[7] += makefield("Total Running Processes", to_string(sysInfo.procs), ": ");

        finallines[9] += makefield("Shell", defshell, ": ");
        finallines[10] += makefield("Desktop session", getenv("DESKTOP_SESSION"), ": ");
        
        finallines[12] += makefield("CPU", regex_replace(cpbrand, regex(" CPU"), ""), ": ");
        finallines[13] += makefield("Clock speed", cpclocks, ": ");
        
        for(string fn : finallines){
            cout << fn << endl;
        }
    }
    else{
        cout << "No " << colorize("operating system identifier", "white", true) << " found." << endl;
        exit(-1);
    }

    return 0;
}   